#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include "constant.h"
#include "Map.h"

// Player ki mojooda position, rukh (angle) aur halat (states) ke variables
sf::Vector2f playerPosition(0.0f, 0.0f);
float playerAngle = 0.0f;
bool hittingObstacle = false;
bool wallImpact = false;
int currentCharacter = 0;

// Game shuru hone par player ko uski shuruati jagah aur character ke sath set karne ka function
void setupPlayer(int characterID, sf::Vector2f startPosition)
{
    if (characterID >= 0 && characterID < CHARACTER_COUNT)
    {
        currentCharacter = characterID;
    }

    playerPosition = startPosition;
    playerAngle = 0.0f;
    hittingObstacle = false;
    wallImpact = false;
}

// Kisi bhi di gayi jagah par rider ka collision hitbox calculate karta hai
sf::FloatRect getPlayerBoundsAt(float centerX, float centerY)
{
    const float size = RIDER_HITBOX_SIZE[currentCharacter];

    return sf::FloatRect(
        centerX - size / 2.0f,
        centerY - size / 2.0f,
        size,
        size
    );
}

// Player ki mojooda jagah ka hitbox wapas karta hai
sf::FloatRect getPlayerBounds()
{
    return getPlayerBoundsAt(playerPosition.x, playerPosition.y);
}

// Check karta hai ke player nayi jagah par ja sakta hai ya koi deewar/park rasta rok rahi hai
bool canPlayerMoveTo(float newX, float newY)
{
    const sf::FloatRect box = getPlayerBoundsAt(newX, newY);

    // Pehle check karein ke player map ki boundary se bahar to nahi nikal raha
    if (box.left < 0.0f || box.top < 0.0f ||
        box.left + box.width > MAP_PIXEL_W ||
        box.top + box.height > MAP_PIXEL_H)
    {
        return false;
    }

    // Hitbox ke chaaron konon (corners) ke grid rows aur columns nikalein
    const int firstRow = box.top / TILE_SIZE;
    const int lastRow = (box.top + box.height - 1.0f) / TILE_SIZE;
    const int firstCol = box.left / TILE_SIZE;
    const int lastCol = (box.left + box.width - 1.0f) / TILE_SIZE;

    // Agar chaaron konon mein se kahin bhi solid tile nahi hai, to rasta saaf hai
    return !isSolidTile(firstRow, firstCol) &&
           !isSolidTile(firstRow, lastCol) &&
           !isSolidTile(lastRow, firstCol) &&
           !isSolidTile(lastRow, lastCol);
}

// Check karta hai ke player kisi rukawat (pothole/obstacle) ke upar se guzar raha hai ya nahi
bool isPlayerOnObstacle()
{
    const sf::FloatRect box = getPlayerBounds();

    const int firstRow = box.top / TILE_SIZE;
    const int lastRow = (box.top + box.height - 1.0f) / TILE_SIZE;
    const int firstCol = box.left / TILE_SIZE;
    const int lastCol = (box.left + box.width - 1.0f) / TILE_SIZE;

    for (int row = firstRow; row <= lastRow; row++)
    {
        for (int col = firstCol; col <= lastCol; col++)
        {
            if (row >= 0 && row < MAP_ROWS &&
                col >= 0 && col < MAP_COLS &&
                cityGrid[row][col] == TILE_OBSTACLE)
            {
                return true;
            }
        }
    }

    return false;
}

// Keyboard inputs ke mutabiq player ki harkat (movement) aur speed update karne ka function
void updatePlayerMovement(float deltaTime)
{
    float moveX = 0.0f;
    float moveY = 0.0f;

    // WASD ya Arrow keys se direction input lein
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        moveY = -1.0f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        moveY = 1.0f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        moveX = -1.0f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        moveX = 1.0f;
    }

    const float length = std::sqrt(moveX * moveX + moveY * moveY);

    // Tirchi (diagonal) movement ki speed barabar rakhne ke liye normalize karein aur angle set karein
    if (length > 0.0f)
    {
        moveX = moveX / length;
        moveY = moveY / length;

        if (moveX > 0.0f)
            playerAngle = 90.0f;
        else if (moveX < 0.0f)
            playerAngle = 270.0f;
        else if (moveY > 0.0f)
            playerAngle = 180.0f;
        else
            playerAngle = 0.0f;
    }

    hittingObstacle = isPlayerOnObstacle();

    float speed = RIDER_MAX_SPEED[currentCharacter];

    // Agar rider khadde/rukawat ke upar hai to uski speed kam kar dein
    if (hittingObstacle)
    {
        speed = speed * RIDER_OBSTACLE_FACTOR[currentCharacter];
    }

    const float distance = speed * deltaTime;
    const float newX = playerPosition.x + moveX * distance;
    const float newY = playerPosition.y + moveY * distance;

    wallImpact = false;

    // X aur Y axis ko alag alag move karein taake rider deewar ke sath phisal (slide) sake
    if (moveX != 0.0f)
    {
        if (canPlayerMoveTo(newX, playerPosition.y))
            playerPosition.x = newX;
        else
            wallImpact = true;
    }

    if (moveY != 0.0f)
    {
        if (canPlayerMoveTo(playerPosition.x, newY))
            playerPosition.y = newY;
        else
            wallImpact = true;
    }

    hittingObstacle = isPlayerOnObstacle();
}

// Screen par rider ki sawari (vehicle) aur uska saya draw karne ka function
void drawPlayer(sf::RenderWindow& window, sf::Sprite vehicleSprite)
{
    const float size = RIDER_HITBOX_SIZE[currentCharacter];
    const sf::IntRect textureArea = vehicleSprite.getTextureRect();

    // Sprite ka center point set karein taake rotation bilkul darmiyan se ho
    vehicleSprite.setOrigin(textureArea.width / 2.0f, textureArea.height / 2.0f);
    vehicleSprite.setPosition(playerPosition);

    // Teeno sawariyon ki tasveeron ka size ek jaisa rakhein
    vehicleSprite.setScale(0.10f, 0.10f);

    float imageAngle = playerAngle - 90.0f;

    // Beamer (BMW) ki tasveer side-view hai, is liye upar/neeche jate waqt usay teda na karein
    if (currentCharacter == 2)
    {
        imageAngle = playerAngle == 90.0f ? 180.0f : 0.0f;
    }

    vehicleSprite.setRotation(imageAngle);

    // Sawari ke neeche halka saya draw karein
    sf::CircleShape shadow(size * 0.72f);
    shadow.setOrigin(size * 0.72f, size * 0.72f);
    shadow.setScale(1.25f, 0.55f);
    shadow.setPosition(playerPosition.x + 3.0f, playerPosition.y + 7.0f);
    shadow.setFillColor(sf::Color(10, 16, 23, 115));
    window.draw(shadow);

    window.draw(vehicleSprite);
}

#endif

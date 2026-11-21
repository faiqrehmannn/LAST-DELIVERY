#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include "constant.h"
#include "Map.h"

// Rider ki current position aur direction
sf::Vector2f playerPosition(0.0f, 0.0f);
float playerAngle = 0.0f;
int currentCharacter = 0;

// Pothole aur wall se contact ka status
bool hittingObstacle = false;
    const float oldPlayerX = playerPosition.x;
    const float oldPlayerY = playerPosition.y;
bool wallImpact = false;

// Rider ko selected character aur starting position deta hai
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

// Player ki collision box ko di hui position par calculate karta hai
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

// Player ki current collision box deta hai
sf::FloatRect getPlayerBounds()
{
    return getPlayerBoundsAt(playerPosition.x, playerPosition.y);
}

// Check karta hai ke nayi position par player kisi solid tile se takrayega ya nahi
bool canPlayerMoveTo(float newX, float newY)
{
    const sf::FloatRect box = getPlayerBoundsAt(newX, newY);

    if (box.left < 0.0f ||
        box.top < 0.0f ||
        box.left + box.width > MAP_PIXEL_W ||
        box.top + box.height > MAP_PIXEL_H)
    {
        return false;
    }

    const int firstRow = box.top / TILE_SIZE;
    const int lastRow =
        (box.top + box.height - 1.0f) / TILE_SIZE;
    const int firstCol = box.left / TILE_SIZE;
    const int lastCol =
        (box.left + box.width - 1.0f) / TILE_SIZE;

    for (int row = firstRow; row <= lastRow; row++)
    {
        for (int col = firstCol; col <= lastCol; col++)
        {
            if (isSolidTile(row, col))
            {
                return false;
            }
        }
    }

    return true;
}

    const int firstRow = box.top / TILE_SIZE;
    const int lastRow = (box.top + box.height - 1.0f) / TILE_SIZE;
    const int firstCol = box.left / TILE_SIZE;
    const int lastCol = (box.left + box.width - 1.0f) / TILE_SIZE;

    // Collision box ke char corners check hote hain
    return !isSolidTile(firstRow, firstCol) &&
           !isSolidTile(firstRow, lastCol) &&
           !isSolidTile(lastRow, firstCol) &&
           !isSolidTile(lastRow, lastCol);
}

// Check karta hai ke rider pothole tile par hai ya nahi
bool isPlayerOnObstacleAt(float centerX, float centerY)
{
    const sf::FloatRect box =
        getPlayerBoundsAt(centerX, centerY);

    const int firstRow = box.top / TILE_SIZE;
    const int lastRow =
        (box.top + box.height - 1.0f) / TILE_SIZE;
    const int firstCol = box.left / TILE_SIZE;
    const int lastCol =
        (box.left + box.width - 1.0f) / TILE_SIZE;

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

bool isPlayerOnObstacle()
{
    return isPlayerOnObstacleAt(
        playerPosition.x,
        playerPosition.y
    );
}

// Keyboard input ke mutabiq rider move karta hai
void updatePlayerMovement(float deltaTime)
{
    float moveX = 0.0f;
    float moveY = 0.0f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        moveY -= 1.0f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        moveY += 1.0f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        moveX -= 1.0f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        moveX += 1.0f;
    }
    // Diagonal movement ko normalize karta hai taake speed zyada na ho
    const float length = std::sqrt(moveX * moveX + moveY * moveY);

    if (length > 0.0f)
    {
        moveX = moveX / length;
        moveY = moveY / length;
    }

       hittingObstacle = isPlayerOnObstacle();

    float speed = RIDER_MAX_SPEED[currentCharacter];

    const float normalDistance = speed * deltaTime;
    const float predictedX =
        playerPosition.x + moveX * normalDistance;
    const float predictedY =
        playerPosition.y + moveY * normalDistance;

    const bool enteringObstacle =
        isPlayerOnObstacleAt(predictedX, predictedY);

    if (hittingObstacle || enteringObstacle)
    {
        speed = speed * RIDER_OBSTACLE_FACTOR[currentCharacter];
    }

    const float distance = speed * deltaTime;
    const float newX = playerPosition.x + moveX * distance;
    const float newY = playerPosition.y + moveY * distance;
    wallImpact = false;

    // X aur Y alag move karne se wall ke saath slide karna mumkin hota hai
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
    const float actualMoveX = playerPosition.x - oldPlayerX;
    const float actualMoveY = playerPosition.y - oldPlayerY;

    if (actualMoveX != 0.0f || actualMoveY != 0.0f)
    {
        if (actualMoveX > 0.0f && actualMoveY < 0.0f)
            playerAngle = 45.0f;
        else if (actualMoveX > 0.0f && actualMoveY > 0.0f)
            playerAngle = 135.0f;
        else if (actualMoveX < 0.0f && actualMoveY > 0.0f)
            playerAngle = 225.0f;
        else if (actualMoveX < 0.0f && actualMoveY < 0.0f)
            playerAngle = 315.0f;
        else if (actualMoveX > 0.0f)
            playerAngle = 90.0f;
        else if (actualMoveX < 0.0f)
            playerAngle = 270.0f;
        else if (actualMoveY > 0.0f)
            playerAngle = 180.0f;
        else
            playerAngle = 0.0f;
    }
    hittingObstacle = isPlayerOnObstacle();
}

// Rider aur uske neeche shadow draw karta hai
void drawPlayer(sf::RenderWindow& window, sf::Sprite vehicleSprite)
{
    const float size = RIDER_HITBOX_SIZE[currentCharacter];
    const sf::IntRect textureArea = vehicleSprite.getTextureRect();

    vehicleSprite.setOrigin(
        textureArea.width / 2.0f,
        textureArea.height / 2.0f
    );

    vehicleSprite.setPosition(playerPosition);

    // Teeno vehicles ko same visual scale par rakhta hai
    vehicleSprite.setScale(0.10f, 0.10f);

    float imageAngle = playerAngle - 90.0f;

    if (currentCharacter == 2)
    {
        // Beamer side-view image hai; vertical travel par usay side par nahi ghumate
        imageAngle = playerAngle == 90.0f ? 180.0f : 0.0f;
    }

    vehicleSprite.setRotation(imageAngle);

    sf::CircleShape shadow(size * 0.72f);
    shadow.setOrigin(size * 0.72f, size * 0.72f);
    shadow.setScale(1.25f, 0.55f);
    shadow.setPosition(
        playerPosition.x + 3.0f,
        playerPosition.y + 7.0f
    );
    shadow.setFillColor(sf::Color(10, 16, 23, 115));
    window.draw(shadow);

    window.draw(vehicleSprite);
}

#endif

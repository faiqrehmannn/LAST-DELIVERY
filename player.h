#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include "constant.h"
#include "Map.h"

// Player position and state variables
static sf::Vector2f playerPosition(0.0f, 0.0f);
static float playerAngle = 0.0f;
static bool hittingObstacle = false;
static int currentCharacter = 0;

static void setupPlayer(int characterID, sf::Vector2f startPosition)
{
    if (characterID >= 0 && characterID < CHARACTER_COUNT)
        currentCharacter = characterID;
    playerPosition = startPosition;
    playerAngle = 0.0f;
    hittingObstacle = false;
}

static sf::FloatRect getPlayerBoundsAt(float centerX, float centerY)
{
    const float size = RIDER_HITBOX_SIZE[currentCharacter];
    return sf::FloatRect(centerX - size / 2.0f, centerY - size / 2.0f,
                         size, size);
}

static sf::FloatRect getPlayerBounds()
{
    return getPlayerBoundsAt(playerPosition.x, playerPosition.y);
}

static bool canPlayerMoveTo(float newX, float newY)
{
    const sf::FloatRect box = getPlayerBoundsAt(newX, newY);
    const float left = box.left;
    const float right = box.left + box.width - 1.0f;
    const float top = box.top;
    const float bottom = box.top + box.height - 1.0f;
    const int row1 = top / TILE_SIZE;
    const int row2 = bottom / TILE_SIZE;
    const int col1 = left / TILE_SIZE;
    const int col2 = right / TILE_SIZE;

    return !isSolidTile(row1, col1) && !isSolidTile(row1, col2) &&
           !isSolidTile(row2, col1) && !isSolidTile(row2, col2);
}

static bool isPlayerOnObstacle()
{
    const sf::FloatRect box = getPlayerBounds();
    const int firstRow = box.top / TILE_SIZE;
    const int lastRow = (box.top + box.height - 1) / TILE_SIZE;
    const int firstCol = box.left / TILE_SIZE;
    const int lastCol = (box.left + box.width - 1) / TILE_SIZE;

    for (int row = firstRow; row <= lastRow; row++)
    {
        for (int col = firstCol; col <= lastCol; col++)
        {
            if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS &&
                cityGrid[row][col] == TILE_OBSTACLE)
                return true;
        }
    }
    return false;
}

static bool updatePlayerMovement(float deltaTime)
{
    float moveX = 0.0f;
    float moveY = 0.0f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) moveY = -1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) moveY = 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) moveX = -1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) moveX = 1.0f;

    const float length = std::sqrt(moveX * moveX + moveY * moveY);
    if (length > 0.0f)
    {
        moveX /= length;
        moveY /= length;
        if (moveX > 0.0f) playerAngle = 90.0f;
        else if (moveX < 0.0f) playerAngle = 270.0f;
        else if (moveY > 0.0f) playerAngle = 180.0f;
        else if (moveY < 0.0f) playerAngle = 0.0f;
    }

    hittingObstacle = isPlayerOnObstacle();
    float speed = RIDER_MAX_SPEED[currentCharacter];
    if (hittingObstacle) speed *= RIDER_OBSTACLE_FACTOR[currentCharacter];
    const float distance = speed * deltaTime;
    const float newX = playerPosition.x + moveX * distance;
    const float newY = playerPosition.y + moveY * distance;

    bool bumpedIntoWall = false;
    if (canPlayerMoveTo(newX, playerPosition.y))
        playerPosition.x = newX;
    else if (moveX != 0.0f)
        bumpedIntoWall = true;

    if (canPlayerMoveTo(playerPosition.x, newY))
        playerPosition.y = newY;
    else if (moveY != 0.0f)
        bumpedIntoWall = true;
    hittingObstacle = isPlayerOnObstacle();
    return bumpedIntoWall;
}

static void drawPlayer(sf::RenderWindow& window, sf::Sprite vehicleSprite)
{
    const float size = RIDER_HITBOX_SIZE[currentCharacter];
    const sf::IntRect textureArea = vehicleSprite.getTextureRect();
    vehicleSprite.setOrigin(textureArea.width / 2.0f, textureArea.height / 2.0f);
    vehicleSprite.setPosition(playerPosition);
    const float vehicleScale = currentCharacter == 2 ? 0.10f : 0.14f;
    vehicleSprite.setScale(vehicleScale, vehicleScale);

    float imageAngle = playerAngle - 90.0f;
    if (currentCharacter == 2) imageAngle = playerAngle + 90.0f;
    vehicleSprite.setRotation(imageAngle);

    sf::CircleShape shadow(size * 0.72f);
    shadow.setOrigin(size * 0.72f, size * 0.72f);
    shadow.setScale(1.25f, 0.55f);
    shadow.setPosition(playerPosition.x + 3.0f, playerPosition.y + 7.0f);
    shadow.setFillColor(sf::Color(10, 16, 23, 115));
    window.draw(shadow);
    window.draw(vehicleSprite);
}

#endif

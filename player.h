#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include "constant.h"
#include "Map.h"

// classes use nahi ki simple variables se player system chal rha hai
static sf::Vector2f playerPosition(0.0f, 0.0f);
static float playerAngle = 0.0f;
static bool hittingObstacle = false;
static int currentCharacter = 0;

static void setupPlayer(int characterID, sf::Vector2f startPosition)
{
    // index validation boundaries check laazmi hai
    if (characterID >= 0 && characterID < CHARACTER_COUNT) {
        currentCharacter = characterID;
    }
    playerPosition = startPosition;
    playerAngle = 0.0f;
    hittingObstacle = false;
}

static sf::FloatRect getPlayerBoundsAt(float centerX, float centerY)
{
    const float size = RIDER_HITBOX_SIZE[currentCharacter];
    return sf::FloatRect(centerX - size / 2.0f, centerY - size / 2.0f, size, size);
}

static sf::FloatRect getPlayerBounds()
{
    return getPlayerBoundsAt(playerPosition.x, playerPosition.y);
}

// collision calculation checking node - returns true if path is clear
static bool canPlayerMoveTo(float newX, float newY)
{
    const sf::FloatRect box = getPlayerBoundsAt(newX, newY);
    const float left = box.left;
    const float right = box.left + box.width - 1.0f;
    const float top = box.top;
    const float bottom = box.top + box.height - 1.0f;
    
    // float locations ko index grid math coordinates mein convert krna hai floor se
    const int row1 = (int)std::floor(top / TILE_SIZE);
    const int row2 = (int)std::floor(bottom / TILE_SIZE);
    const int col1 = (int)std::floor(left / TILE_SIZE);
    const int col2 = (int)std::floor(right / TILE_SIZE);

    // agar ek bhi tile solid building ya park nikli tou false return hoga
    return !isSolidTile(row1, col1) && !isSolidTile(row1, col2) &&
           !isSolidTile(row2, col1) && !isSolidTile(row2, col2);
}

// check if bounding box hits any pothole / obstacle tile nodes
static bool isPlayerOnObstacle()
{
    const sf::FloatRect box = getPlayerBounds();
    const int firstRow = (int)std::floor(box.top / TILE_SIZE);
    const int lastRow = (int)std::floor((box.top + box.height - 1) / TILE_SIZE);
    const int firstCol = (int)std::floor(box.left / TILE_SIZE);
    const int lastCol = (int)std::floor((box.left + box.width - 1) / TILE_SIZE);

    for (int row = firstRow; row <= lastRow; row++)
    {
        for (int col = firstCol; col <= lastCol; col++)
        {
            if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS &&
                cityGrid[row][col] == TILE_OBSTACLE) return true; // pothole mil gaya sarak pe
        }
    }
    return false;
}

// main loop logic node for input parsing and moving character arrays
static void updatePlayerMovement(float deltaTime)
{
    float moveX = 0.0f;
    float moveY = 0.0f;

    // wasd and arrow keys input layout checks
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) moveY = -1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) moveY = 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) moveX = -1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) moveX = 1.0f;

    // normalization vector math formula for diagonal uniform speed tracking
    const float length = std::sqrt(moveX * moveX + moveY * moveY);
    if (length > 0.0f)
    {
        moveX /= length;
        moveY /= length;
        
        // facing direction angle set kr rhe hain rotation processing k liye
        if (moveX > 0.0f) playerAngle = 90.0f;
        else if (moveX < 0.0f) playerAngle = 270.0f;
        else if (moveY > 0.0f) playerAngle = 180.0f;
        else if (moveY < 0.0f) playerAngle = 0.0f;
    }

    hittingObstacle = isPlayerOnObstacle();
    float speed = RIDER_MAX_SPEED[currentCharacter];
    
    // gaddhe me speed slow hojayegi factor multiply kr k
    if (hittingObstacle) {
        speed *= RIDER_OBSTACLE_FACTOR[currentCharacter];
    }
    
    const float distance = speed * deltaTime;
    const float newX = playerPosition.x + moveX * distance;
    const float newY = playerPosition.y + moveY * distance;

    // sliding physics mechanism - axis separately split checks so user slides nicely along borders
    if (canPlayerMoveTo(newX, playerPosition.y)) playerPosition.x = newX;
    if (canPlayerMoveTo(playerPosition.x, newY)) playerPosition.y = newY;
    
    hittingObstacle = isPlayerOnObstacle();
}

// temp placeholder rendering code layer: vehicle is drawn as custom shape graphics notes
static void drawPlayer(sf::RenderWindow& window)
{
    const float size = RIDER_HITBOX_SIZE[currentCharacter];

    // realistic drop drop ground shadow overlay layer
    sf::CircleShape shadow(size * 0.72f);
    shadow.setOrigin(size * 0.72f, size * 0.72f);
    shadow.setScale(1.25f, 0.55f);
    shadow.setPosition(playerPosition.x + 3.0f, playerPosition.y + 7.0f);
    shadow.setFillColor(sf::Color(10, 16, 23, 115));
    window.draw(shadow);

    // dynamic player rect body node matching chosen rider profile arrays
    sf::RectangleShape body(sf::Vector2f(size, size));
    body.setOrigin(size / 2.0f, size / 2.0f);
    body.setPosition(playerPosition);
    body.setFillColor(RIDER_PRIMARY_COLOR[currentCharacter]);
    body.setOutlineThickness(2.0f);
    body.setOutlineColor(RIDER_ACCENT_COLOR[currentCharacter]);
    window.draw(body);

    // head direction indicator calculations point setup node formulas
    const float radians = playerAngle * 3.14159265f / 180.0f;
    const float dirX = std::sin(radians);
    const float dirY = -std::cos(radians);
    const float noseRadius = size * 0.16f;
    
    sf::CircleShape nose(noseRadius);
    nose.setOrigin(noseRadius, noseRadius);
    nose.setPosition(playerPosition.x + dirX * size * 0.5f,
                     playerPosition.y + dirY * size * 0.5f);
    nose.setFillColor(sf::Color(255, 220, 133)); // cream tone dot color indicators
    window.draw(nose);
}

#endif

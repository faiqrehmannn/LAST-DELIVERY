#pragma once

#include <SFML/Graphics.hpp>
#include <string>

// Window and map dimensions
const int TILE_SIZE = 48;
const int MAP_COLS = 20;
const int MAP_ROWS = 15;
const int MAP_PIXEL_W = TILE_SIZE * MAP_COLS;
const int MAP_PIXEL_H = TILE_SIZE * MAP_ROWS;
const int WINDOW_W = 1376;
const int WINDOW_H = 768;

// Tile codes
const int TILE_ROAD = 0;
const int TILE_BUILDING = 1;
const int TILE_PARK = 2;
const int TILE_SHOP = 3;
const int TILE_HOUSE = 4;
const int TILE_OBSTACLE = 5;

// Game states
enum GameState {
    STATE_MENU,
    STATE_CHAR_SELECT,
    STATE_INSTRUCTIONS,
    STATE_LEADERBOARD,
    STATE_MUSIC,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER,
    STATE_VICTORY
};

// Character data is stored in matching fixed-size arrays.
const int CHARACTER_COUNT = 3;
static const std::string RIDER_NAMES[CHARACTER_COUNT] = {
    "Noor", "Khan", "Jimmy"
};
static const std::string VEHICLE_NAMES[CHARACTER_COUNT] = {
    "City Bicycle", "Motorcycle", "Beamer (BMW)"
};
static const bool RIDER_IS_FEMALE[CHARACTER_COUNT] = {true, false, false};
static const float RIDER_MAX_SPEED[CHARACTER_COUNT] = {170.0f, 230.0f, 285.0f};
static const float RIDER_HITBOX_SIZE[CHARACTER_COUNT] = {22.0f, 28.0f, 34.0f};
static const float RIDER_SCORE_MULTIPLIER[CHARACTER_COUNT] = {1.0f, 1.25f, 1.5f};
static const float RIDER_OBSTACLE_FACTOR[CHARACTER_COUNT] = {0.80f, 0.55f, 0.55f};
static const sf::Color RIDER_PRIMARY_COLOR[CHARACTER_COUNT] = {
    sf::Color::Cyan, sf::Color::Red, sf::Color::Blue
};
static const sf::Color RIDER_ACCENT_COLOR[CHARACTER_COUNT] = {
    sf::Color::White, sf::Color::Black, sf::Color::White
};

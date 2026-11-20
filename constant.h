#pragma once

#include <SFML/Graphics.hpp>
#include <string>

// City map ke ek square (tile) ka size pixels mein
const int TILE_SIZE = 48;

// Map mein columns (chorai) aur rows (lambai) ki tadaad
const int MAP_COLS = 20;
const int MAP_ROWS = 15;

// Poore city map ka kul size pixels mein
const int MAP_PIXEL_W = TILE_SIZE * MAP_COLS;
const int MAP_PIXEL_H = TILE_SIZE * MAP_ROWS;

// Poori game window ka size, side panel ko mila kar
const int WINDOW_W = 1376;
const int WINDOW_H = 768;

// Map ke mukhtalif hisson (tiles) ko pehchanne ke liye number codes
const int TILE_ROAD = 0;
const int TILE_BUILDING = 1;
const int TILE_PARK = 2;
const int TILE_SHOP = 3;
const int TILE_HOUSE = 4;
const int TILE_OBSTACLE = 5;

// Game ki mukhtalif screens aur states jo main.cpp mein use hoti hain
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

// Khelne ke liye mojood total riders ki tadaad
const int CHARACTER_COUNT = 3;

// Riders ke naam, character-selection screen ki tarteeb ke mutabiq
const std::string RIDER_NAMES[CHARACTER_COUNT] = {
    "Noor",
    "Khan",
    "Jimmy"
};

// Har rider ki sawari (vehicle) ka naam, usi tarteeb se
const std::string VEHICLE_NAMES[CHARACTER_COUNT] = {
    "City Bicycle",
    "Motorcycle",
    "Beamer (BMW)"
};

// True ka matlab rider female hai, aur false ka matlab male
const bool RIDER_IS_FEMALE[CHARACTER_COUNT] = {
    true,
    false,
    false
};

// Har rider ki zyada se zyada raftaar (max speed)
const float RIDER_MAX_SPEED[CHARACTER_COUNT] = {
    170.0f,
    230.0f,
    285.0f
};

// Takrao (collision) check karne ke liye har rider ke hitbox ka size
const float RIDER_HITBOX_SIZE[CHARACTER_COUNT] = {
    22.0f,
    28.0f,
    34.0f
};

// Har rider ke liye score barhane ka bonus multiplier
const float RIDER_SCORE_MULTIPLIER[CHARACTER_COUNT] = {
    1.0f,
    1.25f,
    1.5f
};

// Kisi rukawat (obstacle) ke upar se guzarte waqt kitni speed baqi rehti hai
const float RIDER_OBSTACLE_FACTOR[CHARACTER_COUNT] = {
    0.80f,
    0.55f,
    0.55f
};

// Har rider ko draw karne ke liye bunyadi (primary) rang
const sf::Color RIDER_PRIMARY_COLOR[CHARACTER_COUNT] = {
    sf::Color::Cyan,
    sf::Color::Red,
    sf::Color::Blue
};

// Har rider ke design ke liye secondary (accent) rang
const sf::Color RIDER_ACCENT_COLOR[CHARACTER_COUNT] = {
    sf::Color::White,
    sf::Color::Black,
    sf::Color::White
};

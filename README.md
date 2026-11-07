# Last Delivery

A 2D top-down delivery game written in C++ using SFML.

Players choose a courier and vehicle, drive around a grid-based city, pick up delivery orders, and deliver them before time runs out.

## Team

| Name | Roll number | Main responsibility |
|---|---|---|
| Faiq Aziz | 26L-0786 | City map, core engine, file handling, and leaderboard |
| Noor Fatima | 26L-0780 | Characters, vehicles, movement, and collision |
| Abdul Baseer Khan | 26L-0753 | Orders, timers, scoring, and user interface |

## Characters and vehicles

| Courier | Vehicle | Speed | Hitbox | Score multiplier |
|---|---|---:|---:|---:|
| Noor | Cycle | 170 px/s | 22 px | 1.0× |
| Khan | Harley Davidson | 230 px/s | 28 px | 1.25× |
| Jimmy | Beamer (BMW) | 285 px/s | 34 px | 1.5× |

## Project files

| File | Purpose |
|---|---|
| `Constants.h` | Shared game dimensions, tile codes, states, and character profiles |
| `Map.h` | City grid, map tile checks, and map drawing |
| `Player.h` | Vehicle movement, drawing, and collision |
| `Order.h` | Delivery orders, countdowns, and scoring |
| `UIAndFile.h` | Screens, HUD, minimap, and high-score file handling |
| `main.cpp` | Creates the game window and connects the game systems |
| `highscores.txt` | Saved high scores |

## Build requirements

- A C++ compiler
- SFML 2.x development libraries
- The SFML package must match the compiler and system architecture

## Build and run

The exact command depends on your compiler and where SFML is installed. With a compatible GCC/MinGW setup and SFML 2, the general form is:

```bash
g++ main.cpp -o LastDelivery -lsfml-graphics -lsfml-window -lsfml-system
./LastDelivery

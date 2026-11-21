# Last Delivery

A 2D courier game built in C++11 with SFML 2.6.2. Choose a rider, navigate a tile-based city, pick up delivery orders, and reach each customer before the timer runs out. Complete deliveries to earn points and try to set a high score.

## Features

- Three riders with different vehicles, movement speeds, hitboxes, and score multipliers
- A grid-based city with roads, buildings, parks, shops, houses, and obstacles
- Timed pickup and drop-off orders, scoring, and a saved high-score leaderboard
- Main menu, rider selection, instructions, pause, victory, and game-over screens
- Original looping background music, an in-game track selector, and collision, pickup, delivery, and order-failure sound effects
- Fixed-size arrays and functions for the game’s own data and systems

## Riders

| Rider | Vehicle | Speed | Hitbox | Score multiplier |
|---|---|---:|---:|---:|
| Noor | City bicycle | 170 px/s | 22 px | 1.0× |
| Khan | Motorcycle | 230 px/s | 28 px | 1.25× |
| Jimmy | Beamer (BMW) | 285 px/s | 34 px | 1.5× |

## Getting started on Windows

### Requirements

- Windows 10 or later
- CMake 3.20 or later
- A C++11-compatible MinGW-w64 compiler and `mingw32-make`
- Internet access for the first configure, because CMake fetches SFML 2.6.2 from its official Git repository

Make sure `cmake`, `g++`, and `mingw32-make` are available in PowerShell. From the project folder, configure, build, and launch the game:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
.\build\LastDelivery.exe
```

The build copies the game images and other assets next to the executable. The first configure downloads and builds SFML, so it can take longer than later builds. To rebuild after changing the source, run `cmake --build build --parallel` again.

> The commands above use MinGW. If you use Visual Studio, configure with a Visual Studio CMake generator instead of `MinGW Makefiles`.

## How to play

1. Choose **Start Game** from the main menu.
2. Select a rider by clicking a rider card or pressing `1`, `2`, or `3`, then press `Enter`.
3. Drive to the pickup shown for the active order, collect the package, and deliver it to the customer before time runs out.
4. Complete as many orders as you can. The game ends in victory when the orders are complete or in a game-over state when the shift fails.

### Controls

| Key | Action |
|---|---|
| `W`, `A`, `S`, `D` or arrow keys | Drive |
| `P` | Pause or resume |
| `R` | Restart the shift |
| `H` on the main menu | Open high scores |
| `I` on the main menu | Open instructions |
| `M` | Open the music player from the menu, game, or pause screen |
| `Enter` | Confirm a selection or continue from information screens |
| `Esc` | Pause while playing; return from most screens; close from the main menu |
| Music screen: arrows | Select a track; left/right also start the selected track |
| Music screen: `Enter` / `Space` | Play selected track / pause or resume playback |

## Music files

The game includes an original looping track at `assets/music/city_ride_original.wav`. It starts automatically when the game opens and can be switched in the music player. The player also accepts these optional personal music files in `assets/music/`:

- `jeena_jeena.mp3`
- `oh_oh_jane_jaana.mp3`
- `o_re_piya.mp3`

MP3 files are excluded from Git by `.gitignore`; they are not included in this repository. Add your own copies with the filenames above if you want music playback. The game itself can be built and played without them. Track paths and displayed titles are defined near the top of `main.cpp`.

## Project layout

| Path | Purpose |
|---|---|
| `main.cpp` | Game setup, menus, event handling, and main gameplay loop |
| `constant.h` | Shared dimensions, tile codes, game states, and rider data |
| `Map.h` | City grid, tile collision checks, and map drawing |
| `player.h` | Rider movement, collision, and vehicle drawing |
| `Order.h` | Delivery orders, timers, and scoring |
| `UIAndFile.h` | Interface drawing and high-score file handling |
| `assets/` | Menu, character, and vehicle images; music folder placeholder |
| `CMakeLists.txt` | SFML dependency setup and Windows build configuration |

High scores are written to `highscores.txt` in the working directory when a game ends. That generated file and the build folder are ignored by Git.

## Implementation note

The game’s own data and logic use fixed-size arrays and functions rather than custom `class` or `struct` definitions, `std::vector`, or raw pointer variables. SFML classes and types remain in use for graphics, input, audio, and the game window.

## Team

| Name | Roll number | Main responsibility |
|---|---|---|
| Faiq Aziz | 26L-0786 | City map, core engine, file handling, and leaderboard |
| Noor Fatima | 26L-0780 | Characters, vehicles, movement, and collision |
| Abdul Baseer Khan | 26L-0753 | Orders, timers, scoring, and user interface |

## Repository

Repository: [github.com/faiqrehmannn/LAST-DELIVERY](https://github.com/faiqrehmannn/LAST-DELIVERY).


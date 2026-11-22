# Last Delivery - PF Semester Project

**Course:** CS1002 - Programming Fundamentals  
**Institution:** FAST NUCES, Lahore  

### Group Members
- **Faiq Aziz** (26L-0786)
- **Noor Fatima** (26L-0780)
- **Abdul Baseer Khan** (26L-0753)

---

## Project Overview
Last Delivery is a 2D courier delivery game developed in C++ using SFML 2.6.2. 
The player selects a courier rider and navigates through a grid-based city to pick up food packages from shops and deliver them to customer houses before the order timer expires. 

The project demonstrates core Programming Fundamentals concepts including 2D map grids, parallel 1D arrays for game entities, state-driven control flow, file handling for persistent high scores, and sorting algorithms.

---

## Game Features
- **3 Playable Couriers:** Each has different speed, hitbox size, and score multiplier.
  - Noor: Bicycle (Speed: 170 px/s, Score Multiplier: 1.0x)
  - Khan: Motorcycle (Speed: 230 px/s, Score Multiplier: 1.25x)
  - Jimmy: Car (Speed: 285 px/s, Score Multiplier: 1.5x)
- **Tile-based City Map:** 15x20 grid with roads, buildings, parks, shops, customer houses, and pothole obstacles.
- **Timed Delivery Missions:** Pick up from designated restaurants and deliver to customer houses.
- **Persistent Leaderboard:** Saves top scores to `highscores.txt`.
- **Audio & Music:** In-game music player and sound effects for collisions, pickups, and deliveries.

---

## Controls
- **W / A / S / D** or **Arrow Keys**: Move vehicle
- **P**: Pause / Resume
- **R**: Restart game
- **M**: Open music player
- **H**: View high scores (from main menu)
- **I**: View instructions (from main menu)
- **Enter**: Confirm / Select
- **Esc**: Pause / Return to menu / Exit

---

## Concepts Applied
1. **2D Arrays:** Storing and rendering the city map grid (`cityGrid[15][20]`).
2. **1D Parallel Arrays:** Managing orders, rider attributes, and high score entries.
3. **Control Structures:** Conditional statements and `switch-case` state machine for screen management and input handling.
4. **Loops:** `for` loops for grid rendering, `while` for main event processing, and `do-while` for the bubble sort pass.
5. **Functions & Parameter Passing:** Modular game functions using pass-by-value and pass-by-reference.
6. **Sorting Algorithm:** Bubble Sort used to rank high scores in descending order.
7. **File Handling:** `ifstream` and `ofstream` to load and save player records in `highscores.txt`.
8. **Graphics & Audio:** SFML graphics window, texture rendering, and audio soundbuffers.

---

## How to Build and Run (Windows)

### Requirements
- MinGW-w64 (GCC with C++11 support)
- CMake 3.20 or newer

### Build Instructions
Run the following commands in PowerShell from the project root:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
.\build\LastDelivery.exe
```

*Note: CMake will automatically download and link SFML 2.6.2 on the first build.*

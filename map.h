#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include "constant.h"

// pooray map ka grid data yahan store hota hai
static int cityGrid[MAP_ROWS][MAP_COLS];

static void loadDefaultCity()
{
    // 0 = road, 1 = building, 2 = park, 3 = shop, 4 = house, 5 = obstacle
    int city[MAP_ROWS][MAP_COLS] = {
        {1,1,1,2,2,0,0,2,1,1,1,2,0,0,2,1,1,1,1,2},
        {1,1,1,2,1,0,0,2,1,1,1,2,0,0,2,1,3,1,1,2},
        {1,1,1,2,3,0,0,2,1,1,1,2,0,0,2,2,0,2,2,2},
        {2,2,2,2,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,5,0,0},
        {0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {2,2,0,2,2,0,0,2,2,1,1,2,0,0,2,4,1,1,2,2},
        {1,1,3,1,2,0,0,2,1,1,1,2,0,0,2,1,1,1,1,2},
        {1,1,1,1,2,0,0,2,1,1,4,2,0,0,2,1,1,1,4,2},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,5,0,0,0,0,0,5,0,0,0,0,0,0,0},
        {2,2,4,2,2,0,0,2,2,2,0,2,0,0,2,2,3,2,2,2},
        {1,1,1,1,2,0,0,2,1,1,3,2,0,0,2,1,1,1,1,2},
        {1,1,1,1,2,0,0,2,1,1,1,2,0,0,2,1,1,1,4,2},
        {2,2,2,2,2,0,0,2,2,2,2,2,0,0,2,2,2,2,2,2}
    };

    // copy testing array into the static grid
    for (int row = 0; row < MAP_ROWS; row++)
    {
        for (int col = 0; col < MAP_COLS; col++) {
            cityGrid[row][col] = city[row][col];
        }
    }
}

// collision checking helper function - solid objects return true
static bool isSolidTile(int row, int col)
{
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS) return true; // bound check laazmi hai

    return cityGrid[row][col] == TILE_BUILDING ||
           cityGrid[row][col] == TILE_PARK;
}

// map index to direct pixel math formulas 
static sf::Vector2f tileToPixelCenter(int row, int col)
{
    return sf::Vector2f(col * TILE_SIZE + TILE_SIZE / 2,
                        row * TILE_SIZE + TILE_SIZE / 2);
}

// tree generator function - variant breaks up the patterns visually
static void drawTree(sf::RenderWindow& window, float x, float y, int variant)
{
    // nicha ka kala shadow rectangle wrapper area
    sf::CircleShape shadow(12.0f);
    shadow.setOrigin(12.0f, 12.0f);
    shadow.setScale(1.25f, 0.55f);
    shadow.setPosition(x + 3.0f, y + 5.0f);
    shadow.setFillColor(sf::Color(25, 54, 39, 110));
    window.draw(shadow);

    // wood stem rectangle drawing block
    sf::RectangleShape trunk(sf::Vector2f(5.0f, 9.0f));
    trunk.setOrigin(2.5f, 1.0f);
    trunk.setPosition(x, y + 4.0f);
    trunk.setFillColor(sf::Color(105, 69, 43));
    window.draw(trunk);

    // multi variants selection greens palette
    const sf::Color greens[] = {
        sf::Color(34, 112, 70),
        sf::Color(42, 128, 75),
        sf::Color(31, 101, 68)
    };
    
    sf::CircleShape crown(12.0f);
    crown.setOrigin(12.0f, 12.0f);
    crown.setPosition(x, y - 2.0f);
    crown.setFillColor(greens[variant % 3]);
    crown.setOutlineThickness(1.0f);
    crown.setOutlineColor(sf::Color(24, 82, 55));
    window.draw(crown);

    // light shine drop overlay node
    sf::CircleShape highlight(4.0f);
    highlight.setOrigin(4.0f, 4.0f);
    highlight.setPosition(x - 4.0f, y - 6.0f);
    highlight.setFillColor(sf::Color(91, 158, 92));
    window.draw(highlight);
}

// road layers drawing logic node
static void drawRoad(sf::RenderWindow& window, int row, int col,
                     float x, float y)
{
    sf::RectangleShape asphalt(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    asphalt.setPosition(x, y);
    asphalt.setFillColor(sf::Color(48, 57, 67));
    window.draw(asphalt);

    // road curbs coloring lines
    sf::RectangleShape curb(sf::Vector2f(TILE_SIZE, 2.0f));
    curb.setPosition(x, y);
    curb.setFillColor(sf::Color(112, 119, 121));
    window.draw(curb);

    // check details around neighbors nodes logic to build lanes correctly
    const bool vertical =
        (row > 0 && cityGrid[row - 1][col] == TILE_ROAD) ||
        (row + 1 < MAP_ROWS && cityGrid[row + 1][col] == TILE_ROAD);
    const bool horizontal =
        (col > 0 && cityGrid[row][col - 1] == TILE_ROAD) ||
        (col + 1 < MAP_COLS && cityGrid[row][col + 1] == TILE_ROAD);

    sf::RectangleShape dash(sf::Vector2f(2.0f, 7.0f));
    dash.setFillColor(sf::Color(210, 190, 120, 165)); // divider strips color yellow type transparency
    
    if (vertical)
    {
        dash.setPosition(x + TILE_SIZE / 2.0f - 1.0f, y + 7.0f);
        window.draw(dash);
        dash.setPosition(x + TILE_SIZE / 2.0f - 1.0f, y + 34.0f);
        window.draw(dash);
    }
    if (horizontal)
    {
        dash.setSize(sf::Vector2f(7.0f, 2.0f));
        dash.setPosition(x + 7.0f, y + TILE_SIZE / 2.0f - 1.0f);
        window.draw(dash);
        dash.setPosition(x + 34.0f, y + TILE_SIZE / 2.0f - 1.0f);
        window.draw(dash);
    }
}

// plaza and big tiles handling logic blocks
static void drawBuilding(sf::RenderWindow& window, int row, int col,
                         float x, float y)
{
    sf::RectangleShape lot(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    lot.setPosition(x, y);
    lot.setFillColor(sf::Color(177, 169, 150));
    window.draw(lot);

    // dropdown background black layer alpha
    sf::RectangleShape shadow(sf::Vector2f(38.0f, 39.0f));
    shadow.setPosition(x + 7.0f, y + 8.0f);
    shadow.setFillColor(sf::Color(52, 56, 62, 100));
    window.draw(shadow);

    const sf::Color roofs[] = {
        sf::Color(153, 91, 70), sf::Color(129, 86, 76),
        sf::Color(170, 104, 75), sf::Color(118, 79, 72)
    };
    
    sf::RectangleShape roof(sf::Vector2f(38.0f, 36.0f));
    roof.setPosition(x + 5.0f, y + 4.0f);
    roof.setFillColor(roofs[(row * 7 + col * 3) % 4]); // randomly color matching using calculations
    roof.setOutlineThickness(1.0f);
    roof.setOutlineColor(sf::Color(93, 65, 57));
    window.draw(roof);

    sf::RectangleShape roofLight(sf::Vector2f(30.0f, 3.0f));
    roofLight.setPosition(x + 9.0f, y + 7.0f);
    roofLight.setFillColor(sf::Color(208, 147, 106, 150));
    window.draw(roofLight);

    // tiny ac exhaust on roof details block top down visual wrapper
    sf::RectangleShape vent(sf::Vector2f(7.0f, 5.0f));
    vent.setPosition(x + 21.0f, y + 21.0f);
    vent.setFillColor(sf::Color(96, 77, 69));
    window.draw(vent);
}

// house geometry setup drawing system nodes 
static void drawHouse(sf::RenderWindow& window, float x, float y)
{
    sf::RectangleShape yard(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    yard.setPosition(x, y);
    yard.setFillColor(sf::Color(157, 176, 130));
    window.draw(yard);

    sf::RectangleShape shadow(sf::Vector2f(35.0f, 31.0f));
    shadow.setPosition(x + 8.0f, y + 12.0f);
    shadow.setFillColor(sf::Color(40, 48, 44, 90));
    window.draw(shadow);

    sf::ConvexShape roof;
    roof.setPointCount(5);
    roof.setPoint(0, sf::Vector2f(6.0f, 22.0f));
    roof.setPoint(1, sf::Vector2f(24.0f, 5.0f));
    roof.setPoint(2, sf::Vector2f(42.0f, 22.0f));
    roof.setPoint(3, sf::Vector2f(39.0f, 41.0f));
    roof.setPoint(4, sf::Vector2f(9.0f, 41.0f));
    roof.setPosition(x, y);
    roof.setFillColor(sf::Color(142, 71, 58));
    roof.setOutlineThickness(1.0f);
    roof.setOutlineColor(sf::Color(91, 53, 48));
    window.draw(roof);

    sf::RectangleShape face(sf::Vector2f(22.0f, 17.0f));
    face.setPosition(x + 13.0f, y + 22.0f);
    face.setFillColor(sf::Color(220, 190, 146));
    window.draw(face);

    sf::RectangleShape door(sf::Vector2f(6.0f, 12.0f));
    door.setPosition(x + 21.0f, y + 27.0f);
    door.setFillColor(sf::Color(92, 58, 45));
    window.draw(door);
}

// shops display processing functions elements
static void drawShop(sf::RenderWindow& window, float x, float y)
{
    sf::RectangleShape sidewalk(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    sidewalk.setPosition(x, y);
    sidewalk.setFillColor(sf::Color(177, 169, 150));
    window.draw(sidewalk);

    sf::RectangleShape storefront(sf::Vector2f(38.0f, 32.0f));
    storefront.setPosition(x + 5.0f, y + 10.0f);
    storefront.setFillColor(sf::Color(228, 215, 188));
    storefront.setOutlineThickness(1.0f);
    storefront.setOutlineColor(sf::Color(86, 75, 67));
    window.draw(storefront);

    sf::RectangleShape awning(sf::Vector2f(40.0f, 9.0f));
    awning.setPosition(x + 4.0f, y + 8.0f);
    awning.setFillColor(sf::Color(39, 139, 111));
    window.draw(awning);

    // twin showcases window panes rendering layers code node
    sf::RectangleShape windowGlass(sf::Vector2f(13.0f, 13.0f));
    windowGlass.setPosition(x + 8.0f, y + 20.0f);
    windowGlass.setFillColor(sf::Color(92, 164, 174));
    window.draw(windowGlass);
    
    windowGlass.setPosition(x + 27.0f, y + 20.0f);
    window.draw(windowGlass);
}

// main loop layer running over entire board columns
static void drawCityMap(sf::RenderWindow& window)
{
    for (int row = 0; row < MAP_ROWS; row++)
    {
        for (int col = 0; col < MAP_COLS; col++)
        {
            const int tile = cityGrid[row][col];
            const float x = col * TILE_SIZE;
            const float y = row * TILE_SIZE;

            if (tile == TILE_ROAD) {
                drawRoad(window, row, col, x, y);
            }
            else if (tile == TILE_BUILDING) {
                drawBuilding(window, row, col, x, y);
            }
            else if (tile == TILE_PARK)
            {
                // park ki grass base layer draw ho rhi hai idhar
                sf::RectangleShape grass(sf::Vector2f(TILE_SIZE, TILE_SIZE));
                grass.setPosition(x, y);
                grass.setFillColor(sf::Color(62, 119, 80));
                window.draw(grass);

                // beech mein chalnay ka path rectangle pattern
                sf::RectangleShape path(sf::Vector2f(7.0f, TILE_SIZE));
                path.setPosition(x + 21.0f, y);
                path.setFillColor(sf::Color(158, 148, 121, 180));
                window.draw(path);

                // tree drawing variant call - thora calculation se offset change kia taake symmetric na lage
                drawTree(window, x + 12.0f + (col % 3) * 5.0f,
y + 19.0f + (row % 2) * 7.0f, row + col);
}
else if (tile == TILE_SHOP)
{
drawShop(window, x, y);
}
else if (tile == TILE_HOUSE) {
drawHouse(window, x, y);
}
else if (tile == TILE_OBSTACLE)
{
// obstacle k neechay pehle road layer ani chahiye normal
drawRoad(window, row, col, x, y);
// pothole shape handling - sarak k beech mein gaddha draw kia hai
sf::CircleShape pothole(8.0f);
pothole.setOrigin(8.0f, 8.0f);
pothole.setPosition(x + 24.0f, y + 24.0f);
pothole.setFillColor(sf::Color(28, 33, 39));
pothole.setOutlineThickness(2.0f);
pothole.setOutlineColor(sf::Color(134, 91, 59)); // mitti/brown color for borders
window.draw(pothole);
}
}
}
}
#endif // MAP_H

#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include <string>
#include "constant.h"

// Poore shehar ka grid layout save karne ke liye 2D array
int cityGrid[MAP_ROWS][MAP_COLS];

// Dukano ki tasveerein (textures) aur unke load hone ka status
sf::Texture shopArtTextures[4];
bool shopArtLoaded[4] = {false, false, false, false};

// Tamam shops aur post office ki images ko memory mein load karne ka function
void loadShopArt()
{
    const std::string shopArtPaths[4] = {
        "assets/post-office.png",
        "assets/burger-shop.jpg",
        "assets/pizza-shop.png",
        "assets/orders/taco.jpg"
    };

    for (int i = 0; i < 4; i++)
    {
        shopArtLoaded[i] = shopArtTextures[i].loadFromFile(shopArtPaths[i]);
        shopArtTextures[i].setSmooth(false);
    }
}

// Default city map ko grid mein set karne ka function (0=Road, 1=Building, 2=Park, 3=Shop, 4=House, 5=Obstacle)
void loadDefaultCity()
{
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
        {1,1,1,1,2,0,0,2,1,1,1,2,0,0,2,1,1,4,1,2},
        {2,2,2,2,2,0,0,2,2,2,2,2,0,0,2,2,2,2,2,2}
    };

    for (int row = 0; row < MAP_ROWS; row++)
    {
        for (int col = 0; col < MAP_COLS; col++)
        {
            cityGrid[row][col] = city[row][col];
        }
    }
}

// Check karta hai ke koi tile thos (solid) hai ya nahi taake rider us se guzar na sake
bool isSolidTile(int row, int col)
{
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS)
    {
        return true;
    }

    return cityGrid[row][col] == TILE_BUILDING || cityGrid[row][col] == TILE_PARK;
}

// Grid ke row aur column ko screen ke center pixel coordinates mein badalta hai
sf::Vector2f tileToPixelCenter(int row, int col)
{
    return sf::Vector2f(
        col * TILE_SIZE + TILE_SIZE / 2,
        row * TILE_SIZE + TILE_SIZE / 2
    );
}

// Park mein darakht (tree) aur uska saya draw karne ka function
void drawTree(sf::RenderWindow& window, float x, float y, int variant)
{
    // Darakht ke neeche halka saya
    sf::CircleShape shadow(12.0f);
    shadow.setOrigin(12.0f, 12.0f);
    shadow.setScale(1.25f, 0.55f);
    shadow.setPosition(x + 3.0f, y + 5.0f);
    shadow.setFillColor(sf::Color(25, 54, 39, 110));
    window.draw(shadow);

    // Darakht ka tana (trunk)
    sf::RectangleShape trunk(sf::Vector2f(5.0f, 9.0f));
    trunk.setOrigin(2.5f, 1.0f);
    trunk.setPosition(x, y + 4.0f);
    trunk.setFillColor(sf::Color(105, 69, 43));
    window.draw(trunk);

    const sf::Color greens[3] = {
        sf::Color(34, 112, 70),
        sf::Color(42, 128, 75),
        sf::Color(31, 101, 68)
    };

    // Darakht ke patton ka ghera (crown)
    sf::CircleShape crown(12.0f);
    crown.setOrigin(12.0f, 12.0f);
    crown.setPosition(x, y - 2.0f);
    crown.setFillColor(greens[variant % 3]);
    crown.setOutlineThickness(1.0f);
    crown.setOutlineColor(sf::Color(24, 82, 55));
    window.draw(crown);

    // Dhoop ki chamak dikhane ke liye halka sabz highlight
    sf::CircleShape highlight(4.0f);
    highlight.setOrigin(4.0f, 4.0f);
    highlight.setPosition(x - 4.0f, y - 6.0f);
    highlight.setFillColor(sf::Color(91, 158, 92));
    window.draw(highlight);
}

// Sadak aur uski lines aur zebra crossing draw karne ka function
void drawRoad(sf::RenderWindow& window, int row, int col, float x, float y)
{
    sf::RectangleShape asphalt(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    asphalt.setPosition(x, y);
    asphalt.setFillColor(sf::Color(48, 57, 67));
    window.draw(asphalt);

    // Check karein ke aas paas upar/neeche ya dayein/bayein sadak mojood hai ya nahi
    const bool vertical =
        (row > 0 && cityGrid[row - 1][col] == TILE_ROAD) ||
        (row + 1 < MAP_ROWS && cityGrid[row + 1][col] == TILE_ROAD);

    const bool horizontal =
        (col > 0 && cityGrid[row][col - 1] == TILE_ROAD) ||
        (col + 1 < MAP_COLS && cityGrid[row][col + 1] == TILE_ROAD);

    sf::RectangleShape dash(sf::Vector2f(2.0f, 7.0f));
    dash.setFillColor(sf::Color(210, 190, 120, 165));

    // Seedhi (vertical) sadak ki beech wali peeli lines
    if (vertical)
    {
        dash.setPosition(x + TILE_SIZE / 2.0f - 1.0f, y + 7.0f);
        window.draw(dash);
        dash.setPosition(x + TILE_SIZE / 2.0f - 1.0f, y + 34.0f);
        window.draw(dash);
    }

    // Aari (horizontal) sadak ki beech wali peeli lines
    if (horizontal)
    {
        dash.setSize(sf::Vector2f(7.0f, 2.0f));
        dash.setPosition(x + 7.0f, y + TILE_SIZE / 2.0f - 1.0f);
        window.draw(dash);
        dash.setPosition(x + 34.0f, y + TILE_SIZE / 2.0f - 1.0f);
        window.draw(dash);
    }

    // Jahan vertical aur horizontal sadkein milti hain wahan zebra crossing banayein
    if (vertical && horizontal)
    {
        sf::RectangleShape stripe(sf::Vector2f(4.0f, 7.0f));
        stripe.setFillColor(sf::Color(222, 226, 216, 185));

        for (int i = 0; i < 4; i++)
        {
            stripe.setPosition(x + 5.0f + i * 10.0f, y + 4.0f);
            window.draw(stripe);

            stripe.setPosition(x + 5.0f + i * 10.0f, y + 37.0f);
            window.draw(stripe);
        }
    }
}

// Shehar ki imarat (building) ko chhat aur khidkiyon ke sath draw karne ka function
void drawBuilding(sf::RenderWindow& window, int row, int col, float x, float y)
{
    sf::RectangleShape lot(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    lot.setPosition(x, y);
    lot.setFillColor(sf::Color(177, 169, 150));
    window.draw(lot);

    // Building ka saya
    sf::RectangleShape shadow(sf::Vector2f(38.0f, 39.0f));
    shadow.setPosition(x + 7.0f, y + 8.0f);
    shadow.setFillColor(sf::Color(52, 56, 62, 100));
    window.draw(shadow);

    const sf::Color roofs[4] = {
        sf::Color(153, 91, 70),
        sf::Color(129, 86, 76),
        sf::Color(170, 104, 75),
        sf::Color(118, 79, 72)
    };

    // Building ki chhat (roof)
    sf::RectangleShape roof(sf::Vector2f(38.0f, 36.0f));
    roof.setPosition(x + 5.0f, y + 4.0f);
    roof.setFillColor(roofs[(row * 7 + col * 3) % 4]);
    roof.setOutlineThickness(1.0f);
    roof.setOutlineColor(sf::Color(93, 65, 57));
    window.draw(roof);

    // Chhat ke kinare par roshni ka hissa
    sf::RectangleShape roofLight(sf::Vector2f(30.0f, 3.0f));
    roofLight.setPosition(x + 9.0f, y + 7.0f);
    roofLight.setFillColor(sf::Color(208, 147, 106, 150));
    window.draw(roofLight);

    // Building ki do khidkiyan (windows)
    sf::RectangleShape window(sf::Vector2f(8.0f, 9.0f));
    window.setPosition(x + 10.0f, y + 18.0f);
    window.setFillColor(sf::Color(105, 157, 173));
    window.draw(window);

    window.setPosition(x + 29.0f, y + 18.0f);
    window.draw(window);
}

// Ghar (house) ko lawn, tikoni chhat aur darwaze ke sath draw karne ka function
void drawHouse(sf::RenderWindow& window, float x, float y)
{
    sf::RectangleShape yard(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    yard.setPosition(x, y);
    yard.setFillColor(sf::Color(157, 176, 130));
    window.draw(yard);

    // Ghar ki tikoni chhat (pentagon shape)
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

    // Ghar ki samne wali deewar
    sf::RectangleShape face(sf::Vector2f(22.0f, 17.0f));
    face.setPosition(x + 13.0f, y + 22.0f);
    face.setFillColor(sf::Color(220, 190, 146));
    window.draw(face);

    // Ghar ka darwaza
    sf::RectangleShape door(sf::Vector2f(6.0f, 12.0f));
    door.setPosition(x + 21.0f, y + 27.0f);
    door.setFillColor(sf::Color(92, 58, 45));
    window.draw(door);
}

// Makhsoos coordinates ke mutabiq dukan (shop) ki tasveer draw karne ka function
void drawShop(sf::RenderWindow& window, int row, int col, float x, float y)
{
    sf::RectangleShape sidewalk(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    sidewalk.setPosition(x, y);
    sidewalk.setFillColor(sf::Color(177, 169, 150));
    window.draw(sidewalk);

    int shopIndex = 1;

    // Map par dukan ki jagah ke hisab se sahi image chunein
    if (row == 2 && col == 4)
        shopIndex = 2;
    else if (row == 7 && col == 2)
        shopIndex = 1;
    else if (row == 11 && col == 16)
        shopIndex = 0;
    else if (row == 12 && col == 10)
        shopIndex = 3;

    // Agar tasveer load ho chuki hai to usay tile ke size ke mutabiq scale karein
    if (shopArtLoaded[shopIndex])
    {
        const sf::Vector2u imageSize = shopArtTextures[shopIndex].getSize();

        if (imageSize.x > 0 && imageSize.y > 0)
        {
            const float scaleX = (TILE_SIZE - 4.0f) / imageSize.x;
            const float scaleY = (TILE_SIZE - 4.0f) / imageSize.y;
            const float scale = scaleX < scaleY ? scaleX : scaleY;

            sf::Sprite shopSprite(shopArtTextures[shopIndex]);
            shopSprite.setScale(scale, scale);
            
            // Tasveer ko tile ke bilkul darmiyan (center) mein set karein
            shopSprite.setPosition(
                x + (TILE_SIZE - imageSize.x * scale) / 2.0f,
                y + (TILE_SIZE - imageSize.y * scale) / 2.0f
            );

            window.draw(shopSprite);
            return;
        }
    }

    // Agar dukan ki tasveer load na ho sake to fallback ke tor par sada dukan draw karein
    sf::RectangleShape shopFront(sf::Vector2f(38.0f, 32.0f));
    shopFront.setPosition(x + 5.0f, y + 10.0f);
    shopFront.setFillColor(sf::Color(228, 215, 188));
    shopFront.setOutlineThickness(1.0f);
    shopFront.setOutlineColor(sf::Color(86, 75, 67));
    window.draw(shopFront);

    // Dukan ke upar wala shed (awning)
    sf::RectangleShape awning(sf::Vector2f(40.0f, 9.0f));
    awning.setPosition(x + 4.0f, y + 8.0f);
    awning.setFillColor(sf::Color(39, 139, 111));
    window.draw(awning);
}

// Poore city map ke har row aur column ko check kar ke screen par draw karne ka main function
void drawCityMap(sf::RenderWindow& window)
{
    for (int row = 0; row < MAP_ROWS; row++)
    {
        for (int col = 0; col < MAP_COLS; col++)
        {
            const int tile = cityGrid[row][col];
            const float x = col * TILE_SIZE;
            const float y = row * TILE_SIZE;

            if (tile == TILE_ROAD)
            {
                drawRoad(window, row, col, x, y);
            }
            else if (tile == TILE_BUILDING)
            {
                drawBuilding(window, row, col, x, y);
            }
            else if (tile == TILE_PARK)
            {
                // Park ke liye pehle sabz ghaas (grass) bichayein
                sf::RectangleShape grass(sf::Vector2f(TILE_SIZE, TILE_SIZE));
                grass.setPosition(x, y);
                grass.setFillColor(sf::Color(62, 119, 80));
                window.draw(grass);

                // Phir ghaas ke upar thodi mukhtalif jagah par darakht lagayein
                drawTree(
                    window,
                    x + 12.0f + (col % 3) * 5.0f,
                    y + 19.0f + (row % 2) * 7.0f,
                    row + col
                );
            }
            else if (tile == TILE_SHOP)
            {
                drawShop(window, row, col, x, y);
            }
            else if (tile == TILE_HOUSE)
            {
                drawHouse(window, x, y);
            }
            else if (tile == TILE_OBSTACLE)
            {
                // Rukawat (pothole) dikhane ke liye pehle neeche sadak draw karein
                drawRoad(window, row, col, x, y);

                // Sadak ke beech mein khadda (pothole) banayein
                sf::CircleShape pothole(8.0f);
                pothole.setOrigin(8.0f, 8.0f);
                pothole.setPosition(x + 24.0f, y + 24.0f);
                pothole.setFillColor(sf::Color(28, 33, 39));
                pothole.setOutlineThickness(2.0f);
                pothole.setOutlineColor(sf::Color(134, 91, 59));
                window.draw(pothole);
            }
        }
    }
}

#endif

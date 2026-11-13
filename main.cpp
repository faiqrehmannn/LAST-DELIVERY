#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

#include "constant.h"
#include "Map.h"
#include "player.h"

static void drawText(sf::RenderWindow& window, sf::Font& font, bool fontOk,
                     const std::string& message, unsigned int size,
                     float x, float y, sf::Color color)
{
    if (!fontOk) return;
    sf::Text text;
    text.setFont(font);
    text.setString(message);
    text.setCharacterSize(size);
    text.setPosition(x, y);
    text.setFillColor(color);
    window.draw(text);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_W, WINDOW_H),
                            "Last Delivery | Group 11");
    window.setFramerateLimit(60);

    sf::Font font;
    const bool fontOk = font.loadFromFile("assets/arial.ttf") ||
                        font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    if (!fontOk)
        std::cout << "Font not found; the side panel will have no text.\n";

    loadDefaultCity();
    setupPlayer(0, tileToPixelCenter(4, 5));

    sf::Clock frameClock;

    while (window.isOpen())
    {
        float deltaTime = frameClock.restart().asSeconds();
        if (deltaTime > 0.1f) deltaTime = 0.1f;

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
                if (event.key.code == sf::Keyboard::Num1)
                    setupPlayer(0, tileToPixelCenter(4, 5));
                if (event.key.code == sf::Keyboard::Num2)
                    setupPlayer(1, tileToPixelCenter(4, 5));
                if (event.key.code == sf::Keyboard::Num3)
                    setupPlayer(2, tileToPixelCenter(4, 5));
                if (event.key.code == sf::Keyboard::R)
                    setupPlayer(currentCharacter, tileToPixelCenter(4, 5));
            }
        }

        updatePlayerMovement(deltaTime);

        window.clear(sf::Color(13, 20, 29));
        drawCityMap(window);
        drawPlayer(window);

        sf::RectangleShape sidePanel(sf::Vector2f(WINDOW_W - MAP_PIXEL_W,
                                                  WINDOW_H));
        sidePanel.setPosition(MAP_PIXEL_W, 0.0f);
        sidePanel.setFillColor(sf::Color(12, 27, 43));
        sidePanel.setOutlineThickness(-2.0f);
        sidePanel.setOutlineColor(sf::Color(68, 111, 137));
        window.draw(sidePanel);

        const float panelX = MAP_PIXEL_W + 28;
        drawText(window, font, fontOk, "DELIVERY SHIFT", 26, panelX, 30.0f,
                 sf::Color(255, 220, 133));
        drawText(window, font, fontOk, "RIDER", 15, panelX, 92.0f,
                 sf::Color(117, 177, 201));
        drawText(window, font, fontOk,
                 RIDER_NAMES[currentCharacter] + " / " +
                 VEHICLE_NAMES[currentCharacter],
                 18, panelX, 116.0f, sf::Color::White);
        drawText(window, font, fontOk,
                 "Speed: " + std::to_string((int)RIDER_MAX_SPEED[currentCharacter]),
                 18, panelX, 170.0f, sf::Color::White);
        if (hittingObstacle)
            drawText(window, font, fontOk, "OBSTACLE: SPEED REDUCED", 16,
                     panelX, 580.0f, sf::Color(255, 121, 93));
        drawText(window, font, fontOk,
                 "WASD / Arrows: move   1-3: rider   R: reset   ESC: quit",
                 14, panelX, 694.0f, sf::Color(117, 177, 201));

        window.display();
    }

    return 0;
}

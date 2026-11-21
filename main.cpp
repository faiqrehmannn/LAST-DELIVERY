#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include "Order.h"
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
// Check karta hai ke rider current order ke target ke qareeb hai.
bool isNearCurrentOrderTarget()
{
    if (!hasCurrentOrder())
    {
        return false;
    }

    const int orderIndex = currentOrderIndex;

    const int targetRow = orderPackagePickedUp[orderIndex]
        ? orderDropRows[orderIndex]
        : orderPickupRows[orderIndex];

    const int targetCol = orderPackagePickedUp[orderIndex]
        ? orderDropCols[orderIndex]
        : orderPickupCols[orderIndex];

    const sf::Vector2f target =
        tileToPixelCenter(targetRow, targetCol);

    const float differenceX = playerPosition.x - target.x;
    const float differenceY = playerPosition.y - target.y;

    return differenceX * differenceX + differenceY * differenceY
        <= 28.0f * 28.0f;
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
              if (event.key.code == sf::Keyboard::E &&
    failedCount < 3 &&
    hasCurrentOrder() &&
    isNearCurrentOrderTarget())
{
    if (!orderPackagePickedUp[currentOrderIndex])
    {
        orderPackagePickedUp[currentOrderIndex] = true;
        orderStatusBanner =
            "Package picked up! Deliver it to " +
            orderCustomerNames[currentOrderIndex] + ".";
    }
    else
    {
        completeCurrentOrder(
            RIDER_SCORE_MULTIPLIER[currentCharacter]
        );
    }
}
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
                if (event.key.code == sf::Keyboard::Num1)
                    setupPlayer(0, tileToPixelCenter(4, 5));
              resetOrders();
                if (event.key.code == sf::Keyboard::Num2)
                    setupPlayer(1, tileToPixelCenter(4, 5));
                if (event.key.code == sf::Keyboard::Num3)
                    setupPlayer(2, tileToPixelCenter(4, 5));
               if (event.key.code == sf::Keyboard::R)
{
    setupPlayer(currentCharacter, tileToPixelCenter(4, 5));
    resetOrders();
}
            }
        }

        updatePlayerMovement(deltaTime);
        // Active order ka timer chalao aur time khatam ho to order fail karo.
if (failedCount < 3 && hasCurrentOrder())
{
    updateOrderTimer(deltaTime);

    if (isOrderExpired())
    {
        failCurrentOrder();
    }
}
        window.clear(sf::Color(13, 20, 29));
        drawCityMap(window);
      // Current pickup ya delivery target ko map par highlight karo.
if (hasCurrentOrder() && failedCount < 3)
{
    const int orderIndex = currentOrderIndex;

    const int targetRow = orderPackagePickedUp[orderIndex]
        ? orderDropRows[orderIndex]
        : orderPickupRows[orderIndex];

    const int targetCol = orderPackagePickedUp[orderIndex]
        ? orderDropCols[orderIndex]
        : orderPickupCols[orderIndex];

    const sf::Vector2f target =
        tileToPixelCenter(targetRow, targetCol);

    sf::CircleShape targetMarker(14.0f);
    targetMarker.setOrigin(14.0f, 14.0f);
    targetMarker.setPosition(target);
    targetMarker.setFillColor(
        orderPackagePickedUp[orderIndex]
            ? sf::Color(255, 211, 75, 150)
            : sf::Color(57, 220, 126, 150)
    );
    targetMarker.setOutlineThickness(3.0f);
    targetMarker.setOutlineColor(sf::Color::White);
    window.draw(targetMarker);
}
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
      drawText(window, font, fontOk,
         "Cash: $" + std::to_string(orderScore),
         20, panelX, 220.0f, sf::Color(255, 220, 133));

drawText(window, font, fontOk,
         "Delivered: " + std::to_string(completedCount),
         17, panelX, 260.0f, sf::Color::White);

drawText(window, font, fontOk,
         "Failed: " + std::to_string(failedCount) + " / 3",
         17, panelX, 292.0f, sf::Color(239, 151, 126));

if (hasCurrentOrder() && failedCount < 3)
{
    const int orderIndex = currentOrderIndex;
    const int secondsLeft = orderTimesRemaining[orderIndex];

    drawText(window, font, fontOk,
             "Order: " + orderShopNames[orderIndex],
             17, panelX, 350.0f, sf::Color::White);

    if (orderPackagePickedUp[orderIndex])
    {
        drawText(window, font, fontOk,
                 "Deliver to: " + orderCustomerNames[orderIndex],
                 16, panelX, 385.0f, sf::Color(255, 220, 133));
    }
    else
    {
        drawText(window, font, fontOk,
                 "Pick up: " + orderShopNames[orderIndex],
                 16, panelX, 385.0f, sf::Color(115, 234, 156));
    }

    drawText(window, font, fontOk,
             "Time left: " + std::to_string(secondsLeft) + " sec",
             18, panelX, 425.0f, sf::Color::White);
}

drawText(window, font, fontOk,
         orderStatusBanner,
         14, panelX, 500.0f, sf::Color(213, 230, 235));

drawText(window, font, fontOk,
         "E: pick up / deliver",
         15, panelX, 650.0f, sf::Color(117, 177, 201));
        if (hittingObstacle)
            drawText(window, font, fontOk, "OBSTACLE: SPEED REDUCED", 16,
                     panelX, 580.0f, sf::Color(255, 121, 93));
        drawText(window, font, fontOk,
                 "WASD / Arrows: move   1-3: rider   R: reset   ESC: quit",
                 14, panelX, 694.0f, sf::Color(117, 177, 201));
if (failedCount >= 3 || allOrdersCompleted())
{
    sf::RectangleShape resultPanel(sf::Vector2f(440.0f, 150.0f));
    resultPanel.setPosition(
        MAP_PIXEL_W / 2.0f - 220.0f,
        WINDOW_H / 2.0f - 75.0f
    );
    resultPanel.setFillColor(sf::Color(12, 27, 43, 240));
    resultPanel.setOutlineThickness(3.0f);
    resultPanel.setOutlineColor(
        allOrdersCompleted()
            ? sf::Color(115, 234, 156)
            : sf::Color(239, 151, 126)
    );
    window.draw(resultPanel);

    drawText(window, font, fontOk,
             allOrdersCompleted() ? "SHIFT COMPLETE" : "SHIFT OVER",
             30, MAP_PIXEL_W / 2.0f - 135.0f,
             WINDOW_H / 2.0f - 45.0f,
             sf::Color(255, 220, 133));

    drawText(window, font, fontOk,
             "Press R to start a new shift",
             18, MAP_PIXEL_W / 2.0f - 130.0f,
             WINDOW_H / 2.0f + 15.0f,
             sf::Color::White);
}
        window.display();
    }

    return 0;
}

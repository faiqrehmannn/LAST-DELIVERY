#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

#include "Order.h"
#include "constant.h"
#include "Map.h"
#include "player.h"

// Screen par text likhta hai.
// Agar font load na ho, game phir bhi chalega; sirf text nahi dikhega.
void drawText(sf::RenderWindow& window, sf::Font& font, bool fontOk,
              const std::string& message, unsigned int size,
              float x, float y, sf::Color color)
{
    if (!fontOk)
    {
        return;
    }

    sf::Text text;
    text.setFont(font);
    text.setString(message);
    text.setCharacterSize(size);
    text.setPosition(x, y);
    text.setFillColor(color);
    window.draw(text);
}

// Check karta hai rider current pickup ya delivery point ke qareeb hai.
bool isNearCurrentOrderTarget()
{
    if (!hasCurrentOrder())
    {
        return false;
    }

    const int orderIndex = currentOrderIndex;

    int targetRow;
    int targetCol;

    if (orderPackagePickedUp[orderIndex])
    {
        targetRow = orderDropRows[orderIndex];
        targetCol = orderDropCols[orderIndex];
    }
    else
    {
        targetRow = orderPickupRows[orderIndex];
        targetCol = orderPickupCols[orderIndex];
    }

    const sf::Vector2f target =
        tileToPixelCenter(targetRow, targetCol);

    const float differenceX = playerPosition.x - target.x;
    const float differenceY = playerPosition.y - target.y;

    // Distance ka square check hota hai; is se square root ki zaroorat nahi.
    return differenceX * differenceX + differenceY * differenceY
        <= 28.0f * 28.0f;
}

int main()
{
    sf::RenderWindow window(
        sf::VideoMode(WINDOW_W, WINDOW_H),
        "Last Delivery | Group 11"
    );

    window.setFramerateLimit(60);

    // Font load karne ki koshish.
    sf::Font font;
    const bool fontOk =
        font.loadFromFile("assets/arial.ttf") ||
        font.loadFromFile("C:/Windows/Fonts/arial.ttf");

    if (!fontOk)
    {
        std::cout << "Font not found. Game text will not appear.\n";
    }

    // Map ki shop images load karne ki koshish.
    // Agar images na milen, Map.h ka fallback shop drawing use hoga.
    loadShopArt();
    loadDefaultCity();

    // Temporary blue rider image.
    // Jab vehicle PNG repo mein ho, is image ko us PNG se replace karna.
    sf::Image riderImage;
    riderImage.create(300, 100, sf::Color(45, 160, 220));

    sf::Texture riderTexture;
    riderTexture.loadFromImage(riderImage);

    sf::Sprite riderSprite(riderTexture);

    // Naya game shuru hote hi orders ko ek martaba initialize karo.
    resetOrders();

    // Rider ko starting road tile par rakho.
    setupPlayer(0, tileToPixelCenter(4, 5));

    sf::Clock frameClock;

    while (window.isOpen())
    {
        float deltaTime = frameClock.restart().asSeconds();

        // Agar window kuch dair ruk jaye, to rider ek frame mein bohat door na jaye.
        if (deltaTime > 0.1f)
        {
            deltaTime = 0.1f;
        }

        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed)
            {
                // ESC game band karta hai.
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }

                // 1, 2, 3 se rider badalta hai.
                // Rider badalne se orders reset nahi honge.
                if (event.key.code == sf::Keyboard::Num1)
                {
                    setupPlayer(0, tileToPixelCenter(4, 5));
                }
                else if (event.key.code == sf::Keyboard::Num2)
                {
                    setupPlayer(1, tileToPixelCenter(4, 5));
                }
                else if (event.key.code == sf::Keyboard::Num3)
                {
                    setupPlayer(2, tileToPixelCenter(4, 5));
                }

                // R se rider aur poori shift dobara shuru hoti hai.
                else if (event.key.code == sf::Keyboard::R)
                {
                    setupPlayer(
                        currentCharacter,
                        tileToPixelCenter(4, 5)
                    );

                    resetOrders();
                }

                // E dabane par pickup ya delivery hoti hai,
                // magar sirf jab rider sahi target ke qareeb ho.
                else if (event.key.code == sf::Keyboard::E &&
                         failedCount < 3 &&
                         !allOrdersCompleted() &&
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
            }
        }

        // Shift khatam ho chuki ho to rider aur timer ruk jayenge.
        const bool shiftOver =
            failedCount >= 3 || allOrdersCompleted();

        if (!shiftOver)
        {
            updatePlayerMovement(deltaTime);

            if (hasCurrentOrder())
            {
                updateOrderTimer(deltaTime);

                if (isOrderExpired())
                {
                    failCurrentOrder();
                }
            }
        }

        window.clear(sf::Color(13, 20, 29));

        // Shehar ka map draw karo.
        drawCityMap(window);

        // Current pickup ya delivery target ko highlight karo.
        if (hasCurrentOrder() && failedCount < 3)
        {
            const int orderIndex = currentOrderIndex;

            int targetRow;
            int targetCol;

            if (orderPackagePickedUp[orderIndex])
            {
                targetRow = orderDropRows[orderIndex];
                targetCol = orderDropCols[orderIndex];
            }
            else
            {
                targetRow = orderPickupRows[orderIndex];
                targetCol = orderPickupCols[orderIndex];
            }

            const sf::Vector2f target =
                tileToPixelCenter(targetRow, targetCol);

            sf::CircleShape targetMarker(14.0f);
            targetMarker.setOrigin(14.0f, 14.0f);
            targetMarker.setPosition(target);

            if (orderPackagePickedUp[orderIndex])
            {
                targetMarker.setFillColor(
                    sf::Color(255, 211, 75, 150)
                );
            }
            else
            {
                targetMarker.setFillColor(
                    sf::Color(57, 220, 126, 150)
                );
            }

            targetMarker.setOutlineThickness(3.0f);
            targetMarker.setOutlineColor(sf::Color::White);
            window.draw(targetMarker);
        }

        // player.h ke mutabiq drawPlayer ko sprite dena zaroori hai.
        drawPlayer(window, riderSprite);

        // Right-side information panel.
        sf::RectangleShape sidePanel(
            sf::Vector2f(WINDOW_W - MAP_PIXEL_W, WINDOW_H)
        );

        sidePanel.setPosition(MAP_PIXEL_W, 0.0f);
        sidePanel.setFillColor(sf::Color(12, 27, 43));
        sidePanel.setOutlineThickness(-2.0f);
        sidePanel.setOutlineColor(sf::Color(68, 111, 137));
        window.draw(sidePanel);

        const float panelX = MAP_PIXEL_W + 28.0f;

        drawText(window, font, fontOk,
                 "DELIVERY SHIFT", 26, panelX, 30.0f,
                 sf::Color(255, 220, 133));

        drawText(window, font, fontOk,
                 "RIDER", 15, panelX, 92.0f,
                 sf::Color(117, 177, 201));

        drawText(window, font, fontOk,
                 RIDER_NAMES[currentCharacter] + " / " +
                 VEHICLE_NAMES[currentCharacter],
                 18, panelX, 116.0f, sf::Color::White);

        drawText(window, font, fontOk,
                 "Speed: " +
                 std::to_string(
                     static_cast<int>(
                         RIDER_MAX_SPEED[currentCharacter]
                     )
                 ),
                 18, panelX, 170.0f, sf::Color::White);

        drawText(window, font, fontOk,
                 "Cash: $" + std::to_string(orderScore),
                 20, panelX, 220.0f,
                 sf::Color(255, 220, 133));

        drawText(window, font, fontOk,
                 "Delivered: " + std::to_string(completedCount),
                 17, panelX, 260.0f, sf::Color::White);

        drawText(window, font, fontOk,
                 "Failed: " + std::to_string(failedCount) + " / 3",
                 17, panelX, 292.0f,
                 sf::Color(239, 151, 126));

        // Current order ki details.
        if (hasCurrentOrder() && failedCount < 3)
        {
            const int orderIndex = currentOrderIndex;

            drawText(window, font, fontOk,
                     "Order: " + orderShopNames[orderIndex],
                     17, panelX, 350.0f, sf::Color::White);

            if (orderPackagePickedUp[orderIndex])
            {
                drawText(window, font, fontOk,
                         "Deliver to: " +
                         orderCustomerNames[orderIndex],
                         16, panelX, 385.0f,
                         sf::Color(255, 220, 133));
            }
            else
            {
                drawText(window, font, fontOk,
                         "Pick up: " +
                         orderShopNames[orderIndex],
                         16, panelX, 385.0f,
                         sf::Color(115, 234, 156));
            }

            drawText(window, font, fontOk,
                     "Time left: " +
                     std::to_string(
                         static_cast<int>(
                             orderTimesRemaining[orderIndex]
                         )
                     ) + " sec",
                     18, panelX, 425.0f,
                     sf::Color::White);
        }

        drawText(window, font, fontOk,
                 orderStatusBanner,
                 14, panelX, 500.0f,
                 sf::Color(213, 230, 235));

        if (hittingObstacle)
        {
            drawText(window, font, fontOk,
                     "OBSTACLE: SPEED REDUCED",
                     16, panelX, 580.0f,
                     sf::Color(255, 121, 93));
        }

        drawText(window, font, fontOk,
                 "E: pick up / deliver",
                 15, panelX, 650.0f,
                 sf::Color(117, 177, 201));

        drawText(window, font, fontOk,
                 "WASD / Arrows: move   1-3: rider   R: reset   ESC: quit",
                 14, panelX, 694.0f,
                 sf::Color(117, 177, 201));

        // Shift result panel.
        if (shiftOver)
        {
            const bool won =
                completedCount == MAX_ORDERS;

            sf::RectangleShape resultPanel(
                sf::Vector2f(440.0f, 150.0f)
            );

            resultPanel.setPosition(
                MAP_PIXEL_W / 2.0f - 220.0f,
                WINDOW_H / 2.0f - 75.0f
            );

            resultPanel.setFillColor(
                sf::Color(12, 27, 43, 240)
            );

            resultPanel.setOutlineThickness(3.0f);

            if (won)
            {
                resultPanel.setOutlineColor(
                    sf::Color(115, 234, 156)
                );
            }
            else
            {
                resultPanel.setOutlineColor(
                    sf::Color(239, 151, 126)
                );
            }

            window.draw(resultPanel);

            if (won)
            {
                drawText(window, font, fontOk,
                         "SHIFT COMPLETE",
                         30,
                         MAP_PIXEL_W / 2.0f - 135.0f,
                         WINDOW_H / 2.0f - 45.0f,
                         sf::Color(255, 220, 133));
            }
            else
            {
                drawText(window, font, fontOk,
                         "SHIFT OVER",
                         30,
                         MAP_PIXEL_W / 2.0f - 100.0f,
                         WINDOW_H / 2.0f - 45.0f,
                         sf::Color(255, 220, 133));
            }

            drawText(window, font, fontOk,
                     "Press R to start a new shift",
                     18,
                     MAP_PIXEL_W / 2.0f - 130.0f,
                     WINDOW_H / 2.0f + 15.0f,
                     sf::Color::White);
        }

        window.display();
    }

    return 0;
}

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <cmath>

#include "Order.h"
#include "constant.h"
#include "Map.h"
#include "player.h"

const int MUSIC_TRACK_COUNT = 3;
const int SOUND_EFFECT_COUNT = 4;

std::string musicPaths[MUSIC_TRACK_COUNT] = {
    "assets/music/jeena_jeena.mp3",
    "assets/music/oh_oh_jane_jaana.mp3",
    "assets/music/o_re_piya.mp3"
};

std::string musicTitles[MUSIC_TRACK_COUNT] = {
    "Jeena Jeena",
    "Oh Oh Jane Jaana",
    "O Re Piya"
};

std::string soundPaths[SOUND_EFFECT_COUNT] = {
    "assets/sfx/pickup.wav",
    "assets/sfx/delivery.wav",
    "assets/sfx/collision.wav",
    "assets/sfx/failure.wav"
};

sf::SoundBuffer soundBuffers[SOUND_EFFECT_COUNT];
sf::Sound soundPlayers[SOUND_EFFECT_COUNT];
bool soundLoaded[SOUND_EFFECT_COUNT] = {false, false, false, false};
int selectedMusicTrack = 0;
std::string musicStatus = "Choose a song and press ENTER to play.";
bool musicScreenOpen = false;
bool wasOnObstacle = false;
GameState currentState = STATE_MENU;
int selectedCharacter = 0;

void loadGameSounds()
{
    for (int i = 0; i < SOUND_EFFECT_COUNT; i++)
    {
        soundLoaded[i] = soundBuffers[i].loadFromFile(soundPaths[i]);
        if (soundLoaded[i])
        {
            soundPlayers[i].setBuffer(soundBuffers[i]);
            soundPlayers[i].setVolume(65.0f);
        }
        else
        {
            std::cout << "Sound effect not found: "
                      << soundPaths[i] << std::endl;
        }
    }
}

void playGameSound(int soundIndex)
{
    if (soundIndex >= 0 && soundIndex < SOUND_EFFECT_COUNT &&
        soundLoaded[soundIndex])
    {
        soundPlayers[soundIndex].play();
    }
}

bool playSelectedMusic(sf::Music& music)
{
    music.stop();
    if (!music.openFromFile(musicPaths[selectedMusicTrack]))
    {
        musicStatus = "Track missing: " + musicPaths[selectedMusicTrack];
        std::cout << musicStatus << std::endl;
        return false;
    }

    music.setLoop(true);
    music.setVolume(35.0f);
    music.play();
    musicStatus = "Playing: " + musicTitles[selectedMusicTrack];
    return true;
}

void changeMusicTrack(int direction, sf::Music& music)
{
    selectedMusicTrack = selectedMusicTrack + direction;
    if (selectedMusicTrack < 0)
    {
        selectedMusicTrack = MUSIC_TRACK_COUNT - 1;
    }
    else if (selectedMusicTrack >= MUSIC_TRACK_COUNT)
    {
        selectedMusicTrack = 0;
    }

    if (music.getStatus() == sf::Music::Playing)
    {
        playSelectedMusic(music);
    }
    else
    {
        musicStatus = "Selected: " + musicTitles[selectedMusicTrack];
    }
}

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

void drawMusicPlayer(sf::RenderWindow& window, sf::Font& font, bool fontOk,
                     sf::Music& music)
{
    sf::RectangleShape shade(
        sf::Vector2f(WINDOW_W, WINDOW_H)
    );
    shade.setFillColor(sf::Color(3, 10, 18, 190));
    window.draw(shade);

    sf::RectangleShape panel(sf::Vector2f(650.0f, 500.0f));
    panel.setPosition(363.0f, 134.0f);
    panel.setFillColor(sf::Color(12, 31, 50, 248));
    panel.setOutlineThickness(3.0f);
    panel.setOutlineColor(sf::Color(82, 169, 188));
    window.draw(panel);

    drawText(window, font, fontOk, "MUSIC PLAYER", 36,
             520.0f, 165.0f, sf::Color(255, 220, 133));

    for (int i = 0; i < MUSIC_TRACK_COUNT; i++)
    {
        sf::RectangleShape row(sf::Vector2f(590.0f, 52.0f));
        row.setPosition(393.0f, 230.0f + i * 66.0f);
        row.setFillColor(i == selectedMusicTrack
            ? sf::Color(50, 100, 128)
            : sf::Color(24, 53, 77));
        row.setOutlineThickness(i == selectedMusicTrack ? 3.0f : 1.0f);
        row.setOutlineColor(i == selectedMusicTrack
            ? sf::Color(255, 220, 133)
            : sf::Color(87, 132, 157));
        window.draw(row);

        drawText(window, font, fontOk,
                 std::to_string(i + 1) + ".  " + musicTitles[i],
                 20, 418.0f, 242.0f + i * 66.0f,
                 sf::Color::White);
    }

    const sf::FloatRect buttonAreas[3] = {
        sf::FloatRect(393.0f, 450.0f, 175.0f, 54.0f),
        sf::FloatRect(601.0f, 450.0f, 175.0f, 54.0f),
        sf::FloatRect(809.0f, 450.0f, 175.0f, 54.0f)
    };
    const std::string buttonLabels[3] = {
        "PREVIOUS", "PLAY / PAUSE", "NEXT"
    };

    for (int i = 0; i < 3; i++)
    {
        sf::RectangleShape button(
            sf::Vector2f(buttonAreas[i].width, buttonAreas[i].height)
        );
        button.setPosition(buttonAreas[i].left, buttonAreas[i].top);
        button.setFillColor(sf::Color(37, 79, 105));
        button.setOutlineThickness(2.0f);
        button.setOutlineColor(sf::Color(82, 169, 188));
        window.draw(button);
        drawText(window, font, fontOk, buttonLabels[i], 14,
                 buttonAreas[i].left + 12.0f,
                 buttonAreas[i].top + 17.0f,
                 sf::Color::White);
    }

    std::string playbackStatus = musicStatus;
    if (music.getStatus() == sf::Music::Paused)
    {
        playbackStatus = "Music paused.";
    }
    else if (music.getStatus() == sf::Music::Stopped &&
             musicStatus.find("Playing:") == 0)
    {
        playbackStatus = "Music stopped.";
    }

    drawText(window, font, fontOk, playbackStatus, 16,
             405.0f, 525.0f, sf::Color(155, 221, 183));
    drawText(window, font, fontOk,
             "UP/DOWN: choose   ENTER: play   SPACE: pause",
             14, 405.0f, 565.0f, sf::Color(214, 229, 237));
    drawText(window, font, fontOk,
             "LEFT/RIGHT: change song   M or ESC: close",
             14, 405.0f, 592.0f, sf::Color(214, 229, 237));
}
void drawMainMenu(sf::RenderWindow& window, sf::Font& font, bool fontOk,
                  sf::Texture& menuTexture, bool menuArtLoaded)
{
    if (menuArtLoaded)
    {
        sf::Sprite background(menuTexture);
        sf::Vector2u imageSize = menuTexture.getSize();

        background.setScale(
            WINDOW_W / (float)imageSize.x,
            WINDOW_H / (float)imageSize.y
        );

        window.draw(background);
    }
    else
    {
        window.clear(sf::Color(35, 61, 76));
        drawText(window, font, fontOk, "LAST DELIVERY", 58,
                 115.0f, 100.0f, sf::Color(255, 220, 133));
    }

    // These click areas line up with the buttons in menu-reference.jpeg.
    sf::FloatRect buttons[3] = {
        sf::FloatRect(140.0f, 303.0f, 395.0f, 112.0f),
        sf::FloatRect(140.0f, 439.0f, 395.0f, 112.0f),
        sf::FloatRect(140.0f, 574.0f, 395.0f, 112.0f)
    };

    sf::Vector2i mouse = sf::Mouse::getPosition(window);

    for (int i = 0; i < 3; i++)
    {
        if (buttons[i].contains(mouse.x, mouse.y))
        {
            sf::RectangleShape highlight(
                sf::Vector2f(buttons[i].width, buttons[i].height)
            );

            highlight.setPosition(buttons[i].left, buttons[i].top);
            highlight.setFillColor(sf::Color(255, 220, 133, 35));
            highlight.setOutlineThickness(3.0f);
            highlight.setOutlineColor(sf::Color(255, 220, 133));
            window.draw(highlight);
        }
    }

    drawText(window, font, fontOk, "M: MUSIC PLAYER", 16,
             WINDOW_W - 205.0f, WINDOW_H - 38.0f, sf::Color::White);
}

void drawRiderCard(sf::RenderWindow& window, sf::Font& font, bool fontOk,
                   int rider, int selected, float x, sf::Sprite& vehicle)
{
    sf::RectangleShape card(sf::Vector2f(250.0f, 315.0f));
    card.setPosition(x, 200.0f);

    if (rider == selected)
        card.setFillColor(sf::Color(40, 86, 111));
    else
        card.setFillColor(sf::Color(20, 48, 70));

    card.setOutlineThickness(rider == selected ? 4.0f : 1.0f);

    if (rider == selected)
        card.setOutlineColor(sf::Color(255, 220, 133));
    else
        card.setOutlineColor(sf::Color(83, 132, 154));

    window.draw(card);

    sf::Sprite preview = vehicle;
    sf::IntRect area = preview.getTextureRect();
    preview.setOrigin(area.width / 2.0f, area.height / 2.0f);
    preview.setScale(0.10f, 0.10f);
    preview.setPosition(x + 125.0f, 275.0f);
    window.draw(preview);

    int riderSpeed = RIDER_MAX_SPEED[rider];

    drawText(window, font, fontOk,
             std::to_string(rider + 1) + ". " + RIDER_NAMES[rider],
             22, x + 20.0f, 350.0f, sf::Color::White);

    if (RIDER_IS_FEMALE[rider])
        drawText(window, font, fontOk, "Girl courier", 15,
                 x + 20.0f, 386.0f, sf::Color(166, 207, 221));
    else
        drawText(window, font, fontOk, "Boy courier", 15,
                 x + 20.0f, 386.0f, sf::Color(166, 207, 221));

    drawText(window, font, fontOk, VEHICLE_NAMES[rider], 16,
             x + 20.0f, 414.0f, sf::Color(255, 220, 133));

    drawText(window, font, fontOk,
             "Speed: " + std::to_string(riderSpeed),
             14, x + 20.0f, 455.0f, sf::Color(214, 229, 237));

    drawText(window, font, fontOk,
             "Score bonus: " +
             std::to_string(RIDER_SCORE_MULTIPLIER[rider]) + "x",
             14, x + 20.0f, 482.0f, sf::Color(214, 229, 237));
}

void drawRiderSelection(sf::RenderWindow& window, sf::Font& font,
                        bool fontOk, int selected,
                        sf::Sprite& cycle, sf::Sprite& motorcycle,
                        sf::Sprite& car)
{
    sf::RectangleShape shade(sf::Vector2f(WINDOW_W, WINDOW_H));
    shade.setFillColor(sf::Color(3, 10, 18, 205));
    window.draw(shade);

    sf::RectangleShape panel(sf::Vector2f(900.0f, 545.0f));
    panel.setPosition(238.0f, 110.0f);
    panel.setFillColor(sf::Color(12, 31, 50, 248));
    panel.setOutlineThickness(3.0f);
    panel.setOutlineColor(sf::Color(82, 169, 188));
    window.draw(panel);

    drawText(window, font, fontOk, "CHOOSE YOUR RIDER", 34,
             490.0f, 130.0f, sf::Color(255, 220, 133));

    drawRiderCard(window, font, fontOk, 0, selected, 278.0f, cycle);
    drawRiderCard(window, font, fontOk, 1, selected, 563.0f, motorcycle);
    drawRiderCard(window, font, fontOk, 2, selected, 848.0f, car);

    drawText(window, font, fontOk,
             "Click a rider or press 1, 2, or 3. ENTER starts.",
             15, 450.0f, 525.0f, sf::Color(214, 229, 237));

    sf::RectangleShape startButton(sf::Vector2f(280.0f, 56.0f));
    startButton.setPosition(548.0f, 570.0f);
    startButton.setFillColor(sf::Color(30, 135, 104));
    startButton.setOutlineThickness(2.0f);
    startButton.setOutlineColor(sf::Color(155, 221, 183));
    window.draw(startButton);

    drawText(window, font, fontOk, "START DELIVERY", 20,
             602.0f, 585.0f, sf::Color::White);
}

void drawInstructions(sf::RenderWindow& window, sf::Font& font, bool fontOk)
{
    sf::RectangleShape shade(sf::Vector2f(WINDOW_W, WINDOW_H));
    shade.setFillColor(sf::Color(3, 10, 18, 205));
    window.draw(shade);

    sf::RectangleShape panel(sf::Vector2f(820.0f, 510.0f));
    panel.setPosition(278.0f, 125.0f);
    panel.setFillColor(sf::Color(12, 31, 50, 248));
    panel.setOutlineThickness(3.0f);
    panel.setOutlineColor(sf::Color(82, 169, 188));
    window.draw(panel);

    drawText(window, font, fontOk, "HOW TO PLAY", 34,
             545.0f, 155.0f, sf::Color(255, 220, 133));

    drawText(window, font, fontOk, "WASD or arrow keys: drive", 20,
             360.0f, 245.0f, sf::Color::White);
    drawText(window, font, fontOk,
             "E: pick up a package or deliver it", 20,
             360.0f, 295.0f, sf::Color::White);
    drawText(window, font, fontOk,
             "Follow the red arrow to the marked place.", 20,
             360.0f, 345.0f, sf::Color::White);
    drawText(window, font, fontOk,
             "Avoid obstacles; they slow your vehicle.", 20,
             360.0f, 395.0f, sf::Color::White);
    drawText(window, font, fontOk,
             "P or ESC: pause   M: music   R: restart", 18,
             360.0f, 445.0f, sf::Color(166, 207, 221));

    sf::RectangleShape backButton(sf::Vector2f(260.0f, 56.0f));
    backButton.setPosition(558.0f, 535.0f);
    backButton.setFillColor(sf::Color(37, 79, 105));
    backButton.setOutlineThickness(2.0f);
    backButton.setOutlineColor(sf::Color(82, 169, 188));
    window.draw(backButton);

    drawText(window, font, fontOk, "BACK TO MENU", 18,
             608.0f, 551.0f, sf::Color::White);
}

void drawPauseScreen(sf::RenderWindow& window, sf::Font& font, bool fontOk)
{
    sf::RectangleShape shade(sf::Vector2f(WINDOW_W, WINDOW_H));
    shade.setFillColor(sf::Color(3, 10, 18, 210));
    window.draw(shade);

    sf::RectangleShape panel(sf::Vector2f(520.0f, 430.0f));
    panel.setPosition(428.0f, 165.0f);
    panel.setFillColor(sf::Color(12, 31, 50, 250));
    panel.setOutlineThickness(3.0f);
    panel.setOutlineColor(sf::Color(82, 169, 188));
    window.draw(panel);

    drawText(window, font, fontOk, "SHIFT PAUSED", 34,
             555.0f, 205.0f, sf::Color(255, 220, 133));

    const float buttonY[3] = {290.0f, 375.0f, 460.0f};
    const std::string labels[3] = {
        "RESUME", "MAIN MENU", "QUIT GAME"
    };

    for (int i = 0; i < 3; i++)
    {
        sf::RectangleShape button(sf::Vector2f(300.0f, 58.0f));
        button.setPosition(538.0f, buttonY[i]);
        button.setFillColor(sf::Color(37, 79, 105));
        button.setOutlineThickness(2.0f);
        button.setOutlineColor(sf::Color(82, 169, 188));
        window.draw(button);

        drawText(window, font, fontOk, labels[i], 19,
                 625.0f, buttonY[i] + 16.0f, sf::Color::White);
    }

    drawText(window, font, fontOk, "P or ESC: resume", 14,
             570.0f, 535.0f, sf::Color(166, 207, 221));
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

    // Har character ki sawari ki image load hoti hai.
    sf::Texture vehicleTextures[CHARACTER_COUNT];
    sf::Sprite vehicleSprites[CHARACTER_COUNT];
    bool vehicleLoaded[CHARACTER_COUNT] = {false, false, false};
    const std::string vehiclePaths[CHARACTER_COUNT] = {
        "assets/cycle.jpg",
        "assets/motorcycle.jpg",
        "assets/car.jpg"
    };

    for (int i = 0; i < CHARACTER_COUNT; i++)
    {
        sf::Image vehicleImage;
        if (vehicleImage.loadFromFile(vehiclePaths[i]))
        {
            // Cycle aur motorcycle ke bahar ke plain background ko transparent
            // banata hai. Car ka white body white background se milta hai,
            // is liye us image ko original rakha gaya hai.
            if (i < 2)
            {
                const sf::Color background = vehicleImage.getPixel(0, 0);
                const sf::Vector2u imageSize = vehicleImage.getSize();

                for (unsigned int y = 0; y < imageSize.y; y++)
                {
                    for (unsigned int x = 0; x < imageSize.x; x++)
                    {
                        const sf::Color pixel = vehicleImage.getPixel(x, y);
                        const bool matchesBackground =
                            std::abs(pixel.r - background.r) <= 32 &&
                            std::abs(pixel.g - background.g) <= 32 &&
                            std::abs(pixel.b - background.b) <= 32;

                        if (matchesBackground)
                        {
                            vehicleImage.setPixel(
                                x, y,
                                sf::Color(pixel.r, pixel.g, pixel.b, 0)
                            );
                        }
                    }
                }
            }

            const sf::Vector2u imageSize = vehicleImage.getSize();
            const int imageWidth = imageSize.x;
            const int imageHeight = imageSize.y;
            int minX = imageWidth;
            int minY = imageHeight;
            int maxX = -1;
            int maxY = -1;
            for (unsigned int y = 0; y < imageSize.y; y++)
            {
                for (unsigned int x = 0; x < imageSize.x; x++)
                {
                    const sf::Color pixel = vehicleImage.getPixel(x, y);
                    bool visiblePixel = pixel.a > 0;
                    if (i == 2)
                    {
                        visiblePixel = pixel.r < 245 || pixel.g < 245 ||
                                       pixel.b < 245;
                    }
                    if (visiblePixel)
                    {
                        const int pixelX = x;
                        const int pixelY = y;
                        if (pixelX < minX) minX = pixelX;
                        if (pixelY < minY) minY = pixelY;
                        if (pixelX > maxX) maxX = pixelX;
                        if (pixelY > maxY) maxY = pixelY;
                    }
                }
            }

            vehicleLoaded[i] = vehicleTextures[i].loadFromImage(vehicleImage);
            if (vehicleLoaded[i])
            {
                vehicleTextures[i].setSmooth(false);
                vehicleSprites[i].setTexture(vehicleTextures[i]);
                if (maxX > minX && maxY > minY)
                {
                    const int padding = 8;
                    const int left = minX > padding ? minX - padding : 0;
                    const int top = minY > padding ? minY - padding : 0;
                    const int right = maxX + padding < imageWidth
                        ? maxX + padding : imageWidth - 1;
                    const int bottom = maxY + padding < imageHeight
                        ? maxY + padding : imageHeight - 1;
                    vehicleSprites[i].setTextureRect(sf::IntRect(
                        left, top, right - left + 1, bottom - top + 1
                    ));
                }
            }
        }

        if (!vehicleLoaded[i])
        {
            std::cout << "Vehicle image not found: "
                      << vehiclePaths[i] << std::endl;
        }
    }

    sf::Image arrowImage;
    sf::Texture targetArrowTexture;
    bool targetArrowLoaded = false;
    if (arrowImage.loadFromFile("assets/orders/direction-arrow.png"))
    {
        const sf::Vector2u imageSize = arrowImage.getSize();
        for (unsigned int y = 0; y < imageSize.y; y++)
        {
            for (unsigned int x = 0; x < imageSize.x; x++)
            {
                const sf::Color pixel = arrowImage.getPixel(x, y);
                if (pixel.r > 245 && pixel.g > 245 && pixel.b > 245)
                {
                    arrowImage.setPixel(
                        x, y, sf::Color(pixel.r, pixel.g, pixel.b, 0)
                    );
                }
            }
        }
        targetArrowLoaded = targetArrowTexture.loadFromImage(arrowImage);
    }

    loadGameSounds();
    sf::Music backgroundMusic;

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
                if (musicScreenOpen)
                {
                    if (event.key.code == sf::Keyboard::Escape ||
                        event.key.code == sf::Keyboard::M)
                    {
                        musicScreenOpen = false;
                    }
                    else if (event.key.code == sf::Keyboard::Up)
                    {
                        changeMusicTrack(-1, backgroundMusic);
                    }
                    else if (event.key.code == sf::Keyboard::Down)
                    {
                        changeMusicTrack(1, backgroundMusic);
                    }
                    else if (event.key.code == sf::Keyboard::Left)
                    {
                        changeMusicTrack(-1, backgroundMusic);
                        playSelectedMusic(backgroundMusic);
                    }
                    else if (event.key.code == sf::Keyboard::Right)
                    {
                        changeMusicTrack(1, backgroundMusic);
                        playSelectedMusic(backgroundMusic);
                    }
                    else if (event.key.code == sf::Keyboard::Enter)
                    {
                        playSelectedMusic(backgroundMusic);
                    }
                    else if (event.key.code == sf::Keyboard::Space)
                    {
                        if (backgroundMusic.getStatus() == sf::Music::Playing)
                        {
                            backgroundMusic.pause();
                            musicStatus = "Music paused.";
                        }
                        else if (backgroundMusic.getStatus() == sf::Music::Paused)
                        {
                            backgroundMusic.play();
                            musicStatus = "Playing: " +
                                musicTitles[selectedMusicTrack];
                        }
                        else
                        {
                            playSelectedMusic(backgroundMusic);
                        }
                    }
                    continue;
                }

                if (event.key.code == sf::Keyboard::M)
                {
                    musicScreenOpen = true;
                    continue;
                }

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
                        playGameSound(0);

                        orderStatusBanner =
                            "Package picked up! Deliver it to " +
                            orderCustomerNames[currentOrderIndex] + ".";
                    }
                    else
                    {
                        completeCurrentOrder(
                            RIDER_SCORE_MULTIPLIER[currentCharacter]
                        );
                        playGameSound(1);
                    }
                }
            }

            if (musicScreenOpen &&
                event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                const float clickX = event.mouseButton.x;
                const float clickY = event.mouseButton.y;

                for (int i = 0; i < MUSIC_TRACK_COUNT; i++)
                {
                    const sf::FloatRect trackRow(
                        393.0f, 230.0f + i * 66.0f, 590.0f, 52.0f
                    );
                    if (trackRow.contains(clickX, clickY))
                    {
                        selectedMusicTrack = i;
                        playSelectedMusic(backgroundMusic);
                    }
                }

                if (sf::FloatRect(393.0f, 450.0f, 175.0f, 54.0f)
                        .contains(clickX, clickY))
                {
                    changeMusicTrack(-1, backgroundMusic);
                    playSelectedMusic(backgroundMusic);
                }
                else if (sf::FloatRect(601.0f, 450.0f, 175.0f, 54.0f)
                             .contains(clickX, clickY))
                {
                    if (backgroundMusic.getStatus() == sf::Music::Playing)
                    {
                        backgroundMusic.pause();
                        musicStatus = "Music paused.";
                    }
                    else if (backgroundMusic.getStatus() == sf::Music::Paused)
                    {
                        backgroundMusic.play();
                        musicStatus = "Playing: " +
                            musicTitles[selectedMusicTrack];
                    }
                    else
                    {
                        playSelectedMusic(backgroundMusic);
                    }
                }
                else if (sf::FloatRect(809.0f, 450.0f, 175.0f, 54.0f)
                             .contains(clickX, clickY))
                {
                    changeMusicTrack(1, backgroundMusic);
                    playSelectedMusic(backgroundMusic);
                }
            }
        }

        // Shift khatam ho chuki ho to rider aur timer ruk jayenge.
        const bool shiftOver =
            failedCount >= 3 || allOrdersCompleted();

        if (!shiftOver && !musicScreenOpen)
        {
            const bool obstacleWasHit = hittingObstacle;
            const bool wallWasHit = wallImpact;
            updatePlayerMovement(deltaTime);

            if ((hittingObstacle && !obstacleWasHit) ||
                (wallImpact && !wallWasHit))
            {
                playGameSound(2);
            }

            if (hasCurrentOrder())
            {
                updateOrderTimer(deltaTime);

                if (isOrderExpired())
                {
                    playGameSound(3);
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

            if (targetArrowLoaded)
            {
                sf::Sprite targetArrow(targetArrowTexture);
                const sf::Vector2u arrowSize =
                    targetArrowTexture.getSize();
                targetArrow.setOrigin(
                    arrowSize.x / 2.0f, arrowSize.y / 2.0f
                );
                targetArrow.setScale(0.30f, 0.30f);
                targetArrow.setPosition(target.x, target.y - 24.0f);
                window.draw(targetArrow);
            }
        }

        // Current rider ke mutabiq cycle, motorcycle, ya car draw hoti hai.
        drawPlayer(window, vehicleSprites[currentCharacter]);

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

        const int riderSpeed = RIDER_MAX_SPEED[currentCharacter];
        drawText(window, font, fontOk,
                 "Speed: " + std::to_string(riderSpeed),
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

            const int secondsLeft = orderTimesRemaining[orderIndex];
            drawText(window, font, fontOk,
                     "Time left: " + std::to_string(secondsLeft) + " sec",
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
                 "M: music player",
                 15, panelX, 622.0f,
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

        if (musicScreenOpen)
        {
            drawMusicPlayer(window, font, fontOk, backgroundMusic);
        }

        window.display();
    }

    backgroundMusic.stop();

    return 0;
}

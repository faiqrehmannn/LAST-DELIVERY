#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <iostream>
#include <string>

#include "constant.h"
#include "Map.h"
#include "player.h"
#include "Order.h"
#include "UIAndFile.h"

const int CHARACTER_CARD_X[CHARACTER_COUNT] = {55, 510, 965};
const int CHARACTER_CARD_Y = 132;
const int CHARACTER_CARD_W = 356;
const int CHARACTER_CARD_H = 548;
const int MUSIC_TRACK_COUNT = 4;
const float MUSIC_BUTTON_Y = 468.0f;
const float MUSIC_TRACK_ROW_Y = 220.0f;
const float MUSIC_TRACK_ROW_GAP = 56.0f;
const float MUSIC_TRACK_ROW_HEIGHT = 48.0f;
const std::string MUSIC_TRACK_PATHS[MUSIC_TRACK_COUNT] = {
    "assets/music/city_ride_original.wav",
    "assets/music/jeena_jeena.mp3",
    "assets/music/oh_oh_jane_jaana.mp3",
    "assets/music/o_re_piya.mp3"
};
const std::string MUSIC_TRACK_NAMES[MUSIC_TRACK_COUNT] = {
    "City Ride (Original)", "Jeena Jeena", "Oh Oh Jane Jaana", "O Re Piya"
};
const int GAME_SOUND_COUNT = 4;
const int SOUND_COLLISION = 0;
const int SOUND_PICKUP = 1;
const int SOUND_DELIVERY = 2;
const int SOUND_FAILURE = 3;
const std::string GAME_SOUND_PATHS[GAME_SOUND_COUNT] = {
    "assets/sfx/collision.wav",
    "assets/sfx/pickup.wav",
    "assets/sfx/delivery.wav",
    "assets/sfx/failure.wav"
};
int selectedMusicTrack = 0;
GameState stateBeforeMusic = STATE_MENU;
std::string musicMessage = "Choose a song, then press ENTER to play.";

const int MAX_ASSET_PIXELS = 800 * 800;
static int backgroundPixelStack[MAX_ASSET_PIXELS];

bool isNearBackground(sf::Color pixel, sf::Color background)
{
    const int tolerance = 22;
    return std::abs(pixel.r - background.r) <= tolerance &&
           std::abs(pixel.g - background.g) <= tolerance &&
           std::abs(pixel.b - background.b) <= tolerance;
}

// Flood-fill image background removal for vehicle textures
bool loadVehicleImage(sf::Texture& texture, sf::Sprite& sprite,
                      const std::string& filename)
{
    sf::Image image;
    if (!image.loadFromFile(filename))
    {
        std::cout << "Could not load vehicle image: " << filename << std::endl;
        return false;
    }

    const sf::Vector2u imageSize = image.getSize();
    const unsigned int pixelCount = imageSize.x * imageSize.y;

    if (pixelCount == 0 || pixelCount > MAX_ASSET_PIXELS)
    {
        std::cout << "Vehicle image has an unsupported size: "
                  << filename << std::endl;
        return false;
    }

    const int width = imageSize.x;
    const int height = imageSize.y;

    const sf::Color background = image.getPixel(0, 0);
    unsigned int stackSize = 0;
    image.setPixel(0, 0, sf::Color(0, 0, 0, 0));
    backgroundPixelStack[stackSize++] = 0;

    while (stackSize > 0)
    {
        const int pixelIndex = backgroundPixelStack[--stackSize];
        const int x = pixelIndex % width;
        const int y = pixelIndex / width;

        const int neighborX[4] = {
            x - 1,
            x + 1,
            x,
            x
        };
        const int neighborY[4] = {
            y,
            y,
            y - 1,
            y + 1
        };

        for (int direction = 0; direction < 4; direction++)
        {
            const int nextX = neighborX[direction];
            const int nextY = neighborY[direction];

            if (nextX < 0 || nextX >= width ||
                nextY < 0 || nextY >= height)
            {
                continue;
            }

            const sf::Color nextPixel = image.getPixel(
                nextX,
                nextY
            );

            if (nextPixel.a != 0 && isNearBackground(nextPixel, background))
            {
                image.setPixel(
                    nextX,
                    nextY,
                    sf::Color(0, 0, 0, 0)
                );

                if (stackSize < MAX_ASSET_PIXELS)
                {
                    backgroundPixelStack[stackSize++] = nextY * width + nextX;
                }
            }
        }
    }

    if (!texture.loadFromImage(image))
    {
        std::cout << "Could not prepare vehicle image: " << filename << std::endl;
        return false;
    }

    texture.setSmooth(false);
    sprite.setTexture(texture);
    return true;
}

bool playSelectedMusic(sf::Music& music)
{
    music.stop();
    if (!music.openFromFile(MUSIC_TRACK_PATHS[selectedMusicTrack]))
    {
        // Safe fallback to default track if custom track is missing
        if (selectedMusicTrack != 0 && music.openFromFile(MUSIC_TRACK_PATHS[0]))
        {
            music.setLoop(true);
            music.setVolume(28.0f);
            music.play();
            musicMessage = "Playing default: " + MUSIC_TRACK_NAMES[0];
            return true;
        }
        musicMessage = "Could not open " + MUSIC_TRACK_PATHS[selectedMusicTrack];
        std::cout << musicMessage << std::endl;
        return false;
    }

    music.setLoop(true);
    music.setVolume(28.0f);
    music.play();
    musicMessage = "Now playing: " + MUSIC_TRACK_NAMES[selectedMusicTrack];
    return true;
}

bool loadGameSound(sf::SoundBuffer& buffer, sf::Sound& sound,
                   const std::string& filename)
{
    if (!buffer.loadFromFile(filename))
    {
        std::cout << "Could not load sound effect: " << filename << std::endl;
        return false;
    }

    sound.setBuffer(buffer);
    sound.setVolume(65.0f);
    return true;
}

void playGameSound(sf::Sound (&sounds)[GAME_SOUND_COUNT],
                   const bool (&soundLoaded)[GAME_SOUND_COUNT], int soundIndex)
{
    if (soundIndex < 0 || soundIndex >= GAME_SOUND_COUNT ||
        !soundLoaded[soundIndex])
    {
        return;
    }

    if (sounds[soundIndex].getStatus() == sf::Sound::Playing)
        sounds[soundIndex].stop();
    sounds[soundIndex].play();
}

void moveMusicSelection(int direction, sf::Music& music)
{
    selectedMusicTrack += direction;
    if (selectedMusicTrack < 0) selectedMusicTrack = MUSIC_TRACK_COUNT - 1;
    if (selectedMusicTrack >= MUSIC_TRACK_COUNT) selectedMusicTrack = 0;

    if (music.getStatus() != sf::Music::Stopped)
        playSelectedMusic(music);
    else
        musicMessage = "Selected: " + MUSIC_TRACK_NAMES[selectedMusicTrack];
}

void drawText(sf::RenderWindow& window, sf::Font& font,
              const std::string& message, unsigned int size,
              float x, float y, sf::Color color)
{
    sf::Text text;
    text.setFont(font);
    text.setString(message);
    text.setCharacterSize(size);
    text.setPosition(x, y);
    text.setFillColor(color);
    window.draw(text);
}

void drawDimmedBackground(sf::RenderWindow& window, sf::Sprite& menuBackground)
{
    window.draw(menuBackground);
    sf::RectangleShape dimmer(sf::Vector2f(
        WINDOW_W,
        WINDOW_H
    ));
    dimmer.setFillColor(sf::Color(6, 18, 36, 205));
    window.draw(dimmer);
}

void drawMenu(sf::RenderWindow& window, sf::Font& font,
              sf::Sprite& menuBackground)
{
    window.clear(sf::Color(16, 28, 42));
    window.draw(menuBackground);

    const sf::Vector2i mouse = sf::Mouse::getPosition(window);
    const sf::FloatRect menuButtons[3] = {
        sf::FloatRect(140.0f, 295.0f, 400.0f, 125.0f),
        sf::FloatRect(140.0f, 435.0f, 400.0f, 125.0f),
        sf::FloatRect(140.0f, 570.0f, 400.0f, 120.0f)
    };

    for (int i = 0; i < 3; i++)
    {
        if (menuButtons[i].contains(
                mouse.x,
                mouse.y))
        {
            sf::RectangleShape hover(sf::Vector2f(
                menuButtons[i].width,
                menuButtons[i].height
            ));
            hover.setPosition(menuButtons[i].left, menuButtons[i].top);
            hover.setFillColor(sf::Color(255, 255, 255, 25));
            hover.setOutlineThickness(3.0f);
            hover.setOutlineColor(sf::Color(255, 224, 150));
            window.draw(hover);
        }
    }

    drawText(window, font, "H - HIGH SCORES", 18, 1120.0f, 724.0f,
             sf::Color::White);
    drawText(window, font, "M - MUSIC PLAYER", 18, 925.0f, 724.0f,
             sf::Color::White);
    window.display();
}

void drawMusicScreen(sf::RenderWindow& window, sf::Font& font,
                     sf::Sprite& menuBackground, sf::Music& music)
{
    window.clear(sf::Color(10, 25, 42));
    drawDimmedBackground(window, menuBackground);

    sf::RectangleShape panel(sf::Vector2f(760.0f, 555.0f));
    panel.setPosition(308.0f, 105.0f);
    panel.setFillColor(sf::Color(11, 31, 52, 245));
    panel.setOutlineThickness(3.0f);
    panel.setOutlineColor(sf::Color(93, 174, 193));
    window.draw(panel);

    drawText(window, font, "MUSIC PLAYER", 42, 525.0f, 145.0f,
             sf::Color(255, 220, 133));

    for (int i = 0; i < MUSIC_TRACK_COUNT; i++)
    {
        const float rowY = MUSIC_TRACK_ROW_Y + i * MUSIC_TRACK_ROW_GAP;
        sf::RectangleShape row(sf::Vector2f(600.0f, MUSIC_TRACK_ROW_HEIGHT));
        row.setPosition(388.0f, rowY);
        row.setFillColor(i == selectedMusicTrack
            ? sf::Color(55, 103, 132) : sf::Color(22, 49, 72));
        row.setOutlineThickness(i == selectedMusicTrack ? 3.0f : 1.0f);
        row.setOutlineColor(i == selectedMusicTrack
            ? sf::Color(255, 220, 133) : sf::Color(75, 115, 139));
        window.draw(row);

        drawText(window, font,
                 std::to_string(i + 1) + ".  " + MUSIC_TRACK_NAMES[i],
                 20, 420.0f, rowY + 11.0f, sf::Color::White);
    }

    // Music player control buttons
    const sf::FloatRect previousButton(400.0f, MUSIC_BUTTON_Y, 175.0f, 45.0f);
    const sf::FloatRect playButton(590.0f, MUSIC_BUTTON_Y, 205.0f, 45.0f);
    const sf::FloatRect nextButton(810.0f, MUSIC_BUTTON_Y, 175.0f, 45.0f);
    const sf::FloatRect musicButtons[3] = {
        previousButton, playButton, nextButton
    };
    const std::string musicButtonLabels[3] = {
        "PREVIOUS", "PLAY / PAUSE", "NEXT"
    };
    for (int i = 0; i < 3; i++)
    {
        sf::RectangleShape button(sf::Vector2f(
            musicButtons[i].width, musicButtons[i].height));
        button.setPosition(musicButtons[i].left, musicButtons[i].top);
        button.setFillColor(sf::Color(38, 83, 111));
        button.setOutlineThickness(2.0f);
        button.setOutlineColor(sf::Color(104, 161, 185));
        window.draw(button);
        drawText(window, font, musicButtonLabels[i], 15,
                 musicButtons[i].left + 12.0f,
                 musicButtons[i].top + 13.0f, sf::Color::White);
    }

    const bool isPlaying = music.getStatus() == sf::Music::Playing;
    drawText(window, font, isPlaying ? "PLAYING" : "STOPPED / PAUSED",
             18, 570.0f, 522.0f, sf::Color(170, 220, 190));
    drawText(window, font, musicMessage, 16, 405.0f, 550.0f,
             sf::Color(218, 230, 236));
    drawText(window, font,
             "Click a song to play it. Keyboard: UP/DOWN select, ENTER play, SPACE pause.",
             16, 385.0f, 587.0f, sf::Color::White);
    drawText(window, font, "LEFT/RIGHT: PREVIOUS/NEXT   ESC: BACK",
             16, 435.0f, 620.0f, sf::Color::White);
    window.display();
}

void drawCharacterSelection(sf::RenderWindow& window, sf::Font& font,
                            sf::Texture& characterTexture,
                            sf::Sprite (&vehicleSprites)[CHARACTER_COUNT],
                            int selectedCharacter)
{
    window.clear(sf::Color(9, 22, 42));

    sf::RectangleShape topBand(sf::Vector2f(
        WINDOW_W, 104.0f
    ));
    topBand.setFillColor(sf::Color(17, 48, 77));
    window.draw(topBand);

    drawText(window, font, "CHOOSE YOUR RIDER", 44, 450.0f, 27.0f,
             sf::Color(255, 220, 133));
    drawText(window, font, "Select a card, then press ENTER", 20,
             505.0f, 82.0f, sf::Color(215, 228, 237));

    const sf::IntRect portraitAreas[CHARACTER_COUNT] = {
        sf::IntRect(111, 82, 82, 198),
        sf::IntRect(300, 82, 82, 198),
        sf::IntRect(491, 82, 82, 198)
    };

    for (int i = 0; i < CHARACTER_COUNT; i++)
    {
        const float cardX = CHARACTER_CARD_X[i];
        const float cardY = CHARACTER_CARD_Y;

        sf::RectangleShape card(sf::Vector2f(
            CHARACTER_CARD_W,
            CHARACTER_CARD_H
        ));
        card.setPosition(cardX, cardY);
        card.setFillColor(i == selectedCharacter
            ? sf::Color(36, 74, 110)
            : sf::Color(24, 48, 77));
        card.setOutlineThickness(i == selectedCharacter ? 5.0f : 2.0f);
        card.setOutlineColor(i == selectedCharacter
            ? sf::Color(255, 208, 96)
            : sf::Color(95, 145, 180));
        window.draw(card);

        sf::RectangleShape nameBand(sf::Vector2f(
            CHARACTER_CARD_W - 4, 62.0f
        ));
        nameBand.setPosition(cardX + 2.0f, cardY + 2.0f);
        nameBand.setFillColor(sf::Color(66, 91, 137));
        window.draw(nameBand);

        const std::string gender = RIDER_IS_FEMALE[i] ? "WOMAN" : "MAN";
        drawText(window, font,
            RIDER_NAMES[i] + "  |  " + gender,
            25, cardX + 20.0f, cardY + 15.0f, sf::Color::White);

        sf::Sprite portrait(characterTexture);
        portrait.setTextureRect(portraitAreas[i]);
        portrait.setScale(1.78f, 1.38f);
        portrait.setPosition(cardX + 105.0f, cardY + 82.0f);
        window.draw(portrait);

        sf::RectangleShape vehicleBand(sf::Vector2f(
            CHARACTER_CARD_W - 20, 132.0f
        ));
        vehicleBand.setPosition(cardX + 10.0f, cardY + 365.0f);
        vehicleBand.setFillColor(sf::Color(13, 30, 49));
        window.draw(vehicleBand);

        sf::Sprite vehicle(vehicleSprites[i]);
        const sf::IntRect textureArea = vehicle.getTextureRect();
        vehicle.setOrigin(textureArea.width / 2.0f, textureArea.height / 2.0f);
        vehicle.setScale(0.16f, 0.16f);
        vehicle.setPosition(cardX + CHARACTER_CARD_W / 2.0f,
                            cardY + 428.0f);
        window.draw(vehicle);

        drawText(window, font, VEHICLE_NAMES[i], 19,
                 cardX + 22.0f, cardY + 510.0f,
                 sf::Color(238, 229, 207));
        const int riderSpeed = RIDER_MAX_SPEED[i];
        drawText(window, font,
                 "SPEED " + std::to_string(riderSpeed) +
                 "   SCORE x" + std::to_string(RIDER_SCORE_MULTIPLIER[i]),
                 15, cardX + 22.0f, cardY + 530.0f,
                 sf::Color(193, 213, 226));
    }

    drawText(window, font, "1 / 2 / 3: SELECT     ENTER: PLAY     ESC: BACK",
             20, 400.0f, 704.0f, sf::Color::White);
    window.display();
}

void drawInstructions(sf::RenderWindow& window, sf::Font& font,
                      sf::Sprite& menuBackground)
{
    window.clear(sf::Color(10, 25, 42));
    drawDimmedBackground(window, menuBackground);

    sf::RectangleShape panel(sf::Vector2f(920.0f, 520.0f));
    panel.setPosition(228.0f, 118.0f);
    panel.setFillColor(sf::Color(11, 31, 52, 242));
    panel.setOutlineThickness(3.0f);
    panel.setOutlineColor(sf::Color(93, 174, 193));
    window.draw(panel);

    drawText(window, font, "HOW TO PLAY", 44, 520.0f, 155.0f,
             sf::Color(255, 220, 133));
    drawText(window, font, "W A S D or arrow keys  -  drive", 26,
             350.0f, 250.0f, sf::Color::White);
    drawText(window, font, "Find the green marker to pick up an order", 24,
             350.0f, 315.0f, sf::Color(219, 233, 240));
    drawText(window, font, "Take it to the yellow marker before time runs out", 24,
             350.0f, 370.0f, sf::Color(219, 233, 240));
    drawText(window, font, "P  -  pause or continue", 24,
             350.0f, 435.0f, sf::Color::White);
    drawText(window, font, "R  -  restart this shift", 24,
             350.0f, 480.0f, sf::Color::White);
    drawText(window, font, "ESC  -  pause; press ESC again for the menu", 22,
             350.0f, 525.0f, sf::Color::White);
    drawText(window, font, "M  -  open the music player", 22,
             350.0f, 560.0f, sf::Color::White);
    drawText(window, font, "Press ESC to return", 20,
             568.0f, 590.0f, sf::Color(255, 220, 133));
    window.display();
}

void drawGame(sf::RenderWindow& window, sf::Font& font,
              sf::Sprite (&vehicleSprites)[CHARACTER_COUNT],
              sf::Texture& burgerShopTexture,
              sf::Texture& pizzaShopTexture,
              int selectedCharacter, GameState currentState)
{
    window.clear(sf::Color(13, 20, 29));
    drawCityMap(window, burgerShopTexture, pizzaShopTexture);

    if (!allOrdersCompleted())
    {
        const int activeOrder = currentOrderIndex;
        const int targetRow = orderPackagePickedUp[activeOrder]
            ? orderDropRows[activeOrder] : orderPickupRows[activeOrder];
        const int targetCol = orderPackagePickedUp[activeOrder]
            ? orderDropCols[activeOrder] : orderPickupCols[activeOrder];
        const sf::Vector2f target = tileToPixelCenter(targetRow, targetCol);

        // Current target indicator circle
        sf::CircleShape marker(21.0f);
        marker.setOrigin(21.0f, 21.0f);
        marker.setPosition(target);
        marker.setFillColor(orderPackagePickedUp[activeOrder]
            ? sf::Color(255, 211, 75, 45)
            : sf::Color(57, 220, 126, 45));
        marker.setOutlineThickness(4.0f);
        marker.setOutlineColor(orderPackagePickedUp[activeOrder]
            ? sf::Color(255, 211, 75, 235)
            : sf::Color(57, 220, 126, 235));
        window.draw(marker);
    }

    drawPlayer(window, vehicleSprites[selectedCharacter]);

    sf::RectangleShape sidePanel(sf::Vector2f(
        WINDOW_W - MAP_PIXEL_W,
        WINDOW_H
    ));
    sidePanel.setPosition(MAP_PIXEL_W, 0.0f);
    sidePanel.setFillColor(sf::Color(12, 27, 43));
    sidePanel.setOutlineThickness(-2.0f);
    sidePanel.setOutlineColor(sf::Color(68, 111, 137));
    window.draw(sidePanel);

    const float panelX = MAP_PIXEL_W + 28;
    drawText(window, font, "DELIVERY SHIFT", 26, panelX, 30.0f,
             sf::Color(255, 220, 133));
    drawText(window, font, "RIDER", 15, panelX, 92.0f,
             sf::Color(117, 177, 201));
    drawText(window, font,
             RIDER_NAMES[currentCharacter] + " / " + VEHICLE_NAMES[currentCharacter],
             18, panelX, 116.0f, sf::Color::White);

    sf::RectangleShape separator(sf::Vector2f(340.0f, 2.0f));
    separator.setPosition(panelX, 158.0f);
    separator.setFillColor(sf::Color(65, 103, 128));
    window.draw(separator);

    drawText(window, font, "CASH", 15, panelX, 183.0f,
             sf::Color(117, 177, 201));
    drawText(window, font, "$" + std::to_string(orderScore),
             28, panelX, 208.0f, sf::Color(255, 220, 133));
    drawText(window, font,
             "Delivered: " + std::to_string(completedCount),
             18, panelX, 265.0f, sf::Color::White);
    drawText(window, font,
             "Failed: " + std::to_string(failedCount) + " / 3",
             18, panelX, 298.0f, sf::Color(239, 151, 126));
    drawText(window, font,
             "Streak: " + std::to_string(streakCount),
             18, panelX, 331.0f, sf::Color(180, 224, 167));

    if (!allOrdersCompleted())
    {
        const int activeOrder = currentOrderIndex;
        drawText(window, font, "CURRENT ORDER", 15, panelX, 390.0f,
                 sf::Color(117, 177, 201));
        drawText(window, font, orderShopNames[activeOrder], 19, panelX, 416.0f,
                 sf::Color::White);
        drawText(window, font,
                 orderPackagePickedUp[activeOrder] ? "Deliver to:" : "Pick up at:",
                 17, panelX, 451.0f, sf::Color(207, 220, 228));
        drawText(window, font,
                 orderPackagePickedUp[activeOrder]
                     ? orderCustomerNames[activeOrder] : orderShopNames[activeOrder],
                 18, panelX, 478.0f, sf::Color(255, 220, 133));
        const int secondsRemaining = orderTimesRemaining[activeOrder];
        drawText(window, font,
                 "Time: " + std::to_string(secondsRemaining) + " sec",
                 22, panelX, 523.0f, sf::Color(255, 255, 255));
    }

    if (hittingObstacle)
    {
        drawText(window, font, "OBSTACLE: SPEED REDUCED", 16,
                 panelX, 580.0f, sf::Color(255, 121, 93));
    }

    drawText(window, font, orderStatusBanner, 15,
             panelX, 632.0f, sf::Color(213, 230, 235));
    drawText(window, font, "P: PAUSE   M: MUSIC", 15,
             panelX, 694.0f, sf::Color(117, 177, 201));

    if (currentState == STATE_PAUSED)
    {
        sf::RectangleShape overlay(sf::Vector2f(
            WINDOW_W,
            WINDOW_H
        ));
        overlay.setFillColor(sf::Color(0, 0, 0, 175));
        window.draw(overlay);
        drawText(window, font, "SHIFT PAUSED", 52, 495.0f, 310.0f,
                 sf::Color(255, 220, 133));
        drawText(window, font, "P: RESUME     R: RESTART", 22,
                 500.0f, 375.0f, sf::Color::White);
        drawText(window, font, "M: MUSIC PLAYER     ESC: MAIN MENU", 20,
                 470.0f, 425.0f, sf::Color::White);
    }

    window.display();
}

void drawEndScreen(sf::RenderWindow& window, sf::Font& font,
                   sf::Sprite& menuBackground, bool victory)
{
    window.clear(sf::Color(10, 25, 42));
    drawDimmedBackground(window, menuBackground);

    sf::RectangleShape panel(sf::Vector2f(820.0f, 420.0f));
    panel.setPosition(278.0f, 170.0f);
    panel.setFillColor(sf::Color(11, 31, 52, 242));
    panel.setOutlineThickness(3.0f);
    panel.setOutlineColor(victory
        ? sf::Color(99, 212, 145)
        : sf::Color(225, 113, 91));
    window.draw(panel);

    drawText(window, font, victory ? "SHIFT COMPLETE" : "SHIFT OVER",
             48, 490.0f, 220.0f,
             victory ? sf::Color(115, 234, 156) : sf::Color(255, 139, 115));
    drawText(window, font,
             "Final cash: $" + std::to_string(orderScore),
             29, 490.0f, 315.0f, sf::Color::White);
    drawText(window, font,
             "Delivered: " + std::to_string(completedCount) +
             "     Failed: " + std::to_string(failedCount),
             23, 490.0f, 370.0f, sf::Color(218, 230, 236));
    drawText(window, font, "R - PLAY AGAIN     ENTER - MENU",
             22, 490.0f, 455.0f, sf::Color(255, 220, 133));
    window.display();
}

int main()
{
    sf::RenderWindow window(
        sf::VideoMode(WINDOW_W, WINDOW_H),
        "Last Delivery | Group 11"
    );
    window.setFramerateLimit(60);
    sf::Music backgroundMusic;
    sf::SoundBuffer gameSoundBuffers[GAME_SOUND_COUNT];
    sf::Sound gameSounds[GAME_SOUND_COUNT];
    bool gameSoundLoaded[GAME_SOUND_COUNT] = {false, false, false, false};
    for (int soundIndex = 0; soundIndex < GAME_SOUND_COUNT; soundIndex++)
    {
        gameSoundLoaded[soundIndex] = loadGameSound(
            gameSoundBuffers[soundIndex], gameSounds[soundIndex],
            GAME_SOUND_PATHS[soundIndex]
        );
    }
    // Start background music
    playSelectedMusic(backgroundMusic);

    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf") &&
        !font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        std::cout << "Font not found. Place a font at assets/arial.ttf."
                  << std::endl;
    }

    sf::Texture menuTexture;
    sf::Sprite menuBackground;
    if (menuTexture.loadFromFile("assets/menu-reference.jpeg"))
    {
        menuTexture.setSmooth(false);
        menuBackground.setTexture(menuTexture);
    }
    else
    {
        std::cout << "Menu image not found: assets/menu-reference.jpeg" << std::endl;
    }

    sf::Texture characterTexture;
    if (!characterTexture.loadFromFile("assets/character-select-reference.jpg"))
    {
        std::cout << "Character reference image could not be loaded." << std::endl;
    }
    characterTexture.setSmooth(false);

    sf::Texture vehicleTextures[CHARACTER_COUNT];
    sf::Sprite vehicleSprites[CHARACTER_COUNT];
    loadVehicleImage(vehicleTextures[0], vehicleSprites[0], "assets/cycle.jpg");
    loadVehicleImage(vehicleTextures[1], vehicleSprites[1], "assets/motorcycle.jpg");
    loadVehicleImage(vehicleTextures[2], vehicleSprites[2], "assets/car.jpg");

    sf::Texture burgerShopTexture;
    if (!burgerShopTexture.loadFromFile("assets/burger-shop.png"))
    {
        std::cout << "Burger shop image not found: assets/burger-shop.png"
                  << std::endl;
    }
    burgerShopTexture.setSmooth(true);

    sf::Texture pizzaShopTexture;
    if (!pizzaShopTexture.loadFromFile("assets/pizza-shop.png"))
    {
        std::cout << "Pizza shop image not found: assets/pizza-shop.png"
                  << std::endl;
    }
    pizzaShopTexture.setSmooth(true);

    loadDefaultCity();
    resetOrders();
    loadHighScores();
    setupPlayer(0, tileToPixelCenter(4, 5));

    int selectedCharacter = 0;
    GameState currentState = STATE_MENU;
    bool highScoreSaved = false;
    bool wasOnObstacle = false;
    sf::Clock frameClock;
    sf::Clock collisionSoundClock;

    const sf::FloatRect startButton(140.0f, 295.0f, 400.0f, 125.0f);
    const sf::FloatRect instructionsButton(140.0f, 435.0f, 400.0f, 125.0f);
    const sf::FloatRect exitButton(140.0f, 570.0f, 400.0f, 120.0f);

    while (window.isOpen())
    {
        float deltaTime = frameClock.restart().asSeconds();
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
                if (event.key.code == sf::Keyboard::Escape)
                {
                    switch (currentState)
                    {
                        case STATE_MENU:
                            window.close();
                            break;
                        case STATE_PLAYING:
                            currentState = STATE_PAUSED;
                            break;
                        case STATE_MUSIC:
                            currentState = stateBeforeMusic;
                            break;
                        default:
                            currentState = STATE_MENU;
                            break;
                    }
                }
                else
                {
                    switch (currentState)
                    {
                        case STATE_MENU:
                            if (event.key.code == sf::Keyboard::Enter)
                            {
                                currentState = STATE_CHAR_SELECT;
                            }
                            else if (event.key.code == sf::Keyboard::I)
                            {
                                currentState = STATE_INSTRUCTIONS;
                            }
                            else if (event.key.code == sf::Keyboard::H)
                            {
                                loadHighScores();
                                currentState = STATE_LEADERBOARD;
                            }
                            else if (event.key.code == sf::Keyboard::M)
                            {
                                stateBeforeMusic = STATE_MENU;
                                currentState = STATE_MUSIC;
                            }
                            break;

                        case STATE_MUSIC:
                            if (event.key.code == sf::Keyboard::Up)
                                moveMusicSelection(-1, backgroundMusic);
                            else if (event.key.code == sf::Keyboard::Down)
                                moveMusicSelection(1, backgroundMusic);
                            else if (event.key.code == sf::Keyboard::Left)
                            {
                                moveMusicSelection(-1, backgroundMusic);
                                playSelectedMusic(backgroundMusic);
                            }
                            else if (event.key.code == sf::Keyboard::Right)
                            {
                                moveMusicSelection(1, backgroundMusic);
                                playSelectedMusic(backgroundMusic);
                            }
                            else if (event.key.code == sf::Keyboard::Enter)
                                playSelectedMusic(backgroundMusic);
                            else if (event.key.code == sf::Keyboard::Space)
                            {
                                if (backgroundMusic.getStatus() == sf::Music::Playing)
                                {
                                    backgroundMusic.pause();
                                    musicMessage = "Music paused.";
                                }
                                else if (backgroundMusic.getStatus() == sf::Music::Paused)
                                {
                                    backgroundMusic.play();
                                    musicMessage = "Now playing: " +
                                        MUSIC_TRACK_NAMES[selectedMusicTrack];
                                }
                                else
                                    playSelectedMusic(backgroundMusic);
                            }
                            break;

                        case STATE_CHAR_SELECT:
                            if (event.key.code == sf::Keyboard::Num1) selectedCharacter = 0;
                            else if (event.key.code == sf::Keyboard::Num2) selectedCharacter = 1;
                            else if (event.key.code == sf::Keyboard::Num3) selectedCharacter = 2;
                            else if (event.key.code == sf::Keyboard::Enter)
                            {
                                resetOrders();
                                setupPlayer(selectedCharacter, tileToPixelCenter(4, 5));
                                wasOnObstacle = false;
                                highScoreSaved = false;
                                currentState = STATE_PLAYING;
                            }
                            break;

                        case STATE_INSTRUCTIONS:
                        case STATE_LEADERBOARD:
                            if (event.key.code == sf::Keyboard::Enter)
                            {
                                currentState = STATE_MENU;
                            }
                            break;

                        case STATE_PLAYING:
                            if (event.key.code == sf::Keyboard::M)
                            {
                                stateBeforeMusic = STATE_PLAYING;
                                currentState = STATE_MUSIC;
                            }
                            else if (event.key.code == sf::Keyboard::P)
                            {
                                currentState = STATE_PAUSED;
                            }
                            else if (event.key.code == sf::Keyboard::R)
                            {
                                resetOrders();
                                setupPlayer(selectedCharacter, tileToPixelCenter(4, 5));
                                wasOnObstacle = false;
                                highScoreSaved = false;
                            }
                            break;

                        case STATE_PAUSED:
                            if (event.key.code == sf::Keyboard::M)
                            {
                                stateBeforeMusic = STATE_PAUSED;
                                currentState = STATE_MUSIC;
                            }
                            else if (event.key.code == sf::Keyboard::P)
                            {
                                currentState = STATE_PLAYING;
                            }
                            else if (event.key.code == sf::Keyboard::R)
                            {
                                resetOrders();
                                setupPlayer(selectedCharacter, tileToPixelCenter(4, 5));
                                wasOnObstacle = false;
                                highScoreSaved = false;
                                currentState = STATE_PLAYING;
                            }
                            break;

                        case STATE_GAME_OVER:
                        case STATE_VICTORY:
                            if (event.key.code == sf::Keyboard::R)
                            {
                                resetOrders();
                                setupPlayer(selectedCharacter, tileToPixelCenter(4, 5));
                                wasOnObstacle = false;
                                highScoreSaved = false;
                                currentState = STATE_PLAYING;
                            }
                            else if (event.key.code == sf::Keyboard::Enter)
                            {
                                currentState = STATE_MENU;
                            }
                            break;

                        default:
                            break;
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                const float mouseX = event.mouseButton.x;
                const float mouseY = event.mouseButton.y;

                switch (currentState)
                {
                    case STATE_MUSIC:
                    {
                        bool clickedTrack = false;
                        for (int i = 0; i < MUSIC_TRACK_COUNT; i++)
                        {
                            const sf::FloatRect trackRow(
                                388.0f,
                                MUSIC_TRACK_ROW_Y + i * MUSIC_TRACK_ROW_GAP,
                                600.0f, MUSIC_TRACK_ROW_HEIGHT);
                            if (trackRow.contains(mouseX, mouseY))
                            {
                                selectedMusicTrack = i;
                                playSelectedMusic(backgroundMusic);
                                clickedTrack = true;
                            }
                        }

                        if (!clickedTrack &&
                            sf::FloatRect(400.0f, MUSIC_BUTTON_Y, 175.0f, 45.0f)
                                .contains(mouseX, mouseY))
                        {
                            moveMusicSelection(-1, backgroundMusic);
                            playSelectedMusic(backgroundMusic);
                        }
                        else if (!clickedTrack &&
                            sf::FloatRect(590.0f, MUSIC_BUTTON_Y, 205.0f, 45.0f)
                                .contains(mouseX, mouseY))
                        {
                            if (backgroundMusic.getStatus() == sf::Music::Playing)
                            {
                                backgroundMusic.pause();
                                musicMessage = "Music paused.";
                            }
                            else if (backgroundMusic.getStatus() == sf::Music::Paused)
                            {
                                backgroundMusic.play();
                                musicMessage = "Now playing: " +
                                    MUSIC_TRACK_NAMES[selectedMusicTrack];
                            }
                            else
                                playSelectedMusic(backgroundMusic);
                        }
                        else if (!clickedTrack &&
                            sf::FloatRect(810.0f, MUSIC_BUTTON_Y, 175.0f, 45.0f)
                                .contains(mouseX, mouseY))
                        {
                            moveMusicSelection(1, backgroundMusic);
                            playSelectedMusic(backgroundMusic);
                        }
                        break;
                    }

                    case STATE_MENU:
                    {
                        if (startButton.contains(mouseX, mouseY))
                        {
                            currentState = STATE_CHAR_SELECT;
                        }
                        else if (instructionsButton.contains(mouseX, mouseY))
                        {
                            currentState = STATE_INSTRUCTIONS;
                        }
                        else if (exitButton.contains(mouseX, mouseY))
                        {
                            window.close();
                        }
                        break;
                    }

                    case STATE_CHAR_SELECT:
                    {
                        if (mouseY >= CHARACTER_CARD_Y &&
                            mouseY <= CHARACTER_CARD_Y + CHARACTER_CARD_H)
                        {
                            for (int i = 0; i < CHARACTER_COUNT; i++)
                            {
                                if (mouseX >= CHARACTER_CARD_X[i] &&
                                    mouseX <= CHARACTER_CARD_X[i] + CHARACTER_CARD_W)
                                {
                                    selectedCharacter = i;
                                }
                            }
                        }
                        break;
                    }

                    default:
                        break;
                }
            }
        }

        if (currentState == STATE_PLAYING)
        {
            const bool bumpedIntoWall = updatePlayerMovement(deltaTime);
            const bool onObstacle = isPlayerOnObstacle();
            if ((bumpedIntoWall &&
                 collisionSoundClock.getElapsedTime().asMilliseconds() >= 350) ||
                (onObstacle && !wasOnObstacle))
            {
                playGameSound(gameSounds, gameSoundLoaded, SOUND_COLLISION);
                collisionSoundClock.restart();
            }
            wasOnObstacle = onObstacle;

            if (!allOrdersCompleted())
            {
                updateOrderTimer(deltaTime);

                if (isOrderExpired())
                {
                    failCurrentOrder();
                    playGameSound(gameSounds, gameSoundLoaded, SOUND_FAILURE);
                    if (failedCount >= 3)
                    {
                        currentState = STATE_GAME_OVER;
                    }
                    else if (allOrdersCompleted())
                    {
                        currentState = completedCount > 0
                            ? STATE_VICTORY : STATE_GAME_OVER;
                    }
                }
                else
                {
                    const int activeOrder = currentOrderIndex;
                    const int targetRow = orderPackagePickedUp[activeOrder]
                        ? orderDropRows[activeOrder] : orderPickupRows[activeOrder];
                    const int targetCol = orderPackagePickedUp[activeOrder]
                        ? orderDropCols[activeOrder] : orderPickupCols[activeOrder];
                    const sf::Vector2f target = tileToPixelCenter(
                        targetRow, targetCol
                    );

                    const sf::FloatRect targetZone(
                        target.x - 28.0f,
                        target.y - 28.0f,
                        56.0f,
                        56.0f
                    );

                    if (getPlayerBounds().intersects(targetZone))
                    {
                        if (!orderPackagePickedUp[activeOrder])
                        {
                            orderPackagePickedUp[activeOrder] = true;
                            playGameSound(gameSounds, gameSoundLoaded, SOUND_PICKUP);
                            orderStatusBanner =
                                "Package picked up. Find the customer!";
                        }
                        else
                        {
                            playGameSound(gameSounds, gameSoundLoaded, SOUND_DELIVERY);
                            completeCurrentOrder(
                                RIDER_SCORE_MULTIPLIER[currentCharacter]
                            );
                            if (allOrdersCompleted())
                            {
                                currentState = STATE_VICTORY;
                            }
                        }
                    }
                }
            }
            else
            {
                currentState = completedCount > 0
                    ? STATE_VICTORY : STATE_GAME_OVER;
            }
        }

        if ((currentState == STATE_GAME_OVER ||
             currentState == STATE_VICTORY) && !highScoreSaved)
        {
            std::string scoreName = RIDER_NAMES[currentCharacter] + "-" +
                                    VEHICLE_NAMES[currentCharacter];
            for (std::size_t i = 0; i < scoreName.size(); i++)
            {
                if (scoreName[i] == ' ')
                {
                    scoreName[i] = '_';
                }
            }
            addHighScore(scoreName, orderScore);
            highScoreSaved = true;
        }

        switch (currentState)
        {
            case STATE_MENU:
                drawMenu(window, font, menuBackground);
                break;

            case STATE_CHAR_SELECT:
                drawCharacterSelection(
                    window, font, characterTexture,
                    vehicleSprites, selectedCharacter
                );
                break;

            case STATE_INSTRUCTIONS:
                drawInstructions(window, font, menuBackground);
                break;

            case STATE_LEADERBOARD:
                window.clear(sf::Color(10, 25, 42));
                drawDimmedBackground(window, menuBackground);
                {
                    sf::RectangleShape panel(sf::Vector2f(850.0f, 560.0f));
                    panel.setPosition(263.0f, 100.0f);
                    panel.setFillColor(sf::Color(11, 31, 52, 242));
                    panel.setOutlineThickness(3.0f);
                    panel.setOutlineColor(sf::Color(93, 174, 193));
                    window.draw(panel);
                }
                drawHighScores(window, font, 390.0f, 150.0f);
                drawText(window, font, "Press ENTER or ESC to return", 20,
                         490.0f, 650.0f, sf::Color(255, 220, 133));
                window.display();
                break;

            case STATE_MUSIC:
                drawMusicScreen(window, font, menuBackground, backgroundMusic);
                break;

            case STATE_PLAYING:
            case STATE_PAUSED:
                drawGame(window, font, vehicleSprites, burgerShopTexture,
                         pizzaShopTexture, selectedCharacter, currentState);
                break;

            case STATE_GAME_OVER:
            case STATE_VICTORY:
                drawEndScreen(window, font, menuBackground,
                              currentState == STATE_VICTORY);
                break;
        }
    }

    return 0;
}

#ifndef UI_AND_FILE_H
#define UI_AND_FILE_H

#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>

const int MAX_HIGH_SCORES = 10;
static std::string highScoreNames[MAX_HIGH_SCORES];
static int highScoreValues[MAX_HIGH_SCORES];
static int highScoreCount = 0;

static void sortHighScoresBubble()
{
    if (highScoreCount <= 1) return;

    bool swapped;
    do
    {
        swapped = false;
        for (int j = 0; j < highScoreCount - 1; j++)
        {
            if (highScoreValues[j] < highScoreValues[j + 1])
            {
                int savedScore = highScoreValues[j];
                highScoreValues[j] = highScoreValues[j + 1];
                highScoreValues[j + 1] = savedScore;

                std::string savedName = highScoreNames[j];
                highScoreNames[j] = highScoreNames[j + 1];
                highScoreNames[j + 1] = savedName;

                swapped = true;
            }
        }
    } while (swapped);
}

static void loadHighScores()
{
    highScoreCount = 0;
    std::ifstream inputFile("highscores.txt");
    if (!inputFile.is_open())
    {
        highScoreNames[0] = "Jimmy-Beamer"; highScoreValues[0] = 650;
        highScoreNames[1] = "Khan-Motorcycle"; highScoreValues[1] = 520;
        highScoreNames[2] = "Noor-Bicycle"; highScoreValues[2] = 410;
        highScoreCount = 3;
        return;
    }

    while (highScoreCount < MAX_HIGH_SCORES &&
           (inputFile >> highScoreNames[highScoreCount]
                      >> highScoreValues[highScoreCount]))
        highScoreCount++;

    inputFile.close();
    sortHighScoresBubble();
}

static void saveHighScores()
{
    std::ofstream outputFile("highscores.txt");
    if (!outputFile.is_open()) return;

    for (int i = 0; i < highScoreCount; i++)
        outputFile << highScoreNames[i] << " " << highScoreValues[i] << "\n";
    outputFile.close();
}

static void addHighScore(const std::string& riderName, int newScore)
{
    if (highScoreCount < MAX_HIGH_SCORES)
    {
        highScoreNames[highScoreCount] = riderName;
        highScoreValues[highScoreCount] = newScore;
        highScoreCount++;
    }
    else if (newScore > highScoreValues[highScoreCount - 1])
    {
        highScoreNames[highScoreCount - 1] = riderName;
        highScoreValues[highScoreCount - 1] = newScore;
    }

    sortHighScoresBubble();
    saveHighScores();
}

static void drawHighScores(sf::RenderWindow& window, sf::Font& font,
                           float x, float y)
{
    sf::Text heading;
    heading.setFont(font);
    heading.setString("HIGH SCORES");
    heading.setCharacterSize(28);
    heading.setFillColor(sf::Color::Yellow);
    heading.setPosition(x, y);
    window.draw(heading);

    for (int i = 0; i < highScoreCount; i++)
    {
        sf::Text scoreText;
        scoreText.setFont(font);
        scoreText.setString(std::to_string(i + 1) + ". " +
                            highScoreNames[i] + " - " +
                            std::to_string(highScoreValues[i]));
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(x, y + 45 + (i * 30));
        window.draw(scoreText);
    }
}

#endif

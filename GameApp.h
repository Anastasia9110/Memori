#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <fstream>
#include "Database.h"
#include "CardSelectionState.h"
#include "GameplayState.h"
#include "DuoGame.h"

class GameApp {
private:
    sf::RenderWindow& window;
    bool isRunning;
    Database db;
    PlayerProfile currentProfile;

    sf::Font pixelFont;
    sf::Font profileFont;
    sf::Music music;
    sf::Texture bgMenu, bgAbout, bgOptions, bgLevel;
    sf::Texture trophyTex;

    sf::SoundBuffer clickBuffer;
    sf::Sound clickSound;
    sf::SoundBuffer matchBuffer;
    sf::SoundBuffer errorBuffer;
    float sfxVolume;
    bool soundLoaded;

    void centerText(sf::Text& text, float x, float y);
    void drawPixelText(sf::RenderTarget& target, sf::Text text, float shadowOffset = 3.0f);
    bool checkClick(const sf::Text& btn, const sf::Vector2i& mousePos) const;
    bool checkClick(const sf::RectangleShape& btn, const sf::Vector2i& mousePos) const;
    bool loadResources();
    void loadSettings();
    void saveSettings();
    void playClickSound();

    void runMainMenu();
    void runDuoMenu();
    void runOptions();
    void runAbout();
    void runBestResults();
    void runThemeAndLevelSelect();
    void runLevelSelect(const std::string& selectedTheme);

    bool showExitConfirmation();
    bool showGameExitConfirmation();
    bool showExitToThemeConfirmation();

public:
    GameApp(sf::RenderWindow& win);
    ~GameApp();
    void run();
    sf::SoundBuffer* getClickBuffer();
    float getSfxVolume();
};
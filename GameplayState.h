#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Database.h"
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

struct Card {
    sf::RectangleShape shape;
    sf::Texture texture;
    sf::Sprite sprite;
    int value = 0;
    bool isFlipped = false;
    bool isMatched = false;
};

enum GameState { PLAYING, GAME_OVER, TIME_OUT };

class GameplayState {
private:
    sf::RenderWindow& window;
    Database* dbPtr;

    sf::Font font;
    sf::RectangleShape background;
    sf::RectangleShape overlay;

    std::vector<Card> cards;
    Card* firstCard;
    Card* secondCard;

    std::string currentTheme;
    GameState gameState;
    float flipTimer;

    int score;
    int moves;
    int mistakes;
    int pairsCount;
    float gameTime;
    float timeLimit;
    bool timeLimitEnabled;
    int maxMoves;
    bool movesLimitEnabled;
    int pairsNotFound;

    sf::Text scoreText;
    sf::Text movesText;
    sf::Text mistakesText;
    sf::Text timerText;
    sf::Text gameOverText;
    sf::Text timeOverText;
    sf::Text finalScoreText;
    sf::Text finalMovesText;

    sf::RectangleShape closeBtn;
    sf::RectangleShape btnMenu;
    sf::RectangleShape btnRetry;

    // ЗВУКИ ИГРЫ
    sf::SoundBuffer* matchBuffer;
    sf::SoundBuffer* errorBuffer;
    sf::Sound matchSound;
    sf::Sound errorSound;
    float sfxVolume;

    bool showGameExitConfirmation();
    void initLevel(int levelIndex);
    void setupCards(int pairs);

public:
    GameplayState(sf::RenderWindow& win, Database* db, sf::SoundBuffer* mBuf, sf::SoundBuffer* eBuf, float volume);
    void setTheme(const std::string& theme);
    int run(int levelIndex);
};
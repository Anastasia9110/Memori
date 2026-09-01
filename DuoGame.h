#ifndef DUOGAME_H
#define DUOGAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <set>

enum class DuoScreen {
    SelectPlayers,
    EnterNames,
    SelectTheme,
    SelectFirstPlayer,
    Playing,
    Results
};

struct Player {
    std::wstring name; // Используем wstring для корректной работы с кириллицей
    int pairsFound = 0;
    int mistakes = 0;
};

struct DuoCard {
    int id;
    bool isFlipped;
    bool isMatched;
    sf::RectangleShape shape;
    sf::Sprite sprite;
    sf::Texture* texture;
};

struct PlayerCountButton {
    sf::RectangleShape rect;
    sf::Text text;
    int value;
};

struct ThemeOption {
    sf::RectangleShape rect;
    sf::Text text;
    std::string themeId;
    std::string themePath;
};

class DuoGame {
private:
    sf::RenderWindow& window;
    sf::SoundBuffer* matchBuffer;
    sf::SoundBuffer* errorBuffer;
    sf::SoundBuffer* clickBuffer;

    sf::Sound matchSound;
    sf::Sound errorSound;
    sf::Sound clickSound;
    float sfxVolume;

    DuoScreen currentScreen;
    bool isRunning;

    std::vector<Player> players;
    int currentPlayerIndex;
    int playerCount;

    std::wstring currentInputName; // wstring вместо string для поддержки Unicode
    int selectedNameIndex;
    float cursorTimer;
    sf::String errorMessage;       // sf::String для корректного вывода текста с L""
    float errorTimer;
    std::set<std::wstring> usedNames;

    std::string selectedTheme;
    std::string selectedThemePath;

    int totalPairs;
    std::vector<DuoCard> cards;
    int firstCardIndex;
    int secondCardIndex;
    bool isProcessing;
    float processTimer;

    sf::Font font;
    sf::Font pixelFont;
    sf::Texture backgroundTexture;

    std::vector<PlayerCountButton> playerCountButtons;
    sf::RectangleShape backButton;
    sf::Text backButtonText;

    std::vector<ThemeOption> themeButtons;

    sf::RectangleShape closeBtn;
    sf::Text currentPlayerText;
    sf::Text scoreText;

    sf::RectangleShape resultsNextBtn;
    sf::Text resultsNextText;

    void playClickSound();
    void playMatchSound();
    void playErrorSound();

    void centerText(sf::Text& text, float x, float y);
    bool checkClick(const sf::RectangleShape& shape, const sf::Vector2f& pos) const;
    sf::Vector2f getMousePos() const;

    bool isNameUnique(const std::wstring& name) const;
    void showExitConfirmation();

    void initSelectPlayers();
    void updateSelectPlayers();
    void drawSelectPlayers();

    void initEnterNames();
    void updateEnterNames();
    void drawEnterNames();

    void initSelectTheme();
    void updateSelectTheme();
    void drawSelectTheme();

    void initSelectFirstPlayer();
    void updateSelectFirstPlayer();
    void drawSelectFirstPlayer();

    void createCards(int pairs, const std::string& themePath);
    void shuffleCards();
    void flipCard(int index);
    void resetFlippedCards();

    void initPlaying();
    void updatePlaying();
    void drawPlaying();

    void initResults();
    void updateResults();
    void drawResults();

public:
    DuoGame(sf::RenderWindow& win, sf::SoundBuffer* match, sf::SoundBuffer* error,
        float volume, const sf::Font& fontRef);
    ~DuoGame();

    void run();
    void handleEvents();
    void update();
    void draw();
};

#endif // DUOGAME_H
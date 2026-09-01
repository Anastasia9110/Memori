#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>

class CardSelectionState {
private:
    sf::RenderWindow& window;
    sf::Font font;
    sf::Texture* backgroundTexture;
    sf::SoundBuffer* clickBuffer;
    float sfxVolume;

    struct ThemeButton {
        sf::RectangleShape rect;
        sf::Text text;
        std::string folderName;
        bool isHovered = false;
    };

    ThemeButton backBtn;
    bool showDialog;
    sf::RectangleShape dialogBg;
    sf::Text dialogTitle;
    sf::Text dialogMessage;
    sf::RectangleShape btnYes;
    sf::RectangleShape btnNo;
    sf::Text txtYes;
    sf::Text txtNo;

    std::vector<ThemeButton> buttons;
    int selectedThemeIndex;
    bool isSelected;

    void initUI();
    void initDialog();
    void playClickSound();

public:
    CardSelectionState(sf::RenderWindow& win, sf::Texture* bgTexture, sf::SoundBuffer* sfxBuffer, float volume);
    ~CardSelectionState();

    void handleEvents();
    void update();
    void draw();

    std::string getSelectedTheme() const;
    bool isThemeSelected() const;
    bool shouldExitToMenu() const;

    void loadFont(const std::string& fontPath);
    void showDialogBox();
    bool isDialogShowing() const;
    void closeDialog();
};
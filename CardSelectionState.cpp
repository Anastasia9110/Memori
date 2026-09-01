#include "CardSelectionState.h"
#include <iostream>

CardSelectionState::CardSelectionState(sf::RenderWindow& win, sf::Texture* bgTexture, sf::SoundBuffer* sfxBuffer, float volume)
    : window(win), backgroundTexture(bgTexture), clickBuffer(sfxBuffer), sfxVolume(volume), selectedThemeIndex(-1), isSelected(false), showDialog(false) {
    loadFont("resources/Pixelify_Sans/PixelifySans.ttf");
    initUI();
    initDialog();
}

CardSelectionState::~CardSelectionState() {}

void CardSelectionState::playClickSound() {
    if (clickBuffer && sfxVolume > 0) {
        sf::Sound clickSound(*clickBuffer);
        clickSound.setVolume(sfxVolume);
        clickSound.play();
    }
}

void CardSelectionState::loadFont(const std::string& fontPath) {
    if (!font.loadFromFile(fontPath)) {
        std::cerr << "[ERROR] Шрифт не найден: " << fontPath << std::endl;
    }
}

void CardSelectionState::initUI() {
    float windowWidth = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);

    struct ThemeData {
        std::wstring displayName;
        std::string folderName;
        sf::Color color;
    };

    std::vector<ThemeData> themes = {
        {L"Животные", "animal", sf::Color(139, 90, 43)},
        {L"Ягоды", "Berry", sf::Color(220, 20, 60)},
        {L"Мультфильмы", "cartoons", sf::Color(138, 43, 226)},
        {L"Машины", "cars", sf::Color(255, 140, 0)}
    };

    float buttonWidth = 540.0f;
    float buttonHeight = 90.0f;
    float startX = (windowWidth - buttonWidth) / 2.0f;
    float startY = 210.0f;
    float spacing = 30.0f;

    for (size_t i = 0; i < themes.size(); i++) {
        ThemeButton btn;
        btn.folderName = themes[i].folderName;
        btn.isHovered = false;

        btn.rect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        btn.rect.setPosition(startX, startY + static_cast<float>(i) * (buttonHeight + spacing));
        btn.rect.setFillColor(sf::Color(34, 139, 34));
        btn.rect.setOutlineThickness(4.0f);
        btn.rect.setOutlineColor(sf::Color(80, 40, 80));

        btn.text.setFont(font);
        btn.text.setString(themes[i].displayName);
        btn.text.setCharacterSize(36u);
        btn.text.setFillColor(sf::Color(80, 40, 80));
        btn.text.setOutlineThickness(2.0f);
        btn.text.setOutlineColor(sf::Color::White);

        sf::FloatRect textBounds = btn.text.getLocalBounds();
        btn.text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
        btn.text.setPosition(startX + buttonWidth / 2.0f, startY + static_cast<float>(i) * (buttonHeight + spacing) + buttonHeight / 2.0f);

        buttons.push_back(btn);
    }

    backBtn.rect.setSize(sf::Vector2f(280.0f, 65.0f));
    backBtn.rect.setPosition((windowWidth - 280.0f) / 2.0f, windowHeight - 85.0f);
    backBtn.rect.setFillColor(sf::Color(34, 139, 34));
    backBtn.rect.setOutlineThickness(4.0f);
    backBtn.rect.setOutlineColor(sf::Color(80, 40, 80));
    backBtn.isHovered = false;

    backBtn.text.setFont(font);
    backBtn.text.setString(L"НАЗАД В МЕНЮ");
    backBtn.text.setCharacterSize(28u);
    backBtn.text.setFillColor(sf::Color(80, 40, 80));
    backBtn.text.setOutlineThickness(2.0f);
    backBtn.text.setOutlineColor(sf::Color::White);

    sf::FloatRect backTextBounds = backBtn.text.getLocalBounds();
    backBtn.text.setOrigin(backTextBounds.left + backTextBounds.width / 2.0f, backTextBounds.top + backTextBounds.height / 2.0f);
    backBtn.text.setPosition((windowWidth - 280.0f) / 2.0f + 140.0f, windowHeight - 85.0f + 32.0f);
}

void CardSelectionState::initDialog() {
    float windowWidth = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);

    dialogBg.setSize(sf::Vector2f(600.0f, 250.0f));
    dialogBg.setPosition((windowWidth - 600.0f) / 2.0f, (windowHeight - 250.0f) / 2.0f);
    dialogBg.setFillColor(sf::Color(30, 30, 50));
    dialogBg.setOutlineThickness(3.0f);
    dialogBg.setOutlineColor(sf::Color(100, 100, 150));

    dialogTitle.setFont(font);
    dialogTitle.setString(L"Подтверждение");
    dialogTitle.setCharacterSize(28u);
    dialogTitle.setFillColor(sf::Color(237, 147, 0));
    dialogTitle.setPosition((windowWidth - 600.0f) / 2.0f + 30.0f, (windowHeight - 250.0f) / 2.0f + 20.0f);

    dialogMessage.setFont(font);
    dialogMessage.setString(L"Вы действительно хотите выйти в главное меню?");
    dialogMessage.setCharacterSize(24u);
    dialogMessage.setFillColor(sf::Color::White);
    dialogMessage.setPosition((windowWidth - 600.0f) / 2.0f + 30.0f, (windowHeight - 250.0f) / 2.0f + 80.0f);

    btnYes.setSize(sf::Vector2f(150.0f, 50.0f));
    btnYes.setPosition((windowWidth - 320.0f) / 2.0f, (windowHeight - 250.0f) / 2.0f + 160.0f);
    btnYes.setFillColor(sf::Color(34, 139, 34));
    btnYes.setOutlineThickness(3.0f);
    btnYes.setOutlineColor(sf::Color(80, 40, 80));

    txtYes.setFont(font);
    txtYes.setString(L"ДА");
    txtYes.setCharacterSize(24u);
    txtYes.setFillColor(sf::Color(80, 40, 80));
    txtYes.setOutlineThickness(2.0f);
    txtYes.setOutlineColor(sf::Color::White);
    sf::FloatRect yesBounds = txtYes.getLocalBounds();
    txtYes.setOrigin(yesBounds.left + yesBounds.width / 2.0f, yesBounds.top + yesBounds.height / 2.0f);
    txtYes.setPosition((windowWidth - 320.0f) / 2.0f + 75.0f, (windowHeight - 250.0f) / 2.0f + 185.0f);

    btnNo.setSize(sf::Vector2f(150.0f, 50.0f));
    btnNo.setPosition((windowWidth + 20.0f) / 2.0f, (windowHeight - 250.0f) / 2.0f + 160.0f);
    btnNo.setFillColor(sf::Color(34, 139, 34));
    btnNo.setOutlineThickness(3.0f);
    btnNo.setOutlineColor(sf::Color(80, 40, 80));

    txtNo.setFont(font);
    txtNo.setString(L"НЕТ");
    txtNo.setCharacterSize(24u);
    txtNo.setFillColor(sf::Color(80, 40, 80));
    txtNo.setOutlineThickness(2.0f);
    txtNo.setOutlineColor(sf::Color::White);
    sf::FloatRect noBounds = txtNo.getLocalBounds();
    txtNo.setOrigin(noBounds.left + noBounds.width / 2.0f, noBounds.top + noBounds.height / 2.0f);
    txtNo.setPosition((windowWidth + 20.0f) / 2.0f + 75.0f, (windowHeight - 250.0f) / 2.0f + 185.0f);
}

void CardSelectionState::showDialogBox() { showDialog = true; }
void CardSelectionState::closeDialog() { showDialog = false; }
bool CardSelectionState::isDialogShowing() const { return showDialog; }
bool CardSelectionState::isThemeSelected() const { return isSelected; }
bool CardSelectionState::shouldExitToMenu() const { return isSelected && selectedThemeIndex == -1; }

void CardSelectionState::handleEvents() {
    if (showDialog) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { window.close(); return; }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF = static_cast<sf::Vector2f>(mousePos);
                if (btnYes.getGlobalBounds().contains(mousePosF)) {
                    playClickSound();
                    isSelected = true; selectedThemeIndex = -1; showDialog = false; return;
                }
                if (btnNo.getGlobalBounds().contains(mousePosF)) {
                    playClickSound();
                    showDialog = false; return;
                }
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) { showDialog = false; return; }
        }
        return;
    }

    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) { window.close(); return; }
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) { showDialog = true; return; }
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosF = static_cast<sf::Vector2f>(mousePos);
            if (backBtn.rect.getGlobalBounds().contains(mousePosF)) {
                playClickSound();
                showDialog = true; return;
            }
            for (size_t i = 0; i < buttons.size(); i++) {
                if (buttons[i].rect.getGlobalBounds().contains(mousePosF)) {
                    playClickSound();
                    selectedThemeIndex = static_cast<int>(i); isSelected = true; return;
                }
            }
        }
        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosF = static_cast<sf::Vector2f>(mousePos);
            if (backBtn.rect.getGlobalBounds().contains(mousePosF)) {
                if (!backBtn.isHovered) { backBtn.isHovered = true; backBtn.rect.setFillColor(sf::Color(50, 180, 50)); }
            }
            else { if (backBtn.isHovered) { backBtn.isHovered = false; backBtn.rect.setFillColor(sf::Color(34, 139, 34)); } }
            for (auto& btn : buttons) {
                if (btn.rect.getGlobalBounds().contains(mousePosF)) {
                    if (!btn.isHovered) { btn.isHovered = true; btn.rect.setFillColor(sf::Color(50, 180, 50)); }
                }
                else { if (btn.isHovered) { btn.isHovered = false; btn.rect.setFillColor(sf::Color(34, 139, 34)); } }
            }
        }
    }
}

void CardSelectionState::update() {}

void CardSelectionState::draw() {
    float windowWidth = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);

    if (backgroundTexture && backgroundTexture->getSize().x > 0) {
        sf::Sprite bgSprite;
        bgSprite.setTexture(*backgroundTexture);
        bgSprite.setScale(windowWidth / static_cast<float>(backgroundTexture->getSize().x), windowHeight / static_cast<float>(backgroundTexture->getSize().y));
        window.draw(bgSprite);
    }
    else { window.clear(sf::Color(30, 30, 50)); }

    sf::Text title;
    title.setFont(font);
    title.setString(L"Выберите набор карточек");
    title.setCharacterSize(50u);
    title.setFillColor(sf::Color(237, 147, 0));
    title.setOutlineThickness(3.0f);
    title.setOutlineColor(sf::Color::Black);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top + titleBounds.height / 2.0f);
    title.setPosition(windowWidth / 2.0f, 110.0f);
    window.draw(title);

    for (const auto& btn : buttons) { window.draw(btn.rect); window.draw(btn.text); }
    window.draw(backBtn.rect); window.draw(backBtn.text);

    if (showDialog) {
        sf::RectangleShape overlay(sf::Vector2f(windowWidth, windowHeight));
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(overlay);
        window.draw(dialogBg); window.draw(dialogTitle); window.draw(dialogMessage);
        window.draw(btnYes); window.draw(txtYes); window.draw(btnNo); window.draw(txtNo);
    }
}

std::string CardSelectionState::getSelectedTheme() const {
    if (selectedThemeIndex >= 0 && selectedThemeIndex < static_cast<int>(buttons.size())) {
        return buttons[selectedThemeIndex].folderName;
    }
    return "";
}
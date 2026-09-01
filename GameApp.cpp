#include "GameApp.h"
#include <iostream>
#include <sstream>
#include <string>
#include <direct.h>

#pragma warning(disable: 26495)

GameApp::GameApp(sf::RenderWindow& win)
    : window(win), isRunning(true), db("game_data.db"), sfxVolume(100.0f), soundLoaded(false) {

    if (!db.init()) {
        std::cerr << "[ERROR] Ошибка инициализации БД!" << std::endl;
    }

    currentProfile = db.loadProfile();

    if (!loadResources()) {
        std::cerr << "[ERROR] Ошибка загрузки ресурсов!" << std::endl;
        isRunning = false;
    }

    loadSettings();
}

GameApp::~GameApp() {
    music.stop();
    saveSettings();
}

sf::SoundBuffer* GameApp::getClickBuffer() {
    return soundLoaded ? &clickBuffer : nullptr;
}

float GameApp::getSfxVolume() {
    return sfxVolume;
}

void GameApp::loadSettings() {
    std::ifstream file("settings.ini");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("music_volume=") == 0) {
                float vol = std::stof(line.substr(13));
                music.setVolume(vol);
            }
            if (line.find("sfx_volume=") == 0) {
                sfxVolume = std::stof(line.substr(11));
                if (soundLoaded) clickSound.setVolume(sfxVolume);
            }
        }
        file.close();
        std::cout << "[OK] Настройки загружены" << std::endl;
    }
}

void GameApp::saveSettings() {
    std::ofstream file("settings.ini");
    if (file.is_open()) {
        file << "music_volume=" << music.getVolume() << "\n";
        file << "sfx_volume=" << sfxVolume << "\n";
        file.close();
        std::cout << "[OK] Настройки сохранены" << std::endl;
    }
}

void GameApp::playClickSound() {
    if (soundLoaded && sfxVolume > 0) {
        clickSound.play();
    }
}

bool GameApp::loadResources() {
    std::cout << "\n[INFO] Загрузка ресурсов..." << std::endl;

    if (!pixelFont.loadFromFile("resources/Pixelify_Sans/PixelifySans.ttf")) {
        std::cerr << "[ERROR] PixelifySans.ttf не найден!" << std::endl;
        return false;
    }
    std::cout << "[OK] PixelifySans загружен" << std::endl;

    if (!profileFont.loadFromFile("resources/Roboto/Roboto.ttf")) {
        std::cout << "[WARNING] Roboto.ttf не найден" << std::endl;
        profileFont = pixelFont;
    }

    if (!music.openFromFile("resources/music/menu.ogg")) {
        std::cout << "[WARNING] Музыка не найдена" << std::endl;
    }
    else {
        music.setLoop(true);
        music.play();
    }

    if (!clickBuffer.loadFromFile("resources/music/click.wav")) {
        std::cout << "[WARNING] Звук клика не найден" << std::endl;
        soundLoaded = false;
    }
    else {
        clickSound.setBuffer(clickBuffer);
        clickSound.setVolume(sfxVolume);
        soundLoaded = true;
        std::cout << "[OK] Звук клика загружен" << std::endl;
    }

    if (!matchBuffer.loadFromFile("resources/music/match.wav")) {
        std::cout << "[WARNING] match.wav не найден" << std::endl;
    }
    else { std::cout << "[OK] match.wav загружен" << std::endl; }

    if (!errorBuffer.loadFromFile("resources/music/error.wav")) {
        std::cout << "[WARNING] error.wav не найден" << std::endl;
    }
    else { std::cout << "[OK] error.wav загружен" << std::endl; }

    if (!bgMenu.loadFromFile("img/menu1.jpg")) { std::cerr << "[ERROR] img/menu1.jpg\n"; return false; }
    if (!bgAbout.loadFromFile("img/1.jpg")) { std::cerr << "[ERROR] img/1.jpg\n"; return false; }
    if (!bgOptions.loadFromFile("img/menu3.jpg")) { std::cerr << "[ERROR] img/menu3.jpg\n"; return false; }
    bgLevel = bgMenu;

    if (!trophyTex.loadFromFile("img/кубок.jpg")) {
        std::cout << "[INFO] кубок.jpg не найден" << std::endl;
    }

    std::cout << "[SUCCESS] ВСЕ РЕСУРСЫ ЗАГРУЖЕНЫ!\n" << std::endl;
    return true;
}

void GameApp::centerText(sf::Text& text, float x, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    text.setPosition(x, y);
}

void GameApp::drawPixelText(sf::RenderTarget& target, sf::Text text, float shadowOffset) {
    sf::Color originalColor = text.getFillColor();
    text.setFillColor(sf::Color(0, 0, 0, 160));
    text.move(shadowOffset, shadowOffset);
    target.draw(text);
    text.setFillColor(originalColor);
    text.move(-shadowOffset, -shadowOffset);
    target.draw(text);
}

bool GameApp::checkClick(const sf::Text& btn, const sf::Vector2i& mousePos) const {
    return btn.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

bool GameApp::checkClick(const sf::RectangleShape& btn, const sf::Vector2i& mousePos) const {
    return btn.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

bool GameApp::showExitConfirmation() {
    sf::RenderWindow confirmWin(sf::VideoMode(600, 250), L"Подтверждение", sf::Style::Titlebar | sf::Style::Close);
    confirmWin.setPosition(sf::Vector2i(660, 415));
    confirmWin.setMouseCursorVisible(true);

    sf::Font localFont = pixelFont;
    sf::Text question;
    question.setFont(localFont);
    question.setString(L"Вы действительно хотите выйти?");
    question.setCharacterSize(28u);
    question.setFillColor(sf::Color(80, 40, 80));
    question.setOutlineThickness(2.0f);
    question.setOutlineColor(sf::Color::White);
    centerText(question, 300.0f, 60.0f);

    sf::RectangleShape btnYes(sf::Vector2f(150.0f, 50.0f));
    btnYes.setPosition(150.0f, 140.0f);
    btnYes.setFillColor(sf::Color(34, 139, 34));
    btnYes.setOutlineThickness(4.0f);
    btnYes.setOutlineColor(sf::Color(80, 40, 80));

    sf::Text txtYes;
    txtYes.setFont(localFont);
    txtYes.setString(L"ДА");
    txtYes.setCharacterSize(24u);
    txtYes.setFillColor(sf::Color(80, 40, 80));
    txtYes.setOutlineThickness(2.0f);
    txtYes.setOutlineColor(sf::Color::White);
    txtYes.setPosition(183.0f, 152.0f);

    sf::RectangleShape btnNo(sf::Vector2f(150.0f, 50.0f));
    btnNo.setPosition(300.0f, 140.0f);
    btnNo.setFillColor(sf::Color(34, 139, 34));
    btnNo.setOutlineThickness(4.0f);
    btnNo.setOutlineColor(sf::Color(80, 40, 80));

    sf::Text txtNo;
    txtNo.setFont(localFont);
    txtNo.setString(L"НЕТ");
    txtNo.setCharacterSize(24u);
    txtNo.setFillColor(sf::Color(80, 40, 80));
    txtNo.setOutlineThickness(2.0f);
    txtNo.setOutlineColor(sf::Color::White);
    txtNo.setPosition(333.0f, 152.0f);

    while (confirmWin.isOpen()) {
        sf::Event event;
        while (confirmWin.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { confirmWin.close(); return false; }
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mPos = sf::Mouse::getPosition(confirmWin);
                if (btnYes.getGlobalBounds().contains(static_cast<sf::Vector2f>(mPos))) { confirmWin.close(); return true; }
                if (btnNo.getGlobalBounds().contains(static_cast<sf::Vector2f>(mPos))) { confirmWin.close(); return false; }
            }
        }
        sf::Vector2i mPos = sf::Mouse::getPosition(confirmWin);
        btnYes.setFillColor(btnYes.getGlobalBounds().contains(static_cast<sf::Vector2f>(mPos)) ? sf::Color(50, 180, 50) : sf::Color(34, 139, 34));
        btnNo.setFillColor(btnNo.getGlobalBounds().contains(static_cast<sf::Vector2f>(mPos)) ? sf::Color(50, 180, 50) : sf::Color(34, 139, 34));

        confirmWin.clear(sf::Color(30, 30, 50));
        confirmWin.draw(question); confirmWin.draw(btnYes); confirmWin.draw(txtYes);
        confirmWin.draw(btnNo); confirmWin.draw(txtNo);
        confirmWin.display();
    }
    return false;
}

bool GameApp::showGameExitConfirmation() { return showExitConfirmation(); }
bool GameApp::showExitToThemeConfirmation() { return showExitConfirmation(); }

void GameApp::runBestResults() {
    playClickSound();
    sf::RenderWindow userWin(sf::VideoMode(550, 750), L"Лучшие результаты", sf::Style::Titlebar | sf::Style::Close);
    userWin.setPosition(sf::Vector2i(1000, 170));

    sf::Text title;
    title.setFont(profileFont);
    title.setString(L"ЛУЧШИЕ РЕЗУЛЬТАТЫ");
    title.setCharacterSize(36);
    title.setFillColor(sf::Color(237, 147, 0));
    title.setOutlineThickness(2);
    title.setOutlineColor(sf::Color::Black);
    centerText(title, 275.0f, 40.0f);

    std::wstringstream ss;
    ss << L"УРОВНИ:\n\n";
    for (int i = 0; i < 4; ++i) {
        ss << L"Ур." << (i + 1) << L": ";
        if (currentProfile.levels[i].bestTime > 0) {
            ss << static_cast<int>(currentProfile.levels[i].bestTime) << L" сек | ";
            ss << L"Мин.ошибок: " << currentProfile.levels[i].minMistakes << L"\n";
        }
        else {
            ss << L"Не пройден\n";
        }
    }
    ss << L"\n----------------\n";
    ss << L"Всего игр: " << currentProfile.totalGames << L"\n";
    ss << L"Общий счёт: " << currentProfile.totalScore << L"\n";

    sf::Text info;
    info.setFont(profileFont);
    info.setString(ss.str());
    info.setCharacterSize(18);
    info.setFillColor(sf::Color::White);
    info.setPosition(40.0f, 90.0f);
    info.setLineSpacing(1.4f);

    sf::RectangleShape resetBtn(sf::Vector2f(240.0f, 55.0f));
    resetBtn.setPosition(155.0f, 660.0f);
    resetBtn.setFillColor(sf::Color(34, 139, 34));
    resetBtn.setOutlineThickness(4.0f);
    resetBtn.setOutlineColor(sf::Color(80, 40, 80));

    sf::Text resetText;
    resetText.setFont(profileFont);
    resetText.setString(L"СБРОСИТЬ");
    resetText.setCharacterSize(24);
    resetText.setFillColor(sf::Color(80, 40, 80));
    resetText.setOutlineThickness(2.0f);
    resetText.setOutlineColor(sf::Color::White);
    sf::FloatRect rtb = resetText.getLocalBounds();
    resetText.setOrigin(rtb.left + rtb.width / 2.0f, rtb.top + rtb.height / 2.0f);
    resetText.setPosition(275.0f, 687.0f);

    while (userWin.isOpen()) {
        sf::Event event;
        while (userWin.pollEvent(event)) {
            if (event.type == sf::Event::Closed) userWin.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) userWin.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mPos = sf::Mouse::getPosition(userWin);
                if (resetBtn.getGlobalBounds().contains(static_cast<sf::Vector2f>(mPos))) {
                    sf::RenderWindow confirmWin(sf::VideoMode(420, 220), L"Подтверждение", sf::Style::Titlebar | sf::Style::Close);
                    confirmWin.setPosition(sf::Vector2i(910, 370));

                    sf::Text confirmText;
                    confirmText.setFont(profileFont);
                    confirmText.setString(L"Сбросить весь прогресс?");
                    confirmText.setCharacterSize(24);
                    confirmText.setFillColor(sf::Color(237, 147, 0));
                    confirmText.setOutlineThickness(2.0f);
                    confirmText.setOutlineColor(sf::Color::Black);
                    confirmText.setPosition(50.0f, 45.0f);

                    sf::RectangleShape btnYes(sf::Vector2f(130.0f, 45.0f));
                    btnYes.setPosition(65.0f, 120.0f);
                    btnYes.setFillColor(sf::Color(34, 139, 34));
                    btnYes.setOutlineThickness(3.0f);
                    btnYes.setOutlineColor(sf::Color(80, 40, 80));

                    sf::Text txtYes;
                    txtYes.setFont(profileFont);
                    txtYes.setString(L"ДА");
                    txtYes.setCharacterSize(20);
                    txtYes.setFillColor(sf::Color(80, 40, 80));
                    txtYes.setOutlineThickness(2.0f);
                    txtYes.setOutlineColor(sf::Color::White);
                    txtYes.setPosition(100.0f, 128.0f);

                    sf::RectangleShape btnNo(sf::Vector2f(130.0f, 45.0f));
                    btnNo.setPosition(225.0f, 120.0f);
                    btnNo.setFillColor(sf::Color(34, 139, 34));
                    btnNo.setOutlineThickness(3.0f);
                    btnNo.setOutlineColor(sf::Color(80, 40, 80));

                    sf::Text txtNo;
                    txtNo.setFont(profileFont);
                    txtNo.setString(L"НЕТ");
                    txtNo.setCharacterSize(20);
                    txtNo.setFillColor(sf::Color(80, 40, 80));
                    txtNo.setOutlineThickness(2.0f);
                    txtNo.setOutlineColor(sf::Color::White);
                    txtNo.setPosition(260.0f, 128.0f);

                    while (confirmWin.isOpen()) {
                        sf::Event ce;
                        while (confirmWin.pollEvent(ce)) {
                            if (ce.type == sf::Event::Closed) confirmWin.close();
                            if (ce.type == sf::Event::MouseButtonPressed) {
                                sf::Vector2i mp = sf::Mouse::getPosition(confirmWin);
                                if (btnYes.getGlobalBounds().contains(static_cast<sf::Vector2f>(mp))) {
                                    db.resetProgress();
                                    currentProfile = db.loadProfile();
                                    confirmWin.close();
                                    userWin.close();
                                }
                                if (btnNo.getGlobalBounds().contains(static_cast<sf::Vector2f>(mp))) confirmWin.close();
                            }
                        }
                        confirmWin.clear(sf::Color(30, 30, 50));
                        confirmWin.draw(confirmText);
                        confirmWin.draw(btnYes); confirmWin.draw(txtYes);
                        confirmWin.draw(btnNo); confirmWin.draw(txtNo);
                        confirmWin.display();
                    }
                }
            }
        }
        userWin.clear(sf::Color(30, 30, 40));
        userWin.draw(title);
        userWin.draw(info);
        userWin.draw(resetBtn);
        userWin.draw(resetText);
        userWin.display();
    }
}

void GameApp::runLevelSelect(const std::string& selectedTheme) {
    sf::Sprite backgroundSprite;
    if (bgLevel.getSize().x > 0) {
        backgroundSprite.setTexture(bgLevel);
        backgroundSprite.setScale(1920.0f / bgLevel.getSize().x, 1080.0f / bgLevel.getSize().y);
    }

    sf::Text title;
    title.setFont(pixelFont);
    title.setString(L"Выберите уровень");
    title.setCharacterSize(60);
    title.setFillColor(sf::Color(237, 147, 0));
    title.setOutlineThickness(3);
    title.setOutlineColor(sf::Color::Black);
    centerText(title, 960.0f, 80.0f);

    struct LevelButton {
        sf::RectangleShape rect;
        sf::Text text;
        bool isLocked = false;
    };

    std::vector<LevelButton> levelButtons;
    sf::String levelNames[] = {
        L"Уровень 1 (8 пар)", L"Уровень 2 (12 пар, 2 мин)",
        L"Уровень 3 (17 пар, 1:30)", L"Уровень 4 (ВСЕ карты! 1:30 + 30 ходов)"
    };

    float buttonWidth = 550.0f, buttonHeight = 75.0f;
    float startX = (1920.0f - buttonWidth) / 2.0f, startY = 180.0f, spacing = 25.0f;

    for (int i = 0; i < 4; i++) {
        LevelButton btn;
        btn.isLocked = !currentProfile.levels[i].unlocked;

        btn.rect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        btn.rect.setPosition(startX, startY + i * (buttonHeight + spacing));

        if (btn.isLocked) { btn.rect.setFillColor(sf::Color(80, 80, 80)); }
        else { btn.rect.setFillColor(sf::Color(34, 139, 34)); }

        btn.rect.setOutlineThickness(4.0f);
        btn.rect.setOutlineColor(sf::Color(80, 40, 80));

        btn.text.setFont(pixelFont);
        if (btn.isLocked) { btn.text.setString(L"Уровень " + std::to_wstring(i + 1) + L" (Закрыт)"); }
        else { btn.text.setString(levelNames[i]); }

        btn.text.setCharacterSize(28);
        if (btn.isLocked) { btn.text.setFillColor(sf::Color(150, 150, 150)); }
        else { btn.text.setFillColor(sf::Color(80, 40, 80)); }

        btn.text.setOutlineThickness(2.0f);
        btn.text.setOutlineColor(sf::Color::White);
        sf::FloatRect tb = btn.text.getLocalBounds();
        btn.text.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
        btn.text.setPosition(startX + buttonWidth / 2.0f, startY + i * (buttonHeight + spacing) + buttonHeight / 2.0f);
        levelButtons.push_back(btn);
    }

    sf::RectangleShape backRect(sf::Vector2f(230.0f, 55.0f));
    backRect.setPosition((1920.0f - 230.0f) / 2.0f, 820.0f);
    backRect.setFillColor(sf::Color(34, 139, 34));
    backRect.setOutlineThickness(4.0f);
    backRect.setOutlineColor(sf::Color(80, 40, 80));

    sf::Text backText;
    backText.setFont(pixelFont);
    backText.setString(L"НАЗАД");
    backText.setCharacterSize(26);
    backText.setFillColor(sf::Color(80, 40, 80));
    backText.setOutlineThickness(2.0f);
    backText.setOutlineColor(sf::Color::White);
    sf::FloatRect btb = backText.getLocalBounds();
    backText.setOrigin(btb.left + btb.width / 2.0f, btb.top + btb.height / 2.0f);
    backText.setPosition((1920.0f - 230.0f) / 2.0f + 115.0f, 820.0f + 27.0f);

    int selectedLevel = -1; bool exitToTheme = false;

    while (window.isOpen() && !exitToTheme && selectedLevel == -1) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { window.close(); return; }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                if (showExitToThemeConfirmation()) exitToTheme = true;
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mPos = sf::Mouse::getPosition(window);
                sf::Vector2f mPosF = static_cast<sf::Vector2f>(mPos);
                for (int i = 0; i < 4; i++) {
                    if (levelButtons[i].rect.getGlobalBounds().contains(mPosF) && !levelButtons[i].isLocked) {
                        playClickSound();
                        selectedLevel = i; break;
                    }
                }
                if (backRect.getGlobalBounds().contains(mPosF)) {
                    if (showExitToThemeConfirmation()) exitToTheme = true;
                }
            }
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2f mPosF = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
                for (auto& btn : levelButtons) {
                    if (!btn.isLocked) {
                        if (btn.rect.getGlobalBounds().contains(mPosF)) btn.rect.setFillColor(sf::Color(50, 180, 50));
                        else btn.rect.setFillColor(sf::Color(34, 139, 34));
                    }
                }
                if (backRect.getGlobalBounds().contains(mPosF)) backRect.setFillColor(sf::Color(50, 180, 50));
                else backRect.setFillColor(sf::Color(34, 139, 34));
            }
        }
        window.clear();
        window.draw(backgroundSprite);
        drawPixelText(window, title);
        for (const auto& btn : levelButtons) { window.draw(btn.rect); drawPixelText(window, btn.text); }
        window.draw(backRect);
        drawPixelText(window, backText);
        window.display();
    }

    if (selectedLevel >= 0) {
        GameplayState game(window, &db, &matchBuffer, &errorBuffer, sfxVolume);
        game.setTheme(selectedTheme);
        game.run(selectedLevel + 1);
        currentProfile = db.loadProfile();
    }
}

void GameApp::runThemeAndLevelSelect() {
    sf::Sprite backgroundSprite;
    if (bgLevel.getSize().x > 0) {
        backgroundSprite.setTexture(bgLevel);
        backgroundSprite.setScale(1920.0f / bgLevel.getSize().x, 1080.0f / bgLevel.getSize().y);
    }
    CardSelectionState themeSelect(window, &bgLevel, &clickBuffer, sfxVolume);
    while (window.isOpen() && !themeSelect.isThemeSelected()) {
        themeSelect.handleEvents(); themeSelect.update();
        sf::Event event;
        while (window.pollEvent(event)) { if (event.type == sf::Event::Closed) { window.close(); return; } }
        window.clear();
        window.draw(backgroundSprite);
        themeSelect.draw();
        window.display();
    }
    if (!themeSelect.isThemeSelected() || themeSelect.getSelectedTheme() == "") return;
    runLevelSelect(themeSelect.getSelectedTheme());
}

void GameApp::runMainMenu() {
    window.setMouseCursorVisible(true);
    sf::Sprite backgroundSprite;
    if (bgMenu.getSize().x > 0) {
        backgroundSprite.setTexture(bgMenu);
        backgroundSprite.setScale(1920.0f / bgMenu.getSize().x, 1080.0f / bgMenu.getSize().y);
    }

    sf::Text title;
    title.setFont(pixelFont);
    title.setString(L"Мемори");
    title.setCharacterSize(150);
    title.setFillColor(sf::Color(237, 147, 0));
    title.setOutlineThickness(5);
    title.setOutlineColor(sf::Color::Black);
    centerText(title, 960.0f, 100.0f);

    struct MenuButton { sf::RectangleShape rect; sf::Text text; };
    std::vector<MenuButton> menuButtons;

    sf::String menuItems[] = {
        L"Одиночная игра",
        L"Играть (Дуо)",
        L"Настройки",
        L"О игре",
        L"Выход"
    };

    float buttonWidth = 500.0f, buttonHeight = 80.0f;
    // ИЗМЕНЕНО: кнопки опущены ниже (startY = 280 вместо 160)
    float startX = (1920.0f - buttonWidth) / 2.0f, startY = 280.0f, spacing = 30.0f;

    for (int i = 0; i < 5; i++) {
        MenuButton btn;
        btn.rect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        btn.rect.setPosition(startX, startY + i * (buttonHeight + spacing));
        btn.rect.setFillColor(sf::Color(34, 139, 34));
        btn.rect.setOutlineThickness(4.0f);
        btn.rect.setOutlineColor(sf::Color(80, 40, 80));

        btn.text.setFont(pixelFont);
        btn.text.setString(menuItems[i]);
        btn.text.setCharacterSize(38);
        btn.text.setFillColor(sf::Color(80, 40, 80));
        btn.text.setOutlineThickness(2.0f);
        btn.text.setOutlineColor(sf::Color::White);
        sf::FloatRect tb = btn.text.getLocalBounds();
        btn.text.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
        btn.text.setPosition(startX + buttonWidth / 2.0f, startY + i * (buttonHeight + spacing) + buttonHeight / 2.0f);
        menuButtons.push_back(btn);
    }

    sf::Sprite trophySprite;
    if (trophyTex.getSize().x > 0) {
        trophySprite.setTexture(trophyTex);
        trophySprite.setScale(0.2f, 0.2f);
        trophySprite.setPosition(1800.0f, 10.0f);
    }

    while (window.isOpen() && isRunning) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { if (showExitConfirmation()) { window.close(); return; } }
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mPosF = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
                if (trophyTex.getSize().x > 0 && trophySprite.getGlobalBounds().contains(mPosF)) {
                    playClickSound();
                    runBestResults();
                }

                for (int i = 0; i < 5; i++) {
                    if (menuButtons[i].rect.getGlobalBounds().contains(mPosF)) {
                        playClickSound();
                        switch (i) {
                        case 0: runThemeAndLevelSelect(); break;
                        case 1: runDuoMenu(); break;
                        case 2: runOptions(); break;
                        case 3: runAbout(); break;
                        case 4: if (showExitConfirmation()) { window.close(); return; } break;
                        }
                        break;
                    }
                }
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                if (showExitConfirmation()) { window.close(); return; }
            }
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2f mPosF = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
                for (auto& btn : menuButtons) {
                    if (btn.rect.getGlobalBounds().contains(mPosF)) btn.rect.setFillColor(sf::Color(50, 180, 50));
                    else btn.rect.setFillColor(sf::Color(34, 139, 34));
                }
            }
        }
        window.clear();
        window.draw(backgroundSprite);
        drawPixelText(window, title);
        for (const auto& btn : menuButtons) { window.draw(btn.rect); drawPixelText(window, btn.text); }
        if (trophyTex.getSize().x > 0) window.draw(trophySprite);
        window.display();
    }
}

void GameApp::runOptions() {
    sf::Sprite backgroundSprite;
    if (bgOptions.getSize().x > 0) {
        backgroundSprite.setTexture(bgOptions);
        backgroundSprite.setScale(1920.0f / bgOptions.getSize().x, 1080.0f / bgOptions.getSize().y);
    }

    sf::Text title;
    title.setFont(pixelFont);
    title.setString(L"Настройки");
    title.setCharacterSize(100);
    title.setFillColor(sf::Color(237, 147, 0));
    title.setOutlineThickness(3);
    title.setOutlineColor(sf::Color::Black);
    centerText(title, 960.0f, 150.0f);

    sf::RectangleShape closeBtn(sf::Vector2f(50.0f, 50.0f));
    closeBtn.setPosition(1850.0f, 20.0f);
    closeBtn.setFillColor(sf::Color(231, 76, 60));
    closeBtn.setOutlineThickness(3.0f);
    closeBtn.setOutlineColor(sf::Color(80, 40, 80));
    sf::Vertex line1[] = { sf::Vertex(sf::Vector2f(1862.0f, 32.0f), sf::Color::White), sf::Vertex(sf::Vector2f(1888.0f, 58.0f), sf::Color::White) };
    sf::Vertex line2[] = { sf::Vertex(sf::Vector2f(1888.0f, 32.0f), sf::Color::White), sf::Vertex(sf::Vector2f(1862.0f, 58.0f), sf::Color::White) };

    sf::RectangleShape sliderBg(sf::Vector2f(400.0f, 20.0f));
    sliderBg.setPosition(760.0f, 400.0f);
    sliderBg.setFillColor(sf::Color(80, 40, 80));
    sliderBg.setOutlineThickness(2.0f);
    sliderBg.setOutlineColor(sf::Color::White);

    sf::RectangleShape sliderFill, sliderHandle;
    sf::Text volText;
    volText.setFont(pixelFont);
    volText.setCharacterSize(40u);
    volText.setFillColor(sf::Color(237, 147, 0));
    volText.setOutlineThickness(2.0f);
    volText.setOutlineColor(sf::Color::Black);
    centerText(volText, 960.0f, 350.0f);

    sf::Text sfxTitle;
    sfxTitle.setFont(pixelFont);
    sfxTitle.setString(L"Громкость эффектов:");
    sfxTitle.setCharacterSize(32u);
    sfxTitle.setFillColor(sf::Color::White);
    sfxTitle.setPosition(760.0f, 480.0f);

    sf::RectangleShape sfxSliderBg(sf::Vector2f(400.0f, 20.0f));
    sfxSliderBg.setPosition(760.0f, 520.0f);
    sfxSliderBg.setFillColor(sf::Color(80, 40, 80));
    sfxSliderBg.setOutlineThickness(2.0f);
    sfxSliderBg.setOutlineColor(sf::Color::White);

    sf::RectangleShape sfxSliderFill, sfxSliderHandle;
    sf::Text sfxVolText;
    sfxVolText.setFont(pixelFont);
    sfxVolText.setCharacterSize(32u);
    sfxVolText.setFillColor(sf::Color(237, 147, 0));
    sfxVolText.setOutlineThickness(2.0f);
    sfxVolText.setOutlineColor(sf::Color::Black);
    centerText(sfxVolText, 960.0f, 470.0f);

    bool draggingSlider = false, sliderInitialized = false;
    bool draggingSfxSlider = false, sfxSliderInitialized = false;

    while (window.isOpen() && isRunning) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { window.close(); return; }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                playClickSound();
                return;
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mPos = sf::Mouse::getPosition(window);
                sf::Vector2f mPosF = static_cast<sf::Vector2f>(mPos);
                if (closeBtn.getGlobalBounds().contains(mPosF)) {
                    playClickSound();
                    return;
                }
                if (sliderBg.getGlobalBounds().contains(mPosF) || sliderHandle.getGlobalBounds().contains(mPosF)) {
                    playClickSound();
                    draggingSlider = true;
                    float newX = std::max(760.0f, std::min(mPosF.x - 10.0f, 1150.0f));
                    sliderHandle.setPosition(newX, 395.0f);
                    float percent = (newX - 760.0f) / 400.0f;
                    music.setVolume(percent * 100.0f);
                    volText.setString(L"Громкость: " + std::to_wstring(static_cast<int>(percent * 100.0f)) + L"%");
                    centerText(volText, 960.0f, 350.0f);
                    sliderFill.setSize(sf::Vector2f(newX - 760.0f + 10.0f, 20.0f));
                    sliderFill.setPosition(760.0f, 400.0f);
                    sliderFill.setFillColor(sf::Color(34, 139, 34));
                }
                if (sfxSliderBg.getGlobalBounds().contains(mPosF) || sfxSliderHandle.getGlobalBounds().contains(mPosF)) {
                    playClickSound();
                    draggingSfxSlider = true;
                    float newX = std::max(760.0f, std::min(mPosF.x - 10.0f, 1150.0f));
                    sfxSliderHandle.setPosition(newX, 515.0f);
                    float percent = (newX - 760.0f) / 400.0f;
                    sfxVolume = percent * 100.0f;
                    if (soundLoaded) clickSound.setVolume(sfxVolume);
                    sfxVolText.setString(L"Эффекты: " + std::to_wstring(static_cast<int>(sfxVolume)) + L"%");
                    centerText(sfxVolText, 960.0f, 470.0f);
                    sfxSliderFill.setSize(sf::Vector2f(newX - 760.0f + 10.0f, 20.0f));
                    sfxSliderFill.setPosition(760.0f, 520.0f);
                    sfxSliderFill.setFillColor(sf::Color(34, 139, 34));
                }
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                draggingSlider = false;
                draggingSfxSlider = false;
            }
            if (event.type == sf::Event::MouseMoved && draggingSlider) {
                sf::Vector2i mPos = sf::Mouse::getPosition(window);
                sf::Vector2f mPosF = static_cast<sf::Vector2f>(mPos);
                float newX = std::max(760.0f, std::min(mPosF.x - 10.0f, 1150.0f));
                sliderHandle.setPosition(newX, 395.0f);
                float percent = (newX - 760.0f) / 400.0f;
                music.setVolume(percent * 100.0f);
                volText.setString(L"Громкость: " + std::to_wstring(static_cast<int>(percent * 100.0f)) + L"%");
                centerText(volText, 960.0f, 350.0f);
                sliderFill.setSize(sf::Vector2f(newX - 760.0f + 10.0f, 20.0f));
                sliderFill.setPosition(760.0f, 400.0f);
                sliderFill.setFillColor(sf::Color(34, 139, 34));
            }
            if (event.type == sf::Event::MouseMoved && draggingSfxSlider) {
                sf::Vector2i mPos = sf::Mouse::getPosition(window);
                sf::Vector2f mPosF = static_cast<sf::Vector2f>(mPos);
                float newX = std::max(760.0f, std::min(mPosF.x - 10.0f, 1150.0f));
                sfxSliderHandle.setPosition(newX, 515.0f);
                float percent = (newX - 760.0f) / 400.0f;
                sfxVolume = percent * 100.0f;
                if (soundLoaded) clickSound.setVolume(sfxVolume);
                sfxVolText.setString(L"Эффекты: " + std::to_wstring(static_cast<int>(sfxVolume)) + L"%");
                centerText(sfxVolText, 960.0f, 470.0f);
                sfxSliderFill.setSize(sf::Vector2f(newX - 760.0f + 10.0f, 20.0f));
                sfxSliderFill.setPosition(760.0f, 520.0f);
                sfxSliderFill.setFillColor(sf::Color(34, 139, 34));
            }
        }
        if (!sliderInitialized) {
            float volume = music.getVolume();
            float handleX = 760.0f + (volume / 100.0f) * 400.0f - 10.0f;
            sliderHandle.setSize(sf::Vector2f(20.0f, 30.0f));
            sliderHandle.setPosition(handleX, 395.0f);
            sliderHandle.setFillColor(sf::Color(237, 147, 0));
            sliderHandle.setOutlineThickness(2.0f);
            sliderHandle.setOutlineColor(sf::Color::Black);
            sliderFill.setSize(sf::Vector2f((volume / 100.0f) * 400.0f, 20.0f));
            sliderFill.setPosition(760.0f, 400.0f);
            sliderFill.setFillColor(sf::Color(34, 139, 34));
            volText.setString(L"Громкость: " + std::to_wstring(static_cast<int>(volume)) + L"%");
            centerText(volText, 960.0f, 350.0f);
            sliderInitialized = true;
        }
        if (!sfxSliderInitialized) {
            float handleX = 760.0f + (sfxVolume / 100.0f) * 400.0f - 10.0f;
            sfxSliderHandle.setSize(sf::Vector2f(20.0f, 30.0f));
            sfxSliderHandle.setPosition(handleX, 515.0f);
            sfxSliderHandle.setFillColor(sf::Color(237, 147, 0));
            sfxSliderHandle.setOutlineThickness(2.0f);
            sfxSliderHandle.setOutlineColor(sf::Color::Black);
            sfxSliderFill.setSize(sf::Vector2f((sfxVolume / 100.0f) * 400.0f, 20.0f));
            sfxSliderFill.setPosition(760.0f, 520.0f);
            sfxSliderFill.setFillColor(sf::Color(34, 139, 34));
            sfxVolText.setString(L"Эффекты: " + std::to_wstring(static_cast<int>(sfxVolume)) + L"%");
            centerText(sfxVolText, 960.0f, 470.0f);
            sfxSliderInitialized = true;
        }
        window.clear();
        window.draw(backgroundSprite);
        window.draw(title);
        window.draw(volText);
        window.draw(sliderBg);
        window.draw(sliderFill);
        window.draw(sliderHandle);
        window.draw(sfxTitle);
        window.draw(sfxVolText);
        window.draw(sfxSliderBg);
        window.draw(sfxSliderFill);
        window.draw(sfxSliderHandle);
        window.draw(closeBtn);
        window.draw(line1, 2, sf::Lines);
        window.draw(line2, 2, sf::Lines);
        window.display();
    }
}

void GameApp::runAbout() {
    sf::Sprite backgroundSprite;
    if (bgAbout.getSize().x > 0) {
        backgroundSprite.setTexture(bgAbout);
        backgroundSprite.setScale(1920.0f / bgAbout.getSize().x, 1080.0f / bgAbout.getSize().y);
    }

    sf::Text rules;
    rules.setFont(pixelFont);
    rules.setString(L"Правила игры Мемори:\n\n1. Колоды карточек тщательно перемешиваются.\n2. Карточки раскладываются рубашкой вверх.\n3. Игрок открывает любые две карточки за ход.\n4. Если образовалась пара — очко +1 и ход продолжается.\n5. Если картинки разные — карточки закрываются.\n6. Игра заканчивается, когда все пары найдены.\n\nУдачи!");
    rules.setCharacterSize(35u);
    rules.setFillColor(sf::Color(80, 40, 80));
    rules.setOutlineThickness(2.0f);
    rules.setOutlineColor(sf::Color::White);
    centerText(rules, 960.0f, 450.0f);

    sf::RectangleShape closeBtn(sf::Vector2f(50.0f, 50.0f));
    closeBtn.setPosition(1850.0f, 20.0f);
    closeBtn.setFillColor(sf::Color(231, 76, 60));
    closeBtn.setOutlineThickness(3.0f);
    closeBtn.setOutlineColor(sf::Color(80, 40, 80));
    sf::Vertex line1[] = { sf::Vertex(sf::Vector2f(1862.0f, 32.0f), sf::Color::White), sf::Vertex(sf::Vector2f(1888.0f, 58.0f), sf::Color::White) };
    sf::Vertex line2[] = { sf::Vertex(sf::Vector2f(1888.0f, 32.0f), sf::Color::White), sf::Vertex(sf::Vector2f(1862.0f, 58.0f), sf::Color::White) };

    while (window.isOpen() && isRunning) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { window.close(); return; }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                playClickSound();
                return;
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                playClickSound();
                if (closeBtn.getGlobalBounds().contains(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)))) return;
            }
        }
        window.clear();
        window.draw(backgroundSprite);
        window.draw(rules);
        window.draw(closeBtn);
        window.draw(line1, 2, sf::Lines);
        window.draw(line2, 2, sf::Lines);
        window.display();
    }
}

void GameApp::run() {
    while (window.isOpen() && isRunning) { runMainMenu(); }
}

// =========================================================
// НОВАЯ ФУНКЦИЯ: Запуск режима Дуо
// =========================================================
void GameApp::runDuoMenu() {
    std::cout << "[GameApp] Запуск runDuoMenu()" << std::endl;
    DuoGame duoGame(window, &matchBuffer, &errorBuffer, sfxVolume, pixelFont);
    duoGame.run();
    std::cout << "[GameApp] Возврат из runDuoMenu()" << std::endl;
}
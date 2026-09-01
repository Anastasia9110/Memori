#include "DuoGame.h"
#include <iostream>
#include <algorithm>
#include <ctime>
#include <random>

DuoGame::DuoGame(sf::RenderWindow& win, sf::SoundBuffer* match, sf::SoundBuffer* error,
    float volume, const sf::Font& fontRef)
    : window(win), matchBuffer(match), errorBuffer(error),
    clickBuffer(nullptr), sfxVolume(volume),
    currentScreen(DuoScreen::SelectPlayers),
    isRunning(true), currentPlayerIndex(0), playerCount(2),
    selectedNameIndex(0), cursorTimer(0), errorTimer(0),
    totalPairs(17), firstCardIndex(-1), secondCardIndex(-1),
    isProcessing(false), processTimer(0) {

    srand(static_cast<unsigned>(time(nullptr)));

    // Загружаем Roboto как основной шрифт
    if (!font.loadFromFile("resources/Roboto/Roboto.ttf")) {
        std::cerr << "[ERROR] Roboto.ttf не найден!" << std::endl;
        font = fontRef;
    }

    // Pixelify для заголовков и кнопок
    if (!pixelFont.loadFromFile("resources/Pixelify_Sans/PixelifySans.ttf")) {
        pixelFont = font;
    }

    // Загрузка звуков
    clickBuffer = new sf::SoundBuffer();
    if (!clickBuffer->loadFromFile("resources/music/click.wav")) {
        std::cout << "[WARNING] Звук клика не найден" << std::endl;
    }
    else {
        clickSound.setBuffer(*clickBuffer);
        clickSound.setVolume(sfxVolume);
        std::cout << "[OK] Звук клика загружен" << std::endl;
    }

    if (matchBuffer && matchBuffer->loadFromFile("resources/music/match.wav")) {
        matchSound.setBuffer(*matchBuffer);
        matchSound.setVolume(sfxVolume);
        std::cout << "[OK] match.wav загружен" << std::endl;
    }
    else {
        std::cout << "[WARNING] match.wav не найден" << std::endl;
    }

    if (errorBuffer && errorBuffer->loadFromFile("resources/music/error.wav")) {
        errorSound.setBuffer(*errorBuffer);
        errorSound.setVolume(sfxVolume);
        std::cout << "[OK] error.wav загружен" << std::endl;
    }
    else {
        std::cout << "[WARNING] error.wav не найден" << std::endl;
    }

    if (!backgroundTexture.loadFromFile("img/menu1.jpg")) {
        std::cerr << "[WARNING] Не удалось загрузить фон" << std::endl;
    }
}

DuoGame::~DuoGame() {
    if (clickBuffer) {
        delete clickBuffer;
        clickBuffer = nullptr;
    }
    for (auto& card : cards) {
        if (card.texture) {
            delete card.texture;
            card.texture = nullptr;
        }
    }
}

void DuoGame::playClickSound() {
    if (clickBuffer) {
        clickSound.setPlayingOffset(sf::Time::Zero);
        clickSound.play();
    }
}

void DuoGame::playMatchSound() {
    if (matchBuffer) {
        matchSound.setPlayingOffset(sf::Time::Zero);
        matchSound.play();
    }
}

void DuoGame::playErrorSound() {
    if (errorBuffer) {
        errorSound.setPlayingOffset(sf::Time::Zero);
        errorSound.play();
    }
}

void DuoGame::centerText(sf::Text& text, float x, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    text.setPosition(x, y);
}

bool DuoGame::checkClick(const sf::RectangleShape& shape, const sf::Vector2f& pos) const {
    return shape.getGlobalBounds().contains(pos);
}

sf::Vector2f DuoGame::getMousePos() const {
    return window.mapPixelToCoords(sf::Mouse::getPosition(window));
}

void DuoGame::run() {
    std::cout << "[DuoGame] Запуск..." << std::endl;
    currentScreen = DuoScreen::SelectPlayers;
    initSelectPlayers();

    while (isRunning && window.isOpen()) {
        handleEvents();
        update();
        draw();
    }
    std::cout << "[DuoGame] Завершён" << std::endl;
}

void DuoGame::update() {
    if (currentScreen == DuoScreen::EnterNames) {
        cursorTimer += 0.016f;
        if (cursorTimer > 0.5f) cursorTimer = 0;

        if (errorTimer > 0) {
            errorTimer -= 0.016f;
            if (errorTimer <= 0) errorMessage.clear();
        }
    }

    if (currentScreen == DuoScreen::Playing && isProcessing) {
        processTimer += 0.016f;
        if (processTimer > 25.0f) {  
            isProcessing = false;
            processTimer = 0;
            resetFlippedCards();
        }
    }
}

void DuoGame::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            showExitConfirmation();
        }

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            if (currentScreen == DuoScreen::SelectPlayers ||
                currentScreen == DuoScreen::EnterNames ||
                currentScreen == DuoScreen::SelectTheme) {
                currentScreen = DuoScreen::SelectPlayers;
                initSelectPlayers();
            }
            else {
                isRunning = false;
            }
        }

        if (event.type == sf::Event::TextEntered && currentScreen == DuoScreen::EnterNames) {
            sf::Uint32 unicode = event.text.unicode;

            if (unicode == '\b') { 
                if (!currentInputName.empty()) currentInputName.pop_back();
            }
            else if (unicode == 13) { 
                if (!currentInputName.empty()) {
                    if (!isNameUnique(currentInputName)) {
                        errorMessage = L"Имя уже занято!"; 
                        errorTimer = 15.0f;  
                        currentInputName.clear();
                    }
                    else {
                        Player p;
                        p.name = currentInputName;
                        players.push_back(p);
                        usedNames.insert(currentInputName);
                        selectedNameIndex++;
                        currentInputName.clear();
                        if (selectedNameIndex >= playerCount) {
                            currentScreen = DuoScreen::SelectTheme;
                            initSelectTheme();
                        }
                    }
                }
            }
            else if (unicode >= 32 && unicode != 127) { 
                if (currentInputName.length() < 15) {
                    currentInputName += static_cast<wchar_t>(unicode);
                }
            }
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = getMousePos();

            if (currentScreen == DuoScreen::SelectPlayers) {
                for (size_t i = 0; i < playerCountButtons.size(); i++) {
                    if (checkClick(playerCountButtons[i].rect, mousePos)) {
                        playClickSound();
                        playerCount = playerCountButtons[i].value;
                        players.clear();
                        usedNames.clear();
                        currentScreen = DuoScreen::EnterNames;
                        initEnterNames();
                        return;
                    }
                }
                if (checkClick(backButton, mousePos)) {
                    playClickSound();
                    showExitConfirmation();
                    return;
                }
            }
            else if (currentScreen == DuoScreen::EnterNames) {
                if (checkClick(backButton, mousePos)) {
                    playClickSound();
                    currentScreen = DuoScreen::SelectPlayers;
                    initSelectPlayers();
                }
            }
            else if (currentScreen == DuoScreen::SelectTheme) {
                for (size_t i = 0; i < themeButtons.size(); i++) {
                    if (checkClick(themeButtons[i].rect, mousePos)) {
                        playClickSound();
                        selectedTheme = themeButtons[i].themeId;
                        selectedThemePath = themeButtons[i].themePath;
                        currentScreen = DuoScreen::SelectFirstPlayer;
                        initSelectFirstPlayer();
                        return;
                    }
                }
                if (checkClick(backButton, mousePos)) {
                    playClickSound();
                    currentScreen = DuoScreen::SelectPlayers;
                    initSelectPlayers();
                }
            }
            else if (currentScreen == DuoScreen::Playing) {
                if (checkClick(closeBtn, mousePos)) {
                    playClickSound();
                    showExitConfirmation();
                    return;
                }

                if (!isProcessing) {
                    for (int i = 0; i < static_cast<int>(cards.size()); i++) {
                        if (cards[i].shape.getGlobalBounds().contains(mousePos) &&
                            !cards[i].isFlipped && !cards[i].isMatched) {

                            playClickSound();

                            if (firstCardIndex == -1) {
                                firstCardIndex = i;
                                cards[i].isFlipped = true;
                            }
                            else if (secondCardIndex == -1 && i != firstCardIndex) {
                                secondCardIndex = i;
                                cards[i].isFlipped = true;

                                if (cards[firstCardIndex].id == cards[secondCardIndex].id) {
                                    // ПРАВИЛЬНАЯ ПАРА - ход остаётся
                                    cards[firstCardIndex].isMatched = true;
                                    cards[secondCardIndex].isMatched = true;
                                    players[currentPlayerIndex].pairsFound++;
                                    playMatchSound();
                                    firstCardIndex = -1;
                                    secondCardIndex = -1;
                                }
                                else {
                                    // НЕПРАВИЛЬНАЯ ПАРА - ход переходит
                                    players[currentPlayerIndex].mistakes++;
                                    playErrorSound();
                                    isProcessing = true;
                                    processTimer = 0;
                                    currentPlayerIndex = (currentPlayerIndex + 1) % playerCount;
                                }
                                return;
                            }
                            break;
                        }
                    }
                }
            }
            else if (currentScreen == DuoScreen::Results) {
                if (checkClick(resultsNextBtn, mousePos)) {
                    playClickSound();
                    isRunning = false;
                }
            }
        }
    }
}

bool DuoGame::isNameUnique(const std::wstring& name) const {
    return usedNames.find(name) == usedNames.end();
}

void DuoGame::showExitConfirmation() {
    sf::RenderWindow confirmWin(sf::VideoMode(500, 200), L"Подтверждение", sf::Style::Titlebar | sf::Style::Close);
    confirmWin.setPosition(sf::Vector2i(710, 440));

    sf::Text question;
    question.setFont(font);
    question.setString(L"Вы действительно хотите выйти?");
    question.setCharacterSize(28);
    question.setFillColor(sf::Color(237, 147, 0));
    question.setOutlineThickness(2);
    question.setOutlineColor(sf::Color::Black);
    centerText(question, 250, 60);

    sf::RectangleShape btnYes(sf::Vector2f(120, 45));
    btnYes.setPosition(80, 120);
    btnYes.setFillColor(sf::Color(34, 139, 34));
    btnYes.setOutlineThickness(3);
    btnYes.setOutlineColor(sf::Color(80, 40, 80));

    sf::Text txtYes;
    txtYes.setFont(font);
    txtYes.setString(L"ДА");
    txtYes.setCharacterSize(22);
    txtYes.setFillColor(sf::Color::White);
    centerText(txtYes, 140, 135);

    sf::RectangleShape btnNo(sf::Vector2f(120, 45));
    btnNo.setPosition(300, 120);
    btnNo.setFillColor(sf::Color(34, 139, 34));
    btnNo.setOutlineThickness(3);
    btnNo.setOutlineColor(sf::Color(80, 40, 80));

    sf::Text txtNo;
    txtNo.setFont(font);
    txtNo.setString(L"НЕТ");
    txtNo.setCharacterSize(22);
    txtNo.setFillColor(sf::Color::White);
    centerText(txtNo, 360, 135);

    while (confirmWin.isOpen()) {
        sf::Event ce;
        while (confirmWin.pollEvent(ce)) {
            if (ce.type == sf::Event::Closed) confirmWin.close();
            if (ce.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mp = sf::Mouse::getPosition(confirmWin);
                sf::Vector2f pos = confirmWin.mapPixelToCoords(mp);
                if (checkClick(btnYes, pos)) {
                    isRunning = false;
                    confirmWin.close();
                }
                if (checkClick(btnNo, pos)) {
                    confirmWin.close();
                }
            }
        }
        sf::Vector2i mPos = sf::Mouse::getPosition(confirmWin);
        sf::Vector2f pos = confirmWin.mapPixelToCoords(mPos);
        btnYes.setFillColor(checkClick(btnYes, pos) ? sf::Color(50, 180, 50) : sf::Color(34, 139, 34));
        btnNo.setFillColor(checkClick(btnNo, pos) ? sf::Color(50, 180, 50) : sf::Color(34, 139, 34));

        confirmWin.clear(sf::Color(30, 30, 50));
        confirmWin.draw(question);
        confirmWin.draw(btnYes); confirmWin.draw(txtYes);
        confirmWin.draw(btnNo); confirmWin.draw(txtNo);
        confirmWin.display();
    }
}

void DuoGame::initSelectPlayers() {
    playerCountButtons.clear();
    sf::String labels[] = { L"2 ИГРОКА", L"3 ИГРОКА", L"4 ИГРОКА" };
    int values[] = { 2, 3, 4 };
    float buttonWidth = 500.0f, buttonHeight = 90.0f;
    float startX = (1920.0f - buttonWidth) / 2.0f;
    float startY = 450.0f;
    float spacing = 40.0f;

    for (int i = 0; i < 3; i++) {
        PlayerCountButton btn;
        btn.value = values[i];
        btn.rect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        btn.rect.setPosition(startX, startY + i * (buttonHeight + spacing));
        btn.rect.setFillColor(sf::Color(34, 139, 34));
        btn.rect.setOutlineThickness(4.0f);
        btn.rect.setOutlineColor(sf::Color(80, 40, 80));

        btn.text.setFont(pixelFont); // ИСПОЛЬЗУЕМ ПИКСЕЛЬНЫЙ ШРИФТ
        btn.text.setString(labels[i]);
        btn.text.setCharacterSize(40);
        btn.text.setFillColor(sf::Color(80, 40, 80));
        btn.text.setOutlineThickness(2.0f);
        btn.text.setOutlineColor(sf::Color::White);
        centerText(btn.text, startX + buttonWidth / 2.0f, startY + i * (buttonHeight + spacing) + buttonHeight / 2.0f);
        playerCountButtons.push_back(btn);
    }

    backButton.setSize(sf::Vector2f(200.0f, 50.0f));
    backButton.setPosition((1920.0f - 200.0f) / 2.0f, 950.0f);
    backButton.setFillColor(sf::Color(231, 76, 60));
    backButton.setOutlineThickness(3.0f);
    backButton.setOutlineColor(sf::Color(80, 40, 80));

    backButtonText.setFont(pixelFont); //ПИКСЕЛЬНЫЙ ШРИФТ
    backButtonText.setString(L"НАЗАД");
    backButtonText.setCharacterSize(24);
    backButtonText.setFillColor(sf::Color::White);
    centerText(backButtonText, (1920.0f - 200.0f) / 2.0f + 100.0f, 975.0f);
}

void DuoGame::updateSelectPlayers() {}

void DuoGame::drawSelectPlayers() {
    window.clear(sf::Color(20, 30, 50));

    sf::Sprite bgSprite;
    if (backgroundTexture.getSize().x > 0) {
        bgSprite.setTexture(backgroundTexture);
        bgSprite.setScale(1920.0f / backgroundTexture.getSize().x, 1080.0f / backgroundTexture.getSize().y);
        window.draw(bgSprite);
    }

    sf::Text title;
    title.setFont(pixelFont);
    title.setString(L"РЕЖИМ ДУО\nСколько игроков?");
    title.setCharacterSize(70);
    title.setFillColor(sf::Color(237, 147, 0));
    title.setOutlineThickness(4);
    title.setOutlineColor(sf::Color::Black);
    centerText(title, 960.0f, 250.0f);
    window.draw(title);

    sf::Vector2f mousePos = getMousePos();

    for (size_t i = 0; i < playerCountButtons.size(); i++) {
        if (checkClick(playerCountButtons[i].rect, mousePos)) {
            playerCountButtons[i].rect.setFillColor(sf::Color(50, 180, 50));
        }
        else {
            playerCountButtons[i].rect.setFillColor(sf::Color(34, 139, 34));
        }
        window.draw(playerCountButtons[i].rect);
        window.draw(playerCountButtons[i].text);
    }

    if (checkClick(backButton, mousePos)) {
        backButton.setFillColor(sf::Color(200, 50, 50));
    }
    else {
        backButton.setFillColor(sf::Color(231, 76, 60));
    }
    window.draw(backButton);
    window.draw(backButtonText);

    window.display();
}

void DuoGame::initEnterNames() {
    players.clear();
    usedNames.clear();
    selectedNameIndex = 0;
    currentInputName.clear();
    cursorTimer = 0;
    errorMessage.clear();
    errorTimer = 0;
}

void DuoGame::updateEnterNames() {}

void DuoGame::drawEnterNames() {
    window.clear(sf::Color(20, 25, 40));

    sf::Text title;
    title.setFont(pixelFont);
    title.setString(L"ВВЕДИТЕ ИМЕНА ИГРОКОВ");
    title.setCharacterSize(60);
    title.setFillColor(sf::Color(237, 147, 0));
    title.setOutlineThickness(3);
    title.setOutlineColor(sf::Color::Black);
    centerText(title, 960.0f, 120.0f);
    window.draw(title);

    sf::Text hintEnter;
    hintEnter.setFont(font);
    hintEnter.setString(L"Введите имя (буквы/цифры) и нажмите Enter");
    hintEnter.setCharacterSize(24);
    hintEnter.setFillColor(sf::Color(200, 200, 200));
    centerText(hintEnter, 960.0f, 200.0f);
    window.draw(hintEnter);

    float startY = 300.0f;
    for (int i = 0; i < playerCount; i++) {
        sf::Text playerText;
        playerText.setFont(font);  // Roboto шрифт
        playerText.setCharacterSize(45);

        if (i < selectedNameIndex) {
            // players[i].name теперь уже wstring, преобразования не нужны!
            playerText.setString(L"Игрок " + std::to_wstring(i + 1) + L": " + players[i].name);
            playerText.setFillColor(sf::Color(100, 255, 100));
        }
        else if (i == selectedNameIndex) {
            std::wstring displayName = currentInputName;
            if (static_cast<int>(cursorTimer * 2) % 2 == 0) displayName += L"|";
            playerText.setString(L"Игрок " + std::to_wstring(i + 1) + L": " + displayName);
            playerText.setFillColor(sf::Color(255, 255, 255));
        }
        else {
            playerText.setString(L"Игрок " + std::to_wstring(i + 1) + L": ___");
            playerText.setFillColor(sf::Color(150, 150, 150));
        }
        playerText.setPosition(200.0f, startY + i * 90.0f);
        window.draw(playerText);
    }

    sf::RectangleShape inputBox(sf::Vector2f(500.0f, 60.0f));
    inputBox.setPosition(200.0f, startY + selectedNameIndex * 90.0f + 5.0f);
    inputBox.setFillColor(sf::Color(0, 0, 0, 100));
    inputBox.setOutlineThickness(2.0f);
    inputBox.setOutlineColor(sf::Color(237, 147, 0));
    window.draw(inputBox);

    // Сообщение об ошибке (теперь корректно работает с кириллицей)
    if (!errorMessage.isEmpty()) {
        sf::Text errorText;
        errorText.setFont(font);  // Roboto
        errorText.setCharacterSize(36);
        errorText.setFillColor(sf::Color(231, 76, 60));
        errorText.setOutlineThickness(2);
        errorText.setOutlineColor(sf::Color::Black);
        errorText.setString(errorMessage); // errorMessage - это sf::String
        centerText(errorText, 960.0f, startY + playerCount * 90.0f + 80.0f);
        window.draw(errorText);
    }

    sf::Vector2f mousePos = getMousePos();
    if (checkClick(backButton, mousePos)) {
        backButton.setFillColor(sf::Color(200, 50, 50));
    }
    else {
        backButton.setFillColor(sf::Color(231, 76, 60));
    }
    window.draw(backButton);
    window.draw(backButtonText);

    window.display();
}

void DuoGame::initSelectTheme() {
    themeButtons.clear();
    selectedTheme = "";
    selectedThemePath = "";

    sf::String labels[] = { L"Животные", L"Ягоды", L"Мультфильмы", L"Машины" };
    std::string paths[] = { "img/animal/", "img/Berry/", "img/cartoons/", "img/cars/" };

    float buttonWidth = 500.0f, buttonHeight = 80.0f;
    float startX = (1920.0f - buttonWidth) / 2.0f;
    float startY = 280.0f;
    float spacing = 30.0f;

    for (int i = 0; i < 4; i++) {
        ThemeOption btn;
        btn.themeId = labels[i].toAnsiString();
        btn.themePath = paths[i];
        btn.rect.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        btn.rect.setPosition(startX, startY + i * (buttonHeight + spacing));
        btn.rect.setFillColor(sf::Color(34, 139, 34));
        btn.rect.setOutlineThickness(4.0f);
        btn.rect.setOutlineColor(sf::Color(80, 40, 80));

        btn.text.setFont(pixelFont); // ИСПОЛЬЗУЕМ ПИКСЕЛЬНЫЙ ШРИФТ
        btn.text.setString(labels[i]);
        btn.text.setCharacterSize(32);
        btn.text.setFillColor(sf::Color(80, 40, 80));
        btn.text.setOutlineThickness(2.0f);
        btn.text.setOutlineColor(sf::Color::White);
        centerText(btn.text, startX + buttonWidth / 2.0f, startY + i * (buttonHeight + spacing) + buttonHeight / 2.0f);
        themeButtons.push_back(btn);
    }

    backButton.setSize(sf::Vector2f(200.0f, 50.0f));
    backButton.setPosition((1920.0f - 200.0f) / 2.0f, 950.0f);
    backButton.setFillColor(sf::Color(231, 76, 60));
    backButton.setOutlineThickness(3.0f);
    backButton.setOutlineColor(sf::Color(80, 40, 80));

    backButtonText.setFont(pixelFont); // ИСПОЛЬЗУЕМ ПИКСЕЛЬНЫЙ ШРИФТ
    backButtonText.setString(L"НАЗАД В МЕНЮ");
    backButtonText.setCharacterSize(24);
    backButtonText.setFillColor(sf::Color::White);
    centerText(backButtonText, (1920.0f - 200.0f) / 2.0f + 100.0f, 975.0f);
}

void DuoGame::updateSelectTheme() {}

void DuoGame::drawSelectTheme() {
    window.clear(sf::Color(20, 30, 50));

    sf::Sprite bgSprite;
    if (backgroundTexture.getSize().x > 0) {
        bgSprite.setTexture(backgroundTexture);
        bgSprite.setScale(1920.0f / backgroundTexture.getSize().x, 1080.0f / backgroundTexture.getSize().y);
        window.draw(bgSprite);
    }

    sf::Text title;
    title.setFont(pixelFont);
    title.setString(L"Выберите набор карточек");
    title.setCharacterSize(50);
    title.setFillColor(sf::Color(237, 147, 0));
    title.setOutlineThickness(3);
    title.setOutlineColor(sf::Color::Black);
    centerText(title, 960.0f, 100.0f);
    window.draw(title);

    sf::Vector2f mousePos = getMousePos();

    for (size_t i = 0; i < themeButtons.size(); i++) {
        if (checkClick(themeButtons[i].rect, mousePos)) {
            themeButtons[i].rect.setFillColor(sf::Color(50, 180, 50));
        }
        else {
            themeButtons[i].rect.setFillColor(sf::Color(34, 139, 34));
        }
        window.draw(themeButtons[i].rect);
        window.draw(themeButtons[i].text);
    }

    if (checkClick(backButton, mousePos)) {
        backButton.setFillColor(sf::Color(200, 50, 50));
    }
    else {
        backButton.setFillColor(sf::Color(231, 76, 60));
    }
    window.draw(backButton);
    window.draw(backButtonText);

    window.display();
}

void DuoGame::initSelectFirstPlayer() {
    currentPlayerIndex = rand() % playerCount;
    currentScreen = DuoScreen::Playing;
    initPlaying();
}

void DuoGame::updateSelectFirstPlayer() {}

void DuoGame::drawSelectFirstPlayer() {
    window.clear(sf::Color(20, 30, 50));
    sf::Text text;
    text.setFont(font);  // Roboto
    text.setString(L"Первым ходит: " + players[currentPlayerIndex].name);
    text.setCharacterSize(50);
    text.setFillColor(sf::Color(237, 147, 0));
    text.setOutlineThickness(3);
    text.setOutlineColor(sf::Color::Black);
    centerText(text, 960.0f, 540.0f);
    window.draw(text);
    window.display();
    sf::sleep(sf::seconds(2));
}

void DuoGame::createCards(int pairs, const std::string& themePath) {
    cards.clear();

    std::vector<int> values;
    for (int i = 0; i < pairs; i++) {
        values.push_back(i);
        values.push_back(i);
    }
    std::shuffle(values.begin(), values.end(), std::default_random_engine(static_cast<unsigned int>(time(nullptr))));

    float cardSize = 130.0f;
    float padding = 20.0f;
    int cols = 6;
    float totalWidth = cols * cardSize + (cols - 1) * padding;
    float startX = (1920.0f - totalWidth) / 2.0f;
    float startY = 150.0f;

    for (int i = 0; i < pairs * 2; i++) {
        DuoCard c;
        c.id = values[i];
        c.isFlipped = false;
        c.isMatched = false;

        std::string filePath = themePath + std::to_string(values[i] + 1) + ".jpg";
        c.texture = new sf::Texture();
        if (c.texture->loadFromFile(filePath)) {
            c.sprite.setTexture(*c.texture);
            float sx = cardSize / static_cast<float>(c.texture->getSize().x);
            float sy = cardSize / static_cast<float>(c.texture->getSize().y);
            c.sprite.setScale(sx, sy);
        }

        c.shape.setSize(sf::Vector2f(cardSize, cardSize));
        c.shape.setFillColor(sf::Color(60, 60, 80));
        c.shape.setOutlineThickness(3.0f);
        c.shape.setOutlineColor(sf::Color(237, 147, 0));

        int row = i / cols;
        int col = i % cols;
        c.shape.setPosition(startX + col * (cardSize + padding), startY + row * (cardSize + padding));
        c.sprite.setPosition(startX + col * (cardSize + padding), startY + row * (cardSize + padding));

        cards.push_back(c);
    }
}

void DuoGame::shuffleCards() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(cards.begin(), cards.end(), g);
}

void DuoGame::flipCard(int index) {
    if (index < 0 || index >= static_cast<int>(cards.size())) return;
    if (cards[index].isFlipped || cards[index].isMatched) return;
    cards[index].isFlipped = true;
}

void DuoGame::resetFlippedCards() {
    for (size_t i = 0; i < cards.size(); i++) {
        if (!cards[i].isMatched) {
            cards[i].isFlipped = false;
        }
    }
    firstCardIndex = -1;
    secondCardIndex = -1;
}

void DuoGame::initPlaying() {
    createCards(totalPairs, selectedThemePath);
    firstCardIndex = -1;
    secondCardIndex = -1;
    isProcessing = false;
    processTimer = 0;

    closeBtn.setSize(sf::Vector2f(50.0f, 50.0f));
    closeBtn.setPosition(1850.0f, 20.0f);
    closeBtn.setFillColor(sf::Color(231, 76, 60));
    closeBtn.setOutlineThickness(3.0f);
    closeBtn.setOutlineColor(sf::Color(80, 40, 80));
}

void DuoGame::updatePlaying() {}

void DuoGame::drawPlaying() {
    window.clear(sf::Color(20, 25, 40));

    sf::Text gameTitle;
    gameTitle.setFont(pixelFont);
    gameTitle.setString(L"РЕЖИМ ДУО");
    gameTitle.setCharacterSize(35);
    gameTitle.setFillColor(sf::Color(237, 147, 0));
    gameTitle.setOutlineThickness(3);
    gameTitle.setOutlineColor(sf::Color::Black);
    centerText(gameTitle, 960.0f, 35.0f);
    window.draw(gameTitle);

    // Ход игрока
    currentPlayerText.setFont(font);
    currentPlayerText.setString(L"ХОД: " + players[currentPlayerIndex].name);
    currentPlayerText.setCharacterSize(35);
    currentPlayerText.setFillColor(sf::Color(255, 215, 0));
    currentPlayerText.setOutlineThickness(2);
    currentPlayerText.setOutlineColor(sf::Color(0, 0, 0));
    currentPlayerText.setPosition(20.0f, 80.0f);

    scoreText.setFont(font);
    scoreText.setCharacterSize(22);
    scoreText.setFillColor(sf::Color::White);
    std::wstring scoreStr = L"СЧЁТ:\n";
    for (int i = 0; i < playerCount; i++) {
        scoreStr += players[i].name;
        scoreStr += L": " + std::to_wstring(players[i].pairsFound) + L" пар";
        if (i == currentPlayerIndex) scoreStr += L" <---";
        scoreStr += L"\n";
    }
    scoreText.setString(scoreStr);
    scoreText.setPosition(20.0f, 125.0f);

    for (size_t i = 0; i < cards.size(); i++) {
        if (cards[i].isFlipped || cards[i].isMatched) {
            window.draw(cards[i].sprite);
        }
        else {
            window.draw(cards[i].shape);
        }
    }

    window.draw(closeBtn);

    sf::Vertex line1[] = {
        sf::Vertex(sf::Vector2f(1862, 32), sf::Color::White),
        sf::Vertex(sf::Vector2f(1888, 58), sf::Color::White)
    };
    sf::Vertex line2[] = {
        sf::Vertex(sf::Vector2f(1888, 32), sf::Color::White),
        sf::Vertex(sf::Vector2f(1862, 58), sf::Color::White)
    };
    window.draw(line1, 2, sf::Lines);
    window.draw(line2, 2, sf::Lines);

    window.draw(currentPlayerText);
    window.draw(scoreText);
    window.display();

    bool allMatched = true;
    for (size_t i = 0; i < cards.size(); i++) {
        if (!cards[i].isMatched) { allMatched = false; break; }
    }
    if (allMatched && !isProcessing) {
        currentScreen = DuoScreen::Results;
        initResults();
    }
}

void DuoGame::initResults() {
    std::sort(players.begin(), players.end(),
        [](const Player& a, const Player& b) {
            return a.pairsFound > b.pairsFound;
        });
    resultsNextBtn.setSize(sf::Vector2f(400.0f, 70.0f));
    resultsNextBtn.setPosition((1920.0f - 400.0f) / 2.0f, 850.0f);
    resultsNextBtn.setFillColor(sf::Color(34, 139, 34));
    resultsNextBtn.setOutlineThickness(4.0f);
    resultsNextBtn.setOutlineColor(sf::Color(80, 40, 80));

    resultsNextText.setFont(pixelFont); // ИСПОЛЬЗУЕМ ПИКСЕЛЬНЫЙ ШРИФТ
    resultsNextText.setString(L"В ГЛАВНОЕ МЕНЮ");
    resultsNextText.setCharacterSize(30);
    resultsNextText.setFillColor(sf::Color(80, 40, 80));
    resultsNextText.setOutlineThickness(2.0f);
    resultsNextText.setOutlineColor(sf::Color::White);
    centerText(resultsNextText, 960.0f, 885.0f);
}

void DuoGame::updateResults() {}

void DuoGame::drawResults() {
    window.clear(sf::Color(20, 30, 50));
    sf::Text title;
    title.setFont(pixelFont);
    title.setString(L"ИГРА ОКОНЧЕНА!");
    title.setCharacterSize(70);
    title.setFillColor(sf::Color(237, 147, 0));
    title.setOutlineThickness(4);
    title.setOutlineColor(sf::Color::Black);
    centerText(title, 960.0f, 80.0f);
    window.draw(title);

    sf::Text winner;
    winner.setFont(font);  // Roboto
    winner.setString(L"ПОБЕДИЛ: " + players[0].name);
    winner.setCharacterSize(50);
    winner.setFillColor(sf::Color(255, 215, 0));
    winner.setOutlineThickness(3);
    winner.setOutlineColor(sf::Color::Black);
    centerText(winner, 960.0f, 180.0f);
    window.draw(winner);

    float tableY = 300.0f;
    for (int i = 0; i < playerCount; i++) {
        sf::Text rowText;
        rowText.setFont(font);  // Roboto
        rowText.setCharacterSize(32);
        rowText.setFillColor(sf::Color::White);
        std::wstring place;
        if (i == 0) place = L"1 место: ";
        else if (i == 1) place = L"2 место: ";
        else if (i == 2) place = L"3 место: ";
        else place = std::to_wstring(i + 1) + L" место: ";

        rowText.setString(place + players[i].name +
            L"  |  Пар: " + std::to_wstring(players[i].pairsFound) +
            L"  |  Ошибок: " + std::to_wstring(players[i].mistakes));
        centerText(rowText, 960.0f, tableY + i * 70.0f);
        window.draw(rowText);
    }

    sf::Vector2f mousePos = getMousePos();
    if (checkClick(resultsNextBtn, mousePos)) {
        resultsNextBtn.setFillColor(sf::Color(50, 180, 50));
    }
    else {
        resultsNextBtn.setFillColor(sf::Color(34, 139, 34));
    }
    window.draw(resultsNextBtn);
    window.draw(resultsNextText);
    window.display();
}

void DuoGame::draw() {
    switch (currentScreen) {
    case DuoScreen::SelectPlayers:
        drawSelectPlayers();
        break;
    case DuoScreen::EnterNames:
        drawEnterNames();
        break;
    case DuoScreen::SelectTheme:
        drawSelectTheme();
        break;
    case DuoScreen::SelectFirstPlayer:
        drawSelectFirstPlayer();
        break;
    case DuoScreen::Playing:
        drawPlaying();
        break;
    case DuoScreen::Results:
        drawResults();
        break;
    }
}
#include "GameplayState.h"
#include <algorithm>

GameplayState::GameplayState(sf::RenderWindow& win, Database* db, sf::SoundBuffer* mBuf, sf::SoundBuffer* eBuf, float volume)
    : window(win), dbPtr(db), matchBuffer(mBuf), errorBuffer(eBuf), sfxVolume(volume),
    firstCard(nullptr), secondCard(nullptr), currentTheme("animal"),
    gameState(PLAYING), flipTimer(0.0f), score(0), moves(0), mistakes(0), pairsCount(8),
    gameTime(0.0f), timeLimit(0.0f), timeLimitEnabled(false),
    maxMoves(0), movesLimitEnabled(false), pairsNotFound(0) {

    if (!font.loadFromFile("resources/Roboto/Roboto.ttf")) {
        std::cerr << "Font not loaded!" << std::endl;
    }

    background.setFillColor(sf::Color(30, 30, 50));
    background.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 178));
    overlay.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));

    scoreText.setFont(font); scoreText.setCharacterSize(32u); scoreText.setFillColor(sf::Color::White); scoreText.setPosition(20.0f, 20.0f);
    movesText.setFont(font); movesText.setCharacterSize(32u); movesText.setFillColor(sf::Color::White); movesText.setPosition(20.0f, 60.0f);
    mistakesText.setFont(font); mistakesText.setCharacterSize(32u); mistakesText.setFillColor(sf::Color(231, 76, 60)); mistakesText.setPosition(20.0f, 100.0f);
    timerText.setFont(font); timerText.setCharacterSize(32u); timerText.setFillColor(sf::Color(255, 215, 0)); timerText.setPosition(20.0f, 140.0f);

    gameOverText.setFont(font); gameOverText.setCharacterSize(70u); gameOverText.setFillColor(sf::Color(46, 204, 113)); gameOverText.setString(L"ПОБЕДА!");
    timeOverText.setFont(font); timeOverText.setCharacterSize(70u); timeOverText.setFillColor(sf::Color(231, 76, 60)); timeOverText.setString(L"ВРЕМЯ ВЫШЛО!");

    finalScoreText.setFont(font); finalScoreText.setCharacterSize(32u); finalScoreText.setFillColor(sf::Color::White);
    finalMovesText.setFont(font); finalMovesText.setCharacterSize(32u); finalMovesText.setFillColor(sf::Color(241, 196, 15));

    closeBtn.setSize(sf::Vector2f(50.0f, 50.0f)); closeBtn.setPosition(1850.0f, 20.0f);
    closeBtn.setFillColor(sf::Color(231, 76, 60)); closeBtn.setOutlineThickness(2.0f); closeBtn.setOutlineColor(sf::Color::White);

    btnMenu.setSize(sf::Vector2f(250.0f, 70.0f)); btnMenu.setFillColor(sf::Color(231, 76, 60)); btnMenu.setOutlineThickness(3.0f); btnMenu.setOutlineColor(sf::Color::White);
    btnRetry.setSize(sf::Vector2f(250.0f, 70.0f)); btnRetry.setFillColor(sf::Color(46, 204, 113)); btnRetry.setOutlineThickness(3.0f); btnRetry.setOutlineColor(sf::Color::White);

    // ПРИВЯЗКА ЗВУКОВ К БУФЕРАМ
    if (matchBuffer) matchSound.setBuffer(*matchBuffer);
    if (errorBuffer) errorSound.setBuffer(*errorBuffer);
}

bool GameplayState::showGameExitConfirmation() {
    sf::RenderWindow confirmWin(sf::VideoMode(500, 250), L"Пауза", sf::Style::Titlebar | sf::Style::Close);
    confirmWin.setPosition(sf::Vector2i(710, 415));
    confirmWin.setMouseCursorVisible(true);

    sf::Font localFont;
    if (!localFont.loadFromFile("resources/Roboto/Roboto.ttf")) return false;

    sf::Text question;
    question.setFont(localFont);
    question.setString(L"Закончить игру и вернуться к уровням?");
    question.setCharacterSize(24u);
    question.setFillColor(sf::Color::White);
    question.setPosition(50.0f, 50.0f);

    sf::RectangleShape btnContinue(sf::Vector2f(180.0f, 50.0f));
    btnContinue.setPosition(40.0f, 130.0f);
    btnContinue.setFillColor(sf::Color(46, 204, 113));
    btnContinue.setOutlineThickness(2.0f);
    btnContinue.setOutlineColor(sf::Color::White);

    sf::Text txtContinue;
    txtContinue.setFont(localFont);
    txtContinue.setString(L"Продолжить");
    txtContinue.setCharacterSize(20u);
    txtContinue.setFillColor(sf::Color::White);
    txtContinue.setPosition(65.0f, 143.0f);

    sf::RectangleShape btnExit(sf::Vector2f(180.0f, 50.0f));
    btnExit.setPosition(280.0f, 130.0f);
    btnExit.setFillColor(sf::Color(231, 76, 60));
    btnExit.setOutlineThickness(2.0f);
    btnExit.setOutlineColor(sf::Color::White);

    sf::Text txtExit;
    txtExit.setFont(localFont);
    txtExit.setString(L"Выйти");
    txtExit.setCharacterSize(20u);
    txtExit.setFillColor(sf::Color::White);
    txtExit.setPosition(320.0f, 143.0f);

    while (confirmWin.isOpen()) {
        sf::Event event;
        while (confirmWin.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { confirmWin.close(); return false; }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) { confirmWin.close(); return false; }
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mPos = sf::Mouse::getPosition(confirmWin);
                if (btnContinue.getGlobalBounds().contains(static_cast<sf::Vector2f>(mPos))) { confirmWin.close(); return false; }
                if (btnExit.getGlobalBounds().contains(static_cast<sf::Vector2f>(mPos))) { confirmWin.close(); return true; }
            }
        }
        sf::Vector2i mPos = sf::Mouse::getPosition(confirmWin);
        btnContinue.setFillColor(btnContinue.getGlobalBounds().contains(static_cast<sf::Vector2f>(mPos)) ? sf::Color(39, 174, 96) : sf::Color(46, 204, 113));
        btnExit.setFillColor(btnExit.getGlobalBounds().contains(static_cast<sf::Vector2f>(mPos)) ? sf::Color(192, 57, 43) : sf::Color(231, 76, 60));

        confirmWin.clear(sf::Color(30, 30, 50));
        confirmWin.draw(question); confirmWin.draw(btnContinue); confirmWin.draw(txtContinue);
        confirmWin.draw(btnExit); confirmWin.draw(txtExit);
        confirmWin.display();
    }
    return false;
}

void GameplayState::initLevel(int levelIndex) {
    switch (levelIndex) {
    case 1: pairsCount = 8; break;
    case 2: pairsCount = 12; break;
    case 3: pairsCount = 17; break;
    case 4: pairsCount = 17; break;
    default: pairsCount = 8;
    }
    score = 0; moves = 0; mistakes = 0; firstCard = nullptr; secondCard = nullptr;
    flipTimer = 0.0f; gameState = PLAYING; gameTime = 0.0f; pairsNotFound = pairsCount;

    switch (levelIndex) {
    case 1: timeLimitEnabled = false; timeLimit = 0.0f; movesLimitEnabled = false; maxMoves = 0; break;
    case 2: timeLimitEnabled = true; timeLimit = 120.0f; movesLimitEnabled = false; maxMoves = 0; break;
    case 3: timeLimitEnabled = true; timeLimit = 90.0f; movesLimitEnabled = false; maxMoves = 0; break;
    case 4: timeLimitEnabled = true; timeLimit = 90.0f; movesLimitEnabled = true; maxMoves = 30; break;
    default: timeLimitEnabled = false; timeLimit = 0.0f; movesLimitEnabled = false; maxMoves = 0;
    }
    setupCards(pairsCount);
    scoreText.setString(L"Счёт: 0"); movesText.setString(L"Ходы: 0"); mistakesText.setString(L"Ошибки: 0");
}

void GameplayState::setupCards(int pairs) {
    cards.clear();
    int totalCards = pairs * 2;
    cards.resize(static_cast<size_t>(totalCards));
    std::vector<int> values;
    for (int i = 0; i < pairs; i++) { values.push_back(i); values.push_back(i); }
    std::shuffle(values.begin(), values.end(), std::default_random_engine(static_cast<unsigned int>(time(nullptr))));

    float cardSize = (pairs <= 8) ? 150.0f : (pairs <= 12 ? 120.0f : 100.0f);
    float padding = (pairs <= 8) ? 20.0f : (pairs <= 12 ? 15.0f : 10.0f);
    int cols = (pairs <= 8) ? 4 : (pairs <= 12 ? 5 : 6);
    float startX = (window.getSize().x - (cols * (cardSize + padding) - padding)) / 2.0f;
    float startY = 150.0f;

    for (int i = 0; i < totalCards; i++) {
        int row = i / cols; int col = i % cols;
        cards[static_cast<size_t>(i)].value = values[static_cast<size_t>(i)];
        cards[static_cast<size_t>(i)].shape.setSize(sf::Vector2f(cardSize, cardSize));
        cards[static_cast<size_t>(i)].shape.setPosition(startX + col * (cardSize + padding), startY + row * (cardSize + padding));
        cards[static_cast<size_t>(i)].shape.setFillColor(sf::Color(50, 50, 70));
        cards[static_cast<size_t>(i)].shape.setOutlineColor(sf::Color(100, 100, 120));
        cards[static_cast<size_t>(i)].shape.setOutlineThickness(2.0f);

        std::string path = "img/" + currentTheme + "/" + std::to_string(values[static_cast<size_t>(i)] + 1) + ".jpg";
        if (cards[static_cast<size_t>(i)].texture.loadFromFile(path)) {
            float sx = cardSize / static_cast<float>(cards[static_cast<size_t>(i)].texture.getSize().x);
            float sy = cardSize / static_cast<float>(cards[static_cast<size_t>(i)].texture.getSize().y);
            cards[static_cast<size_t>(i)].sprite.setScale(sx, sy);
            cards[static_cast<size_t>(i)].sprite.setTexture(cards[static_cast<size_t>(i)].texture);
        }
    }
}

int GameplayState::run(int levelIndex) {
    initLevel(levelIndex);
    sf::Clock clock; bool exitToMenu = false;

    while (!exitToMenu && window.isOpen()) {
        sf::Time dt = clock.restart();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { window.close(); return 0; }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                if (showGameExitConfirmation()) exitToMenu = true;
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mPos = sf::Mouse::getPosition(window);
                sf::Vector2f mouseWorld = window.mapPixelToCoords(mPos);

                if (closeBtn.getGlobalBounds().contains(static_cast<sf::Vector2f>(mPos))) {
                    if (showGameExitConfirmation()) exitToMenu = true;
                }

                if (gameState == PLAYING && flipTimer <= 0.0f) {
                    for (size_t i = 0; i < cards.size(); i++) {
                        if (!cards[i].isMatched && !cards[i].isFlipped && cards[i].shape.getGlobalBounds().contains(mouseWorld)) {
                            cards[i].isFlipped = true;
                            if (firstCard == nullptr) firstCard = &cards[i];
                            else if (secondCard == nullptr) {
                                secondCard = &cards[i]; moves++;
                                movesText.setString(L"Ходы: " + std::to_wstring(moves));

                                // проверка совпадений+звуки
                                if (firstCard->value == secondCard->value) {
                                    // звук правильного ответа
                                    if (matchBuffer) {
                                        matchSound.setVolume(sfxVolume);
                                        matchSound.play();
                                    }

                                    firstCard->isMatched = true; secondCard->isMatched = true;
                                    score += 10; scoreText.setString(L"Счёт: " + std::to_wstring(score));
                                    pairsNotFound--; firstCard = nullptr; secondCard = nullptr;
                                    if (pairsNotFound == 0) gameState = GAME_OVER;
                                }
                                else {
                                    // звук неправильного ответа
                                    if (errorBuffer) {
                                        errorSound.setVolume(sfxVolume);
                                        errorSound.play();
                                    }

                                    flipTimer = 1.0f; mistakes++;
                                    mistakesText.setString(L"Ошибки: " + std::to_wstring(mistakes));
                                }
                            }
                            break;
                        }
                    }
                }

                if (gameState == GAME_OVER || gameState == TIME_OUT) {
                    if (btnMenu.getGlobalBounds().contains(static_cast<sf::Vector2f>(mPos))) return 0;
                    if (btnRetry.getGlobalBounds().contains(static_cast<sf::Vector2f>(mPos))) initLevel(levelIndex);
                }
            }
        }

        if (exitToMenu) break;

        if (gameState == PLAYING) {
            gameTime += dt.asSeconds();
            if (flipTimer > 0.0f) {
                flipTimer -= dt.asSeconds();
                if (flipTimer <= 0.0f && firstCard && secondCard) {
                    firstCard->isFlipped = false; secondCard->isFlipped = false;
                    firstCard = nullptr; secondCard = nullptr;
                }
            }
            int m = static_cast<int>(gameTime) / 60;
            int s = static_cast<int>(gameTime) % 60;
            std::wstringstream ts;
            ts << L"Время: " << std::setfill(L'0') << std::setw(2) << m << L":" << std::setw(2) << s;
            timerText.setString(ts.str());
            if (timeLimitEnabled && gameTime >= timeLimit) gameState = TIME_OUT;
            if (movesLimitEnabled && moves >= maxMoves && pairsNotFound > 0) gameState = TIME_OUT;
        }

        window.clear(); window.draw(background);
        window.draw(closeBtn);
        sf::Vertex l1[] = { sf::Vertex(sf::Vector2f(1862.0f, 32.0f), sf::Color::White), sf::Vertex(sf::Vector2f(1888.0f, 58.0f), sf::Color::White) };
        sf::Vertex l2[] = { sf::Vertex(sf::Vector2f(1888.0f, 32.0f), sf::Color::White), sf::Vertex(sf::Vector2f(1862.0f, 58.0f), sf::Color::White) };
        window.draw(l1, 2, sf::Lines); window.draw(l2, 2, sf::Lines);

        window.draw(scoreText); window.draw(movesText); window.draw(mistakesText); window.draw(timerText);
        for (auto& card : cards) {
            window.draw(card.shape);
            if (card.isFlipped || card.isMatched) {
                card.sprite.setPosition(card.shape.getPosition());
                window.draw(card.sprite);
            }
        }

        if (gameState == GAME_OVER || gameState == TIME_OUT) {
            if (gameState == GAME_OVER && dbPtr) {
                dbPtr->updateLevelResult(levelIndex, gameTime, mistakes);
            }
            window.draw(overlay);
            sf::Text msg = (gameState == GAME_OVER) ? gameOverText : timeOverText;
            msg.setPosition((window.getSize().x - msg.getLocalBounds().width) / 2.0f, 200.0f);
            window.draw(msg);

            finalScoreText.setString(L"Время: " + timerText.getString());
            finalScoreText.setPosition((window.getSize().x - finalScoreText.getLocalBounds().width) / 2.0f, 280.0f);
            window.draw(finalScoreText);

            finalMovesText.setString(L"Ходов: " + std::to_wstring(moves));
            finalMovesText.setPosition((window.getSize().x - finalMovesText.getLocalBounds().width) / 2.0f, 340.0f);
            window.draw(finalMovesText);

            float centerX = window.getSize().x / 2.0f;
            btnMenu.setPosition(centerX + 20.0f, 420.0f);
            btnRetry.setPosition(centerX - 270.0f, 420.0f);

            window.draw(btnRetry);
            window.draw(btnMenu);

            sf::Text txtRetry, txtMenu;
            txtRetry.setFont(font);
            txtRetry.setString(L"Ещё раз");
            txtRetry.setCharacterSize(28u);
            txtRetry.setFillColor(sf::Color::White);
            txtRetry.setPosition(centerX - 220.0f, 438.0f);

            txtMenu.setFont(font);
            txtMenu.setString(L"В меню");
            txtMenu.setCharacterSize(28u);
            txtMenu.setFillColor(sf::Color::White);
            txtMenu.setPosition(centerX + 70.0f, 438.0f);

            window.draw(txtRetry);
            window.draw(txtMenu);
        }
        window.display();
    }
    return 0;
}

void GameplayState::setTheme(const std::string& theme) {
    currentTheme = theme;
}
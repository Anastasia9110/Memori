#include <SFML/Graphics.hpp>
#include "GameApp.h"
#include <iostream>
#include <direct.h>

#pragma warning(disable: 26495)

int main() {
    char buffer[512];
    _getcwd(buffer, 512);
    std::cout << "[INFO] Рабочая папка: " << buffer << std::endl;

    sf::RenderWindow window(sf::VideoMode(1920, 1080), L"Мемори", sf::Style::None);

    if (!window.isOpen()) {
        std::cerr << "Ошибка создания окна!" << std::endl;
        return 1;
    }

    try {
        GameApp app(window);
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
    }

    return 0;
}
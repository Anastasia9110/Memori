#include "GameMenu.h"

void game::GameMenu::setInitText(sf::Text& text, sf::String str, float xpos, float ypos) {
    text.setFont(font);
    text.setFillColor(menu_text_color);
    text.setString(str);
    text.setCharacterSize(size_font);
    text.setPosition(xpos, ypos);
    text.setOutlineThickness(3);
    text.setOutlineColor(border_color);
}

void game::GameMenu::AlignMenu(int posx) {
    float nullx = 0;

    for (int i = 0; i < max_menu; i++) {
        switch (posx) {
        case 0:
            nullx = 0;
            break;
        case 1:
            nullx = mainMenu[i].getLocalBounds().width;
            break;
        case 2:
            nullx = mainMenu[i].getLocalBounds().width / 2;
            break;
        }
        mainMenu[i].setPosition(mainMenu[i].getPosition().x - nullx, mainMenu[i].getPosition().y);
    }
}

int game::GameMenu::getMouseHoverIndex() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(mywindow);

    for (int i = 0; i < max_menu; i++) {
        sf::FloatRect bounds = mainMenu[i].getGlobalBounds();
        if (bounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            return i;
        }
    }
    return -1;
}

void game::GameMenu::updateMouseHover() {
    int hoveredIndex = getMouseHoverIndex();

    for (int i = 0; i < max_menu; i++) {
        if (i == hoveredIndex) {
            mainMenu[i].setFillColor(chose_text_color);
        }
        else if (i != mainMenuSelected) {
            mainMenu[i].setFillColor(menu_text_color);
        }
    }
}

bool game::GameMenu::handleMouseClick() {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        int clickedIndex = getMouseHoverIndex();

        if (clickedIndex != -1) {
            if (mainMenuSelected >= 0 && mainMenuSelected < max_menu) {
                mainMenu[mainMenuSelected].setFillColor(menu_text_color);
            }

            mainMenuSelected = clickedIndex;
            mainMenu[mainMenuSelected].setFillColor(chose_text_color);

            return true;
        }
    }
    return false;
}

game::GameMenu::GameMenu(sf::RenderWindow& window, float menux, float menuy, int index,
    sf::String name[], int sizeFont, int step)
    :mywindow(window), menu_X(menux), menu_Y(menuy), size_font(sizeFont), menu_Step(step) {
    if (!font.loadFromFile("resources/Roboto/Roboto.ttf"))
        exit(32);

    max_menu = index;
    mainMenu = new sf::Text[max_menu];

    for (int i = 0, ypos = menu_Y; i < max_menu; i++, ypos += menu_Step)
        setInitText(mainMenu[i], name[i], menu_X, ypos);

    mainMenuSelected = 0;
    if (max_menu > 0)
        mainMenu[mainMenuSelected].setFillColor(chose_text_color);
}

void game::GameMenu::MoveUp() {
    if (max_menu == 0) return;

    mainMenu[mainMenuSelected].setFillColor(menu_text_color);
    mainMenuSelected--;

    if (mainMenuSelected < 0)
        mainMenuSelected = max_menu - 1;

    mainMenu[mainMenuSelected].setFillColor(chose_text_color);
}

void game::GameMenu::MoveDown() {
    if (max_menu == 0) return;

    mainMenu[mainMenuSelected].setFillColor(menu_text_color);
    mainMenuSelected++;

    if (mainMenuSelected >= max_menu)
        mainMenuSelected = 0;

    mainMenu[mainMenuSelected].setFillColor(chose_text_color);
}

void game::GameMenu::draw() {
    for (int i = 0; i < max_menu; i++)
        mywindow.draw(mainMenu[i]);
}

void game::GameMenu::setColorTextMenu(sf::Color menColor, sf::Color ChoColor, sf::Color BordColor) {
    menu_text_color = menColor;
    chose_text_color = ChoColor;
    border_color = BordColor;

    for (int i = 0; i < max_menu; i++) {
        mainMenu[i].setFillColor(menu_text_color);
        mainMenu[i].setOutlineColor(border_color);
    }

    if (mainMenuSelected >= 0 && mainMenuSelected < max_menu)
        mainMenu[mainMenuSelected].setFillColor(chose_text_color);
}
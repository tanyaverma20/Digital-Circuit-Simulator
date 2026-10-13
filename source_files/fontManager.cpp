#include "../header_files/fontManager.hpp"

sf::Image FontManager::icon;
sf::Font FontManager::font;

void FontManager::loadAssets(sf::RenderWindow &window)
{
    // icon.loadFromFile("./assets/images/logicIcon.png");
    icon.loadFromFile("./assets/images/logic.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    if (!font.loadFromFile("./assets/fonts/Hack-Regular.ttf"))
    {
        std::cout << "Failed to load font" << std::endl;
    }
}
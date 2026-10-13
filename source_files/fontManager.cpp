#include "../header_files/fontManager.hpp"

sf::Image FontManager::icon;
sf::Font FontManager::font;

void FontManager::loadAssets(sf::RenderWindow &window)
{
    if (icon.loadFromFile("./assets/images/logic.png"))
    {
#if SFML_VERSION_MAJOR >= 3
        window.setIcon(icon);
#else
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
#endif
    }

#if SFML_VERSION_MAJOR >= 3
    if (!font.openFromFile("./assets/fonts/Hack-Regular.ttf"))
#else
    if (!font.loadFromFile("./assets/fonts/Hack-Regular.ttf"))
#endif
    {
        std::cout << "Failed to load font" << std::endl;
    }
}
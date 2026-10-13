#ifndef WINDOW_SETTINGS
#define WINDOW_SETTINGS

#include "includes.hpp"

class FontManager
{
public:
    static sf::Image icon;

    static sf::Font font;
    static void loadAssets(sf::RenderWindow &window);
};

#endif
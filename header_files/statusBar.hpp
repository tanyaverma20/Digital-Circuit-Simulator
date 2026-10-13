#pragma once

#include "includes.hpp"
// #include "fontManager.hpp"

class StatusBar
{
    sf::Text status;
    std::string text;
    sf::Font font;
    sf::Vector2f pos;
    sf::Color textColor;
    float lineSpacing;
    float characterSize;

public:
    StatusBar(sf::Font &font);
    void setFont(sf::Font);
    void addText(std::string);
    void setPosition(sf::Vector2f pos);
    void setTextColor(sf::Color);
    void setLineSpacing(float);
    void setCharacterSize(float);
    void setString(std::string);
    sf::FloatRect getGlobalBounds();
    void drawTo(sf::RenderWindow &);
    
};
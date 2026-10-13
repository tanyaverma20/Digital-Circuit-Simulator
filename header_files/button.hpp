#pragma once

#include "includes.hpp"

class Button
{
    float width;
    float height;
    sf::Color bgColor = sf::Color::White;
    sf::Color textColor = sf::Color::Black;
    sf::Font font;
    sf::Vector2f pos;
    float padding;

    sf::RectangleShape button;
    sf::Text label;
    std::string btnText;

    sf::FloatRect bounds;

    void setText(std::string str);
    void setSize();

public:
    Button(std::string str = "button", sf::Vector2f pos = sf::Vector2f(100, 100));
    Button(float width, float height, sf::Vector2f pos);
    void setLabel(std::string str);
    void setBgColor(sf::Color color);
    void setScale(float scaleX, float scaleY);
    void setTextColor(sf::Color color);
    void setPosition(sf::Vector2f pos);
    void setFont(sf::Font &font);
    void setSize(float width, float height);
    void setPadding(float padding);

    sf::Color getBgColor();
    sf::Color getTextColor();
    sf::Vector2f getPosition();
    std::string getText();

    void drawTo(sf::RenderWindow &window);
    void displayOrigin();
    bool isHovered(sf::RenderWindow &window);
    sf::FloatRect getLocalBounds();
    sf::FloatRect getGlobalBounds();
    bool contains(sf::Vector2f);
};
#include "../header_files/button.hpp"
#include "../header_files/fontManager.hpp"

Button::Button(std::string str, sf::Vector2f pos)
{

    btnText = str;
    padding = 20.f;

    setText(str);
    setSize();

    setPosition(pos);

    setBgColor(bgColor);
    setTextColor(textColor);
}

Button::Button(float width, float height, sf::Vector2f pos)
{
    setSize(width, height);
}
void Button::setPosition(sf::Vector2f pos)
{
    this->pos = pos;

    button.setPosition(pos);
    label.setPosition(sf::Vector2f(pos.x - 1, pos.y - 2));
}

void Button::setSize()
{
    button.setSize(sf::Vector2f(label.getLocalBounds().width + padding, label.getLocalBounds().height + padding));

    label.setOrigin(label.getLocalBounds().width / 2, label.getLocalBounds().height / 2);
    button.setOrigin(button.getLocalBounds().width / 2, button.getLocalBounds().height / 2);
}

void Button::setSize(float width, float height)
{
    this->width = width;
    this->height = height;
}

void Button::setText(std::string str)
{
    label.setFont(FontManager::font);
    label.setCharacterSize(14);
    label.setString(str);
}

void Button::drawTo(sf::RenderWindow &window)
{
    window.draw(button);
    window.draw(label);
}

void Button::setLabel(std::string str)
{
    label.setString(str);
    setSize();
}

void Button::setBgColor(sf::Color color)
{
    bgColor = color;
    button.setFillColor(bgColor);
}
void Button::setTextColor(sf::Color color)
{
    textColor = color;
    label.setFillColor(textColor);
}

void Button::setFont(sf::Font &font)
{
    label.setFont(font);
}

sf::Vector2f Button::getPosition()
{
    return pos;
}

void Button::setPadding(float padding)
{
    this->padding = padding;
    setSize();
}

bool Button::isHovered(sf::RenderWindow &window)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    return button.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

void Button::setScale(float scaleX, float scaleY)
{
    button.setScale(scaleX, scaleY);
    label.setScale(scaleX, scaleY);
}

sf::Color Button::getBgColor()
{
    return bgColor;
}
sf::Color Button::getTextColor()
{
    return textColor;
}

sf::FloatRect Button::getLocalBounds()
{
    return button.getLocalBounds();
}
sf::FloatRect Button::getGlobalBounds()
{
    return button.getGlobalBounds();
}

bool Button::contains(sf::Vector2f pos)
{
    return (button.getGlobalBounds().contains(pos));
}

std::string Button::getText()
{
    return this->btnText;
}

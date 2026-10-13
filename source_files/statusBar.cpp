#include "../header_files/statusBar.hpp"

StatusBar::StatusBar(sf::Font &font)
{
    this->font = font;
    text = " ";
    textColor = sf::Color::White;
    lineSpacing = 1.5;
    characterSize = 14;

    status.setFont(font);
    status.setCharacterSize(characterSize);
    status.setLineSpacing(lineSpacing);
    status.setFillColor(textColor);

    status.setString("gates: 0");

    status.setPosition(100.f, 100.f);
}

void StatusBar::setFont(sf::Font font)
{
    this->font = font;
    status.setFont(font);
}
void StatusBar::addText(std::string text)
{
    this->text.append(" " + text);
    status.setString(this->text);
}
void StatusBar::setPosition(sf::Vector2f pos)
{
    this->pos = pos;
    status.setPosition(pos);
}
void StatusBar::setTextColor(sf::Color color)
{
    textColor = color;
    status.setFillColor(color);
}
void StatusBar::setLineSpacing(float value)
{
    lineSpacing = value;
    status.setLineSpacing(lineSpacing);
}
void StatusBar::setCharacterSize(float value)
{
    characterSize = value;
    status.setCharacterSize(characterSize);
}
void StatusBar::setString(std::string text)
{
    this->text = text;
    status.setString(this->text);
}
void StatusBar::drawTo(sf::RenderWindow &window)
{
    window.draw(status);
}

sf::FloatRect StatusBar::getGlobalBounds()
{
    return status.getGlobalBounds();
}

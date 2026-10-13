#include "../header_files/port.hpp"

std::vector<Port> Port::ports;
// Port *Port::prevPort = nullptr;
// Port *Port::selected = nullptr;

// sf::RectangleShape Port::currentConnection;
// sf::Vector2f Port::startPoint;
// bool Port::isConnecting = false;

float Port::portRadius = 5.f;

Port::Port()
{

    port.setRadius(Port::portRadius);
    port.setOrigin(port.getRadius(), port.getRadius());

    colorFalse = sf::Color::White;
    colorTrue = sf::Color::Green;
    state = false;

    updateColor();
}

void Port::setOrigin(float x, float y)
{
    port.setOrigin(x, y);
}

void Port::setState(bool state)
{
    this->state = state;
    updateColor();
}
void Port::setRadius(float radius)
{
    this->radius = radius;
    port.setRadius(radius);
}
void Port::setPosition(sf::Vector2f pos)
{
    port.setPosition(pos);
}
void Port::toggleState()
{
    state = !state;
    updateColor();
}

bool Port::getState()
{
    return state;
}
float Port::getRadius()
{
    return radius;
}
sf::Vector2f Port::getPosition()
{
    return port.getPosition();
}

void Port::updateColor()
{
    state ? port.setFillColor(colorTrue) : port.setFillColor(colorFalse);
}

void Port::drawTo(sf::RenderWindow &window)
{
    window.draw(port);
}

//

bool Port::isHovered(sf::RenderWindow &window)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    return (port.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)));
}

void Port::hoverAction(sf::RenderWindow &window)
{

    if (isHovered(window))
    {
        port.setFillColor(colorTrue);
    }
    else
    {
        port.setFillColor(colorFalse);
    }
}

bool Port::isClicked(sf::RenderWindow &window)
{
    return (isHovered(window) && sf::Mouse::isButtonPressed(sf::Mouse::Left));
}

void Port::clickAction(sf::RenderWindow &window)
{
    if (isClicked(window))
    {
        toggleState();
    }
}

bool Port::contains(sf::Vector2f pos)
{
    return (port.getGlobalBounds().contains(pos));
}

void Port::duplicate()
{
    Port::ports.push_back(*this);
}

void Port::setOutlineColor(sf::Color color)
{
    port.setOutlineColor(color);
}
void Port::setOutlineThickness(float value)
{
    port.setOutlineThickness(value);
}

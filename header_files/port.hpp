#ifndef PORT
#define PORT

#include "../header_files/includes.hpp"

class Port
{
    sf::CircleShape port;
    float radius;
    bool state;

    sf::Color colorTrue;
    sf::Color colorFalse;

public:
    static std::vector<Port> ports;
    static float portRadius;

    Port();

    void setState(bool);
    void setRadius(float);
    void setPosition(sf::Vector2f);
    void setOutlineColor(sf::Color);
    void setOutlineThickness(float);
    void setOrigin(float, float);


    float getRadius();
    sf::Vector2f getPosition();

    bool getState();
    bool isHovered(sf::RenderWindow &window);
    bool isClicked(sf::RenderWindow &window);
    bool contains(sf::Vector2f pos);

    void hoverAction(sf::RenderWindow &window);
    void clickAction(sf::RenderWindow &window);
    void updateColor();
    void toggleState();
    void duplicate();
    void drawTo(sf::RenderWindow &);
};

#endif
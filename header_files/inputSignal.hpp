#pragma once

#include "includes.hpp"
#include "outputPort.hpp"
#include "inputPort.hpp"
#include "connection.hpp"

class InputSignal : public OutputPort
{

public:
    InputSignal(sf::Vector2f);
    ~InputSignal();
    static std::vector<InputSignal *> signals;
    static InputSignal *selected;
    void deleteConnections();
    static void removeAtPos(sf::Vector2f pos);
    static void removeAll();
    void duplicate();
};
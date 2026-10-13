#pragma once

#include "includes.hpp"
#include "outputPort.hpp"
#include "inputPort.hpp"
#include "connection.hpp"
#include "gate.hpp"

class OutputSignal : public InputPort
{

public:
    static std::vector<OutputSignal *> signals;
    OutputSignal(sf::Vector2f pos);
    ~OutputSignal();
    void deleteConnections();
    static void removeAtPos(sf::Vector2f pos);
    static void removeAll();
    void duplicate();
};
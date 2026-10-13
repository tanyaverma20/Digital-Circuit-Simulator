#pragma once

#include "port.hpp"

class InputPort;

class OutputPort : public Port
{

    bool state;

public:
    // ~OutputPort();
    std::vector<InputPort *> childPorts;

    // void setChildrenState();
};
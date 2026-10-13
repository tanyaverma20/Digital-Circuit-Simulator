
#pragma once

#include "port.hpp"

class OutputPort;

class InputPort : public Port
{

public:
    // ~InputPort();
    bool isConnected = false;
    OutputPort *parentPort = nullptr;
};
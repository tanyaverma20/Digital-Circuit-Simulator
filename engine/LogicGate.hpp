#ifndef ENGINE_LOGIC_GATE_HPP
#define ENGINE_LOGIC_GATE_HPP

#include "GateType.hpp"
#include "Pin.hpp"
#include <string>
#include <vector>

namespace eda {

class LogicGate {
public:
    std::string id;
    GateType type;
    std::vector<Pin> inputPins;
    Pin outputPin;
    float posX = 0.0f;
    float posY = 0.0f;

    LogicGate(std::string gateId, GateType gateType, size_t numInputs = 0, float x = 0.0f, float y = 0.0f);

    LogicValue evaluate();
    void setInputValue(size_t index, LogicValue val);
    LogicValue getInputValue(size_t index) const;
    LogicValue getOutputValue() const;
    void reset();
};

} // namespace eda

#endif // ENGINE_LOGIC_GATE_HPP

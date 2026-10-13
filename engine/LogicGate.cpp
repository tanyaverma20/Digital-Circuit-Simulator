#include "LogicGate.hpp"
#include <stdexcept>

namespace eda {

LogicGate::LogicGate(std::string gateId, GateType gateType, size_t numInputs, float x, float y)
    : id(std::move(gateId)), type(gateType), posX(x), posY(y) {
    size_t count = (numInputs > 0) ? numInputs : defaultInputCount(type);
    inputPins.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        inputPins.emplace_back(id, i, PinDirection::INPUT);
    }
    outputPin = Pin(id, 0, PinDirection::OUTPUT);
}

void LogicGate::setInputValue(size_t index, LogicValue val) {
    if (index >= inputPins.size()) {
        throw std::out_of_range("Input pin index out of range for gate: " + id);
    }
    inputPins[index].value = val;
}

LogicValue LogicGate::getInputValue(size_t index) const {
    if (index >= inputPins.size()) {
        throw std::out_of_range("Input pin index out of range for gate: " + id);
    }
    return inputPins[index].value;
}

LogicValue LogicGate::getOutputValue() const {
    return outputPin.value;
}

void LogicGate::reset() {
    for (auto& pin : inputPins) {
        pin.value = LogicValue::UNDEFINED;
    }
    outputPin.value = LogicValue::UNDEFINED;
}

LogicValue LogicGate::evaluate() {
    if (inputPins.empty()) {
        outputPin.value = LogicValue::UNDEFINED;
        return outputPin.value;
    }

    LogicValue result = LogicValue::UNDEFINED;

    switch (type) {
        case GateType::NOT:
            result = logicNot(inputPins[0].value);
            break;

        case GateType::BUFFER:
            result = inputPins[0].value;
            break;

        case GateType::AND: {
            result = LogicValue::HIGH;
            for (const auto& pin : inputPins) {
                result = logicAnd(result, pin.value);
            }
            break;
        }

        case GateType::OR: {
            result = LogicValue::LOW;
            for (const auto& pin : inputPins) {
                result = logicOr(result, pin.value);
            }
            break;
        }

        case GateType::NAND: {
            LogicValue acc = LogicValue::HIGH;
            for (const auto& pin : inputPins) {
                acc = logicAnd(acc, pin.value);
            }
            result = logicNot(acc);
            break;
        }

        case GateType::NOR: {
            LogicValue acc = LogicValue::LOW;
            for (const auto& pin : inputPins) {
                acc = logicOr(acc, pin.value);
            }
            result = logicNot(acc);
            break;
        }

        case GateType::XOR: {
            result = inputPins[0].value;
            for (size_t i = 1; i < inputPins.size(); ++i) {
                result = logicXor(result, inputPins[i].value);
            }
            break;
        }

        case GateType::XNOR: {
            LogicValue acc = inputPins[0].value;
            for (size_t i = 1; i < inputPins.size(); ++i) {
                acc = logicXor(acc, inputPins[i].value);
            }
            result = logicNot(acc);
            break;
        }
    }

    outputPin.value = result;
    return result;
}

} // namespace eda

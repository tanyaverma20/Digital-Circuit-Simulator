#ifndef ENGINE_PIN_HPP
#define ENGINE_PIN_HPP

#include "LogicValue.hpp"
#include <string>
#include <cstddef>

namespace eda {

enum class PinDirection : uint8_t {
    INPUT,
    OUTPUT
};

struct Pin {
    std::string componentId;
    size_t pinIndex = 0;
    PinDirection direction = PinDirection::INPUT;
    LogicValue value = LogicValue::UNDEFINED;
    bool isConnected = false;

    Pin() = default;
    Pin(std::string compId, size_t idx, PinDirection dir)
        : componentId(std::move(compId)), pinIndex(idx), direction(dir) {}
};

} // namespace eda

#endif // ENGINE_PIN_HPP

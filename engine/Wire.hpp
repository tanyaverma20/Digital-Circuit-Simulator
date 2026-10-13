#ifndef ENGINE_WIRE_HPP
#define ENGINE_WIRE_HPP

#include "LogicValue.hpp"
#include <string>
#include <cstddef>

namespace eda {

struct Wire {
    std::string id;
    std::string sourceId;      // ID of component driving the net (Output)
    size_t sourcePinIndex = 0; // Driver pin index (usually 0 for gates/inputs)
    std::string targetId;      // ID of component receiving the net (Input)
    size_t targetPinIndex = 0; // Input pin index
    LogicValue state = LogicValue::UNDEFINED;

    Wire() = default;
    Wire(std::string wireId, std::string src, size_t srcPin, std::string dst, size_t dstPin)
        : id(std::move(wireId)),
          sourceId(std::move(src)),
          sourcePinIndex(srcPin),
          targetId(std::move(dst)),
          targetPinIndex(dstPin) {}
};

} // namespace eda

#endif // ENGINE_WIRE_HPP

#ifndef ENGINE_GATE_TYPE_HPP
#define ENGINE_GATE_TYPE_HPP

#include <string>
#include <cstdint>

namespace eda {

enum class GateType : uint8_t {
    AND,
    OR,
    NOT,
    NAND,
    NOR,
    XOR,
    XNOR,
    BUFFER
};

std::string gateTypeToString(GateType type);
GateType stringToGateType(const std::string& str);
size_t defaultInputCount(GateType type);
std::ostream& operator<<(std::ostream& os, GateType type);

} // namespace eda


#endif // ENGINE_GATE_TYPE_HPP

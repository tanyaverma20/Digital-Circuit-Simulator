#include "GateType.hpp"
#include <algorithm>
#include <stdexcept>

namespace eda {

std::string gateTypeToString(GateType type) {
    switch (type) {
        case GateType::AND: return "AND";
        case GateType::OR: return "OR";
        case GateType::NOT: return "NOT";
        case GateType::NAND: return "NAND";
        case GateType::NOR: return "NOR";
        case GateType::XOR: return "XOR";
        case GateType::XNOR: return "XNOR";
        case GateType::BUFFER: return "BUFFER";
        default: return "UNKNOWN";
    }
}

GateType stringToGateType(const std::string& str) {
    std::string s = str;
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    if (s == "AND") return GateType::AND;
    if (s == "OR") return GateType::OR;
    if (s == "NOT") return GateType::NOT;
    if (s == "NAND") return GateType::NAND;
    if (s == "NOR") return GateType::NOR;
    if (s == "XOR") return GateType::XOR;
    if (s == "XNOR") return GateType::XNOR;
    if (s == "BUFFER" || s == "BUF") return GateType::BUFFER;
    throw std::invalid_argument("Unknown gate type: " + str);
}

size_t defaultInputCount(GateType type) {
    switch (type) {
        case GateType::NOT:
        case GateType::BUFFER:
            return 1;
        default:
            return 2;
    }
}

std::ostream& operator<<(std::ostream& os, GateType type) {
    return os << gateTypeToString(type);
}

} // namespace eda


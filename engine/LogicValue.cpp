#include "LogicValue.hpp"

namespace eda {

LogicValue logicNot(LogicValue a) {
    switch (a) {
        case LogicValue::LOW: return LogicValue::HIGH;
        case LogicValue::HIGH: return LogicValue::LOW;
        default: return LogicValue::UNDEFINED;
    }
}

LogicValue logicAnd(LogicValue a, LogicValue b) {
    // Dominant 0 in CMOS/EDA: if any input is 0, output is 0 even if other input is undefined
    if (a == LogicValue::LOW || b == LogicValue::LOW) {
        return LogicValue::LOW;
    }
    if (a == LogicValue::HIGH && b == LogicValue::HIGH) {
        return LogicValue::HIGH;
    }
    return LogicValue::UNDEFINED;
}

LogicValue logicOr(LogicValue a, LogicValue b) {
    // Dominant 1 in CMOS/EDA: if any input is 1, output is 1 even if other input is undefined
    if (a == LogicValue::HIGH || b == LogicValue::HIGH) {
        return LogicValue::HIGH;
    }
    if (a == LogicValue::LOW && b == LogicValue::LOW) {
        return LogicValue::LOW;
    }
    return LogicValue::UNDEFINED;
}

LogicValue logicXor(LogicValue a, LogicValue b) {
    if (a == LogicValue::UNDEFINED || b == LogicValue::UNDEFINED) {
        return LogicValue::UNDEFINED;
    }
    return (a != b) ? LogicValue::HIGH : LogicValue::LOW;
}

LogicValue logicNand(LogicValue a, LogicValue b) {
    return logicNot(logicAnd(a, b));
}

LogicValue logicNor(LogicValue a, LogicValue b) {
    return logicNot(logicOr(a, b));
}

LogicValue logicXnor(LogicValue a, LogicValue b) {
    return logicNot(logicXor(a, b));
}

char toChar(LogicValue v) {
    switch (v) {
        case LogicValue::LOW: return '0';
        case LogicValue::HIGH: return '1';
        case LogicValue::UNDEFINED: default: return 'X';
    }
}

std::string toString(LogicValue v) {
    switch (v) {
        case LogicValue::LOW: return "LOW";
        case LogicValue::HIGH: return "HIGH";
        case LogicValue::UNDEFINED: default: return "UNDEFINED";
    }
}

LogicValue fromChar(char c) {
    switch (c) {
        case '0': return LogicValue::LOW;
        case '1': return LogicValue::HIGH;
        default: return LogicValue::UNDEFINED;
    }
}

LogicValue fromBool(bool b) {
    return b ? LogicValue::HIGH : LogicValue::LOW;
}

bool toBool(LogicValue v, bool defaultVal) {
    if (v == LogicValue::HIGH) return true;
    if (v == LogicValue::LOW) return false;
    return defaultVal;
}

std::ostream& operator<<(std::ostream& os, LogicValue v) {
    os << toString(v);
    return os;
}

} // namespace eda

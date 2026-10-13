#ifndef ENGINE_LOGIC_VALUE_HPP
#define ENGINE_LOGIC_VALUE_HPP

#include <iostream>
#include <string>
#include <cstdint>

namespace eda {

/**
 * @brief Three-valued logic representation standard in EDA simulation.
 * LOW (0), HIGH (1), UNDEFINED (X / floating / uninitialized).
 */
enum class LogicValue : uint8_t {
    LOW = 0,
    HIGH = 1,
    UNDEFINED = 2
};

// Logic operations supporting EDA truth evaluation
LogicValue logicNot(LogicValue a);
LogicValue logicAnd(LogicValue a, LogicValue b);
LogicValue logicOr(LogicValue a, LogicValue b);
LogicValue logicXor(LogicValue a, LogicValue b);
LogicValue logicNand(LogicValue a, LogicValue b);
LogicValue logicNor(LogicValue a, LogicValue b);
LogicValue logicXnor(LogicValue a, LogicValue b);

// Helper functions
char toChar(LogicValue v);
std::string toString(LogicValue v);
LogicValue fromChar(char c);
LogicValue fromBool(bool b);
bool toBool(LogicValue v, bool defaultVal = false);

// Stream output
std::ostream& operator<<(std::ostream& os, LogicValue v);

} // namespace eda

#endif // ENGINE_LOGIC_VALUE_HPP

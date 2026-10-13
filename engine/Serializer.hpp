#ifndef ENGINE_SERIALIZER_HPP
#define ENGINE_SERIALIZER_HPP

#include "Circuit.hpp"
#include <string>
#include <iostream>

namespace eda {

struct SerializationResult {
    bool success = false;
    std::string message;

    operator bool() const { return success; }
};

class Serializer {
public:
    static SerializationResult saveToFile(const Circuit& circuit, const std::string& filePath);
    static SerializationResult loadFromFile(Circuit& circuit, const std::string& filePath);

    static std::string serializeToString(const Circuit& circuit);
    static SerializationResult deserializeFromString(Circuit& circuit, const std::string& content);
};

} // namespace eda

#endif // ENGINE_SERIALIZER_HPP

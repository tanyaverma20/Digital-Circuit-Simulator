#include "Serializer.hpp"
#include <fstream>
#include <sstream>

namespace eda {

std::string Serializer::serializeToString(const Circuit& circuit) {
    std::ostringstream ss;
    ss << "# DIGITAL CIRCUIT SIMULATOR NETLIST V2\n";

    // Write Gates
    for (const auto& [id, gate] : circuit.getGates()) {
        ss << "GATE " << gate->id << " "
           << gateTypeToString(gate->type) << " "
           << gate->inputPins.size() << " "
           << gate->posX << " "
           << gate->posY << "\n";
    }

    // Write Inputs
    for (const auto& [id, in] : circuit.getInputs()) {
        ss << "INPUT " << in.id << " "
           << (in.label.empty() ? in.id : in.label) << " "
           << toChar(in.state) << " "
           << in.posX << " "
           << in.posY << "\n";
    }

    // Write Outputs
    for (const auto& [id, out] : circuit.getOutputs()) {
        ss << "OUTPUT " << out.id << " "
           << (out.label.empty() ? out.id : out.label) << " "
           << out.posX << " "
           << out.posY << "\n";
    }

    // Write Wires
    for (const auto& wire : circuit.getWires()) {
        ss << "WIRE " << wire.id << " "
           << wire.sourceId << " "
           << wire.sourcePinIndex << " "
           << wire.targetId << " "
           << wire.targetPinIndex << "\n";
    }

    return ss.str();
}

SerializationResult Serializer::deserializeFromString(Circuit& circuit, const std::string& content) {
    circuit.clear();
    std::istringstream ss(content);
    std::string line;
    size_t lineNum = 0;

    struct DeferredWire {
        std::string srcId;
        size_t srcPin = 0;
        std::string dstId;
        size_t dstPin = 0;
    };
    std::vector<DeferredWire> deferredWires;

    while (std::getline(ss, line)) {
        lineNum++;
        // Trim leading whitespace
        size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) continue; // blank line
        line = line.substr(first);

        if (line[0] == '#') continue; // comment

        std::istringstream lineStream(line);
        std::string tag;
        lineStream >> tag;

        if (tag == "GATE") {
            std::string id, typeStr;
            size_t numInputs = 0;
            float x = 0.0f, y = 0.0f;
            lineStream >> id >> typeStr >> numInputs >> x >> y;
            try {
                GateType type = stringToGateType(typeStr);
                circuit.addGate(type, id, numInputs, x, y);
            } catch (const std::exception& e) {
                return {false, "Error on line " + std::to_string(lineNum) + ": " + e.what()};
            }
        } else if (tag == "INPUT") {
            std::string id, label;
            char valChar = '0';
            float x = 0.0f, y = 0.0f;
            lineStream >> id >> label >> valChar >> x >> y;
            circuit.addInput(id, label, fromChar(valChar), x, y);
        } else if (tag == "OUTPUT") {
            std::string id, label;
            float x = 0.0f, y = 0.0f;
            lineStream >> id >> label >> x >> y;
            circuit.addOutput(id, label, x, y);
        } else if (tag == "WIRE") {
            std::string wireId, srcId, dstId;
            size_t srcPin = 0, dstPin = 0;
            lineStream >> wireId >> srcId >> srcPin >> dstId >> dstPin;
            deferredWires.push_back({srcId, srcPin, dstId, dstPin});
        } else {
            // Check for legacy format: <GateType> <posX> <posY>
            try {
                GateType type = stringToGateType(tag);
                float x = 0.0f, y = 0.0f;
                if (lineStream >> x >> y) {
                    circuit.addGate(type, "", 0, x, y);
                }
            } catch (...) {
                // Ignore unrecognized line
            }
        }
    }

    // Connect deferred wires after all components are populated
    for (const auto& dw : deferredWires) {
        auto res = circuit.connect(dw.srcId, dw.srcPin, dw.dstId, dw.dstPin);
        if (!res.valid) {
            return {false, "Failed to restore connection (" + dw.srcId + " -> " + dw.dstId + "): " + res.message};
        }
    }

    return {true, "Successfully deserialized circuit"};
}

SerializationResult Serializer::saveToFile(const Circuit& circuit, const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return {false, "Failed to open file for writing: " + filePath};
    }
    file << serializeToString(circuit);
    file.close();
    return {true, "Saved successfully to " + filePath};
}

SerializationResult Serializer::loadFromFile(Circuit& circuit, const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return {false, "Failed to open file for reading: " + filePath};
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return deserializeFromString(circuit, buffer.str());
}

} // namespace eda

#ifndef ENGINE_CIRCUIT_HPP
#define ENGINE_CIRCUIT_HPP

#include "LogicGate.hpp"
#include "Wire.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace eda {

struct InputSource {
    std::string id;
    std::string label;
    LogicValue state = LogicValue::LOW;
    float posX = 0.0f;
    float posY = 0.0f;
    Pin pin;

    InputSource() = default;
    InputSource(std::string sourceId, std::string lbl = "", LogicValue st = LogicValue::LOW, float x = 0.0f, float y = 0.0f)
        : id(std::move(sourceId)), label(std::move(lbl)), state(st), posX(x), posY(y),
          pin(id, 0, PinDirection::OUTPUT) {
        pin.value = st;
    }
};

struct OutputProbe {
    std::string id;
    std::string label;
    LogicValue state = LogicValue::UNDEFINED;
    float posX = 0.0f;
    float posY = 0.0f;
    Pin pin;

    OutputProbe() = default;
    OutputProbe(std::string probeId, std::string lbl = "", float x = 0.0f, float y = 0.0f)
        : id(std::move(probeId)), label(std::move(lbl)), posX(x), posY(y),
          pin(id, 0, PinDirection::INPUT) {}
};

struct ValidationResult {
    bool valid = false;
    std::string message;

    operator bool() const { return valid; }
};

enum class CircuitStatus {
    OK,
    HAS_CYCLE,
    DISCONNECTED_INPUTS,
    EMPTY
};

std::ostream& operator<<(std::ostream& os, CircuitStatus status);


class Circuit {
private:
    std::unordered_map<std::string, std::shared_ptr<LogicGate>> m_gates;
    std::unordered_map<std::string, InputSource> m_inputs;
    std::unordered_map<std::string, OutputProbe> m_outputs;
    std::vector<Wire> m_wires;

    uint64_t m_nextId = 1;

public:
    Circuit() = default;
    ~Circuit() = default;

    // Factory ID generator
    std::string generateId(const std::string& prefix = "comp");

    // Component Management
    std::shared_ptr<LogicGate> addGate(GateType type, const std::string& id = "", size_t numInputs = 0, float x = 0.0f, float y = 0.0f);
    bool removeGate(const std::string& id);
    std::shared_ptr<LogicGate> getGate(const std::string& id) const;

    bool addInput(const std::string& id, const std::string& label = "", LogicValue initialVal = LogicValue::LOW, float x = 0.0f, float y = 0.0f);
    bool removeInput(const std::string& id);
    InputSource* getInput(const std::string& id);
    const InputSource* getInput(const std::string& id) const;
    void setInputValue(const std::string& id, LogicValue val);

    bool addOutput(const std::string& id, const std::string& label = "", float x = 0.0f, float y = 0.0f);
    bool removeOutput(const std::string& id);
    OutputProbe* getOutput(const std::string& id);
    const OutputProbe* getOutput(const std::string& id) const;
    LogicValue getOutputValue(const std::string& id) const;

    // Connection / Wire Management
    ValidationResult canConnect(const std::string& sourceId, size_t sourcePin, const std::string& targetId, size_t targetPin) const;
    ValidationResult connect(const std::string& sourceId, size_t sourcePin, const std::string& targetId, size_t targetPin);
    bool disconnect(const std::string& wireId);
    bool disconnectTarget(const std::string& targetId, size_t targetPin);
    void removeAllConnectionsFrom(const std::string& componentId);

    // Graph Analysis & Simulation
    bool hasCycles(std::vector<std::string>* cyclePath = nullptr) const;
    std::vector<std::string> getTopologicalOrder(bool* hasCycleOut = nullptr) const;
    CircuitStatus evaluate();
    void resetState();
    void clear();

    // Accessors
    const std::unordered_map<std::string, std::shared_ptr<LogicGate>>& getGates() const { return m_gates; }
    const std::unordered_map<std::string, InputSource>& getInputs() const { return m_inputs; }
    const std::unordered_map<std::string, OutputProbe>& getOutputs() const { return m_outputs; }
    const std::vector<Wire>& getWires() const { return m_wires; }
    size_t getGateCount() const { return m_gates.size(); }
    size_t getWireCount() const { return m_wires.size(); }
};

} // namespace eda

#endif // ENGINE_CIRCUIT_HPP

#include "Circuit.hpp"
#include <queue>
#include <algorithm>
#include <sstream>

namespace eda {

std::ostream& operator<<(std::ostream& os, CircuitStatus status) {
    switch (status) {
        case CircuitStatus::OK: return os << "OK";
        case CircuitStatus::HAS_CYCLE: return os << "HAS_CYCLE";
        case CircuitStatus::DISCONNECTED_INPUTS: return os << "DISCONNECTED_INPUTS";
        case CircuitStatus::EMPTY: default: return os << "EMPTY";
    }
}

std::string Circuit::generateId(const std::string& prefix) {

    return prefix + "_" + std::to_string(m_nextId++);
}

std::shared_ptr<LogicGate> Circuit::addGate(GateType type, const std::string& id, size_t numInputs, float x, float y) {
    std::string gateId = id.empty() ? generateId("gate") : id;
    auto gate = std::make_shared<LogicGate>(gateId, type, numInputs, x, y);
    m_gates[gateId] = gate;
    return gate;
}

bool Circuit::removeGate(const std::string& id) {
    auto it = m_gates.find(id);
    if (it == m_gates.end()) {
        return false;
    }
    removeAllConnectionsFrom(id);
    m_gates.erase(it);
    return true;
}

std::shared_ptr<LogicGate> Circuit::getGate(const std::string& id) const {
    auto it = m_gates.find(id);
    if (it != m_gates.end()) {
        return it->second;
    }
    return nullptr;
}

bool Circuit::addInput(const std::string& id, const std::string& label, LogicValue initialVal, float x, float y) {
    std::string inputId = id.empty() ? generateId("in") : id;
    if (m_inputs.find(inputId) != m_inputs.end()) {
        return false;
    }
    m_inputs.emplace(inputId, InputSource(inputId, label.empty() ? inputId : label, initialVal, x, y));
    return true;
}

bool Circuit::removeInput(const std::string& id) {
    auto it = m_inputs.find(id);
    if (it == m_inputs.end()) {
        return false;
    }
    removeAllConnectionsFrom(id);
    m_inputs.erase(it);
    return true;
}

InputSource* Circuit::getInput(const std::string& id) {
    auto it = m_inputs.find(id);
    return (it != m_inputs.end()) ? &it->second : nullptr;
}

const InputSource* Circuit::getInput(const std::string& id) const {
    auto it = m_inputs.find(id);
    return (it != m_inputs.end()) ? &it->second : nullptr;
}

void Circuit::setInputValue(const std::string& id, LogicValue val) {
    auto it = m_inputs.find(id);
    if (it != m_inputs.end()) {
        it->second.state = val;
        it->second.pin.value = val;
    }
}

bool Circuit::addOutput(const std::string& id, const std::string& label, float x, float y) {
    std::string outputId = id.empty() ? generateId("out") : id;
    if (m_outputs.find(outputId) != m_outputs.end()) {
        return false;
    }
    m_outputs.emplace(outputId, OutputProbe(outputId, label.empty() ? outputId : label, x, y));
    return true;
}

bool Circuit::removeOutput(const std::string& id) {
    auto it = m_outputs.find(id);
    if (it == m_outputs.end()) {
        return false;
    }
    removeAllConnectionsFrom(id);
    m_outputs.erase(it);
    return true;
}

OutputProbe* Circuit::getOutput(const std::string& id) {
    auto it = m_outputs.find(id);
    return (it != m_outputs.end()) ? &it->second : nullptr;
}

const OutputProbe* Circuit::getOutput(const std::string& id) const {
    auto it = m_outputs.find(id);
    return (it != m_outputs.end()) ? &it->second : nullptr;
}

LogicValue Circuit::getOutputValue(const std::string& id) const {
    auto it = m_outputs.find(id);
    if (it != m_outputs.end()) {
        return it->second.state;
    }
    return LogicValue::UNDEFINED;
}

ValidationResult Circuit::canConnect(const std::string& sourceId, size_t sourcePin,
                                     const std::string& targetId, size_t targetPin) const {
    if (sourceId.empty() || targetId.empty()) {
        return {false, "Component ID cannot be empty"};
    }
    if (sourceId == targetId) {
        return {false, "Self-connections are invalid: cannot connect component output to its own input"};
    }

    // Verify source exists and has an output pin
    bool sourceFound = false;
    if (auto gate = getGate(sourceId)) {
        if (sourcePin != 0) {
            return {false, "Gate output pin index must be 0"};
        }
        sourceFound = true;
    } else if (getInput(sourceId)) {
        if (sourcePin != 0) {
            return {false, "Input signal output pin index must be 0"};
        }
        sourceFound = true;
    }
    if (!sourceFound) {
        return {false, "Source component not found: " + sourceId};
    }

    // Verify target exists and has an input pin
    bool targetFound = false;
    if (auto gate = getGate(targetId)) {
        if (targetPin >= gate->inputPins.size()) {
            return {false, "Gate input pin index " + std::to_string(targetPin) + " out of bounds"};
        }
        targetFound = true;
    } else if (getOutput(targetId)) {
        if (targetPin != 0) {
            return {false, "Output probe input pin index must be 0"};
        }
        targetFound = true;
    }
    if (!targetFound) {
        return {false, "Target component not found: " + targetId};
    }

    // Check if target pin is already driven (driver contention / short circuit prevention)
    for (const auto& wire : m_wires) {
        if (wire.targetId == targetId && wire.targetPinIndex == targetPin) {
            return {false, "Target input pin (" + targetId + ":" + std::to_string(targetPin) +
                           ") is already driven by wire " + wire.id};
        }
        // Duplicate check
        if (wire.sourceId == sourceId && wire.sourcePinIndex == sourcePin &&
            wire.targetId == targetId && wire.targetPinIndex == targetPin) {
            return {false, "Duplicate connection already exists"};
        }
    }

    return {true, "Connection is valid"};
}

ValidationResult Circuit::connect(const std::string& sourceId, size_t sourcePin,
                                  const std::string& targetId, size_t targetPin) {
    ValidationResult res = canConnect(sourceId, sourcePin, targetId, targetPin);
    if (!res.valid) {
        return res;
    }

    std::string wireId = generateId("wire");
    m_wires.emplace_back(wireId, sourceId, sourcePin, targetId, targetPin);

    // Update pin connected state
    if (auto gate = getGate(targetId)) {
        gate->inputPins[targetPin].isConnected = true;
    } else if (auto out = getOutput(targetId)) {
        out->pin.isConnected = true;
    }

    if (auto gate = getGate(sourceId)) {
        gate->outputPin.isConnected = true;
    } else if (auto in = getInput(sourceId)) {
        in->pin.isConnected = true;
    }

    return {true, "Connected successfully: " + wireId};
}

bool Circuit::disconnect(const std::string& wireId) {
    auto it = std::find_if(m_wires.begin(), m_wires.end(), [&](const Wire& w) {
        return w.id == wireId;
    });
    if (it == m_wires.end()) {
        return false;
    }

    std::string targetId = it->targetId;
    size_t targetPin = it->targetPinIndex;
    std::string sourceId = it->sourceId;

    m_wires.erase(it);

    // Update target pin connected flag
    if (auto gate = getGate(targetId)) {
        gate->inputPins[targetPin].isConnected = false;
        gate->inputPins[targetPin].value = LogicValue::UNDEFINED;
    } else if (auto out = getOutput(targetId)) {
        out->pin.isConnected = false;
        out->state = LogicValue::UNDEFINED;
    }

    // Check if source still has outgoing wires
    bool sourceStillConnected = std::any_of(m_wires.begin(), m_wires.end(), [&](const Wire& w) {
        return w.sourceId == sourceId;
    });
    if (!sourceStillConnected) {
        if (auto gate = getGate(sourceId)) {
            gate->outputPin.isConnected = false;
        } else if (auto in = getInput(sourceId)) {
            in->pin.isConnected = false;
        }
    }

    return true;
}

bool Circuit::disconnectTarget(const std::string& targetId, size_t targetPin) {
    auto it = std::find_if(m_wires.begin(), m_wires.end(), [&](const Wire& w) {
        return w.targetId == targetId && w.targetPinIndex == targetPin;
    });
    if (it != m_wires.end()) {
        return disconnect(it->id);
    }
    return false;
}

void Circuit::removeAllConnectionsFrom(const std::string& componentId) {
    std::vector<std::string> toRemove;
    for (const auto& wire : m_wires) {
        if (wire.sourceId == componentId || wire.targetId == componentId) {
            toRemove.push_back(wire.id);
        }
    }
    for (const auto& wid : toRemove) {
        disconnect(wid);
    }
}

bool Circuit::hasCycles(std::vector<std::string>* cyclePath) const {
    // DFS cycle detection over gate dependency graph
    // States: 0 = unvisited, 1 = visiting (on current recursion stack), 2 = visited
    std::unordered_map<std::string, int> state;
    std::unordered_map<std::string, std::string> parent;
    for (const auto& [id, _] : m_gates) {
        state[id] = 0;
    }

    std::unordered_map<std::string, std::vector<std::string>> adj;
    for (const auto& wire : m_wires) {
        if (m_gates.find(wire.sourceId) != m_gates.end() &&
            m_gates.find(wire.targetId) != m_gates.end()) {
            adj[wire.sourceId].push_back(wire.targetId);
        }
    }

    std::string cycleStart = "";
    std::string cycleEnd = "";

    auto dfs = [&](auto self, const std::string& u) -> bool {
        state[u] = 1;
        for (const auto& v : adj[u]) {
            if (state[v] == 1) { // Cycle detected!
                cycleStart = v;
                cycleEnd = u;
                return true;
            }
            if (state[v] == 0) {
                parent[v] = u;
                if (self(self, v)) return true;
            }
        }
        state[u] = 2;
        return false;
    };

    for (const auto& [id, _] : m_gates) {
        if (state[id] == 0) {
            if (dfs(dfs, id)) {
                if (cyclePath) {
                    cyclePath->clear();
                    cyclePath->push_back(cycleStart);
                    for (std::string curr = cycleEnd; curr != cycleStart && !curr.empty(); curr = parent[curr]) {
                        cyclePath->push_back(curr);
                    }
                    cyclePath->push_back(cycleStart);
                    std::reverse(cyclePath->begin(), cyclePath->end());
                }
                return true;
            }
        }
    }
    return false;
}

std::vector<std::string> Circuit::getTopologicalOrder(bool* hasCycleOut) const {
    std::unordered_map<std::string, size_t> inDegree;
    std::unordered_map<std::string, std::vector<std::string>> adj;

    for (const auto& [id, _] : m_gates) {
        inDegree[id] = 0;
    }

    for (const auto& wire : m_wires) {
        if (m_gates.find(wire.sourceId) != m_gates.end() &&
            m_gates.find(wire.targetId) != m_gates.end()) {
            adj[wire.sourceId].push_back(wire.targetId);
            inDegree[wire.targetId]++;
        }
    }

    std::queue<std::string> q;
    for (const auto& [id, deg] : inDegree) {
        if (deg == 0) {
            q.push(id);
        }
    }

    std::vector<std::string> order;
    while (!q.empty()) {
        std::string u = q.front();
        q.pop();
        order.push_back(u);

        for (const auto& v : adj[u]) {
            inDegree[v]--;
            if (inDegree[v] == 0) {
                q.push(v);
            }
        }
    }

    bool hasCycle = (order.size() != m_gates.size());
    if (hasCycleOut) {
        *hasCycleOut = hasCycle;
    }

    return order;
}

CircuitStatus Circuit::evaluate() {
    if (m_gates.empty() && m_inputs.empty()) {
        return CircuitStatus::EMPTY;
    }

    bool cycleFound = false;
    std::vector<std::string> order = getTopologicalOrder(&cycleFound);

    // Initialize all gate input pins to UNDEFINED (handles disconnected pins)
    for (auto& [_, gate] : m_gates) {
        for (auto& pin : gate->inputPins) {
            pin.value = LogicValue::UNDEFINED;
        }
    }
    for (auto& [_, probe] : m_outputs) {
        probe.state = LogicValue::UNDEFINED;
    }

    // Function to propagate values along all wires
    auto propagateWires = [&]() {
        for (auto& wire : m_wires) {
            LogicValue srcVal = LogicValue::UNDEFINED;
            if (auto gate = getGate(wire.sourceId)) {
                srcVal = gate->getOutputValue();
            } else if (auto in = getInput(wire.sourceId)) {
                srcVal = in->state;
            }
            wire.state = srcVal;

            if (auto targetGate = getGate(wire.targetId)) {
                if (wire.targetPinIndex < targetGate->inputPins.size()) {
                    targetGate->setInputValue(wire.targetPinIndex, srcVal);
                }
            } else if (auto targetProbe = getOutput(wire.targetId)) {
                targetProbe->state = srcVal;
                targetProbe->pin.value = srcVal;
            }
        }
    };

    if (!cycleFound) {
        // Acyclic circuit (DAG): deterministic evaluation in topological order
        propagateWires();
        for (const auto& gateId : order) {
            auto gate = getGate(gateId);
            if (gate) {
                gate->evaluate();
                propagateWires();
            }
        }

        // Check for any disconnected pins
        for (const auto& [_, gate] : m_gates) {
            for (const auto& pin : gate->inputPins) {
                if (!pin.isConnected || pin.value == LogicValue::UNDEFINED) {
                    return CircuitStatus::DISCONNECTED_INPUTS;
                }
            }
        }
        return CircuitStatus::OK;
    } else {
        // Cyclic circuit: run iterative relaxation with iteration bound
        const size_t maxIterations = 64;
        for (size_t iter = 0; iter < maxIterations; ++iter) {
            propagateWires();
            bool changed = false;
            for (auto& [_, gate] : m_gates) {
                LogicValue oldVal = gate->getOutputValue();
                LogicValue newVal = gate->evaluate();
                if (oldVal != newVal) {
                    changed = true;
                }
            }
            if (!changed) {
                break; // Circuit settled into a stable state
            }
        }
        propagateWires();
        return CircuitStatus::HAS_CYCLE;
    }
}

void Circuit::resetState() {
    for (auto& [_, in] : m_inputs) {
        in.state = LogicValue::LOW;
        in.pin.value = LogicValue::LOW;
    }
    for (auto& [_, gate] : m_gates) {
        gate->reset();
    }
    for (auto& [_, out] : m_outputs) {
        out.state = LogicValue::UNDEFINED;
        out.pin.value = LogicValue::UNDEFINED;
    }
    for (auto& wire : m_wires) {
        wire.state = LogicValue::UNDEFINED;
    }
}

void Circuit::clear() {
    m_wires.clear();
    m_gates.clear();
    m_inputs.clear();
    m_outputs.clear();
    m_nextId = 1;
}

} // namespace eda

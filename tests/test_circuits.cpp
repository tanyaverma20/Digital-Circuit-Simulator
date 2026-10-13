#include "test_framework.hpp"
#include "../engine/Circuit.hpp"
#include "../engine/Serializer.hpp"

using namespace eda;

TEST_CASE(CircuitEngine, TestHalfAdderCircuit) {
    Circuit circuit;

    // Inputs A and B
    ASSERT_TRUE(circuit.addInput("A", "Input_A", LogicValue::LOW));
    ASSERT_TRUE(circuit.addInput("B", "Input_B", LogicValue::LOW));

    // Gates: XOR for Sum, AND for Carry
    auto xorGate = circuit.addGate(GateType::XOR, "XOR1", 2);
    auto andGate = circuit.addGate(GateType::AND, "AND1", 2);

    // Outputs
    ASSERT_TRUE(circuit.addOutput("Sum", "Sum_Out"));
    ASSERT_TRUE(circuit.addOutput("Carry", "Carry_Out"));

    // Connect Inputs to XOR
    ASSERT_TRUE(circuit.connect("A", 0, "XOR1", 0).valid);
    ASSERT_TRUE(circuit.connect("B", 0, "XOR1", 1).valid);

    // Connect Inputs to AND
    ASSERT_TRUE(circuit.connect("A", 0, "AND1", 0).valid);
    ASSERT_TRUE(circuit.connect("B", 0, "AND1", 1).valid);

    // Connect Gates to Outputs
    ASSERT_TRUE(circuit.connect("XOR1", 0, "Sum", 0).valid);
    ASSERT_TRUE(circuit.connect("AND1", 0, "Carry", 0).valid);

    // Test vectors for Half Adder:
    // Case 1: 0 + 0 = Sum: 0, Carry: 0
    circuit.setInputValue("A", LogicValue::LOW);
    circuit.setInputValue("B", LogicValue::LOW);
    ASSERT_EQ(circuit.evaluate(), CircuitStatus::OK);
    ASSERT_EQ(circuit.getOutputValue("Sum"), LogicValue::LOW);
    ASSERT_EQ(circuit.getOutputValue("Carry"), LogicValue::LOW);

    // Case 2: 0 + 1 = Sum: 1, Carry: 0
    circuit.setInputValue("A", LogicValue::LOW);
    circuit.setInputValue("B", LogicValue::HIGH);
    ASSERT_EQ(circuit.evaluate(), CircuitStatus::OK);
    ASSERT_EQ(circuit.getOutputValue("Sum"), LogicValue::HIGH);
    ASSERT_EQ(circuit.getOutputValue("Carry"), LogicValue::LOW);

    // Case 3: 1 + 0 = Sum: 1, Carry: 0
    circuit.setInputValue("A", LogicValue::HIGH);
    circuit.setInputValue("B", LogicValue::LOW);
    ASSERT_EQ(circuit.evaluate(), CircuitStatus::OK);
    ASSERT_EQ(circuit.getOutputValue("Sum"), LogicValue::HIGH);
    ASSERT_EQ(circuit.getOutputValue("Carry"), LogicValue::LOW);

    // Case 4: 1 + 1 = Sum: 0, Carry: 1
    circuit.setInputValue("A", LogicValue::HIGH);
    circuit.setInputValue("B", LogicValue::HIGH);
    ASSERT_EQ(circuit.evaluate(), CircuitStatus::OK);
    ASSERT_EQ(circuit.getOutputValue("Sum"), LogicValue::LOW);
    ASSERT_EQ(circuit.getOutputValue("Carry"), LogicValue::HIGH);
}

TEST_CASE(CircuitEngine, TestFullAdderMultiLevelDAG) {
    Circuit circuit;

    circuit.addInput("A", "A", LogicValue::LOW);
    circuit.addInput("B", "B", LogicValue::LOW);
    circuit.addInput("Cin", "Cin", LogicValue::LOW);

    circuit.addGate(GateType::XOR, "XOR1", 2);
    circuit.addGate(GateType::XOR, "XOR2", 2);
    circuit.addGate(GateType::AND, "AND1", 2);
    circuit.addGate(GateType::AND, "AND2", 2);
    circuit.addGate(GateType::OR,  "OR1",  2);

    circuit.addOutput("Sum", "Sum");
    circuit.addOutput("Cout", "Cout");

    // Connections:
    // XOR1 = A ^ B
    circuit.connect("A", 0, "XOR1", 0);
    circuit.connect("B", 0, "XOR1", 1);
    // XOR2 = XOR1 ^ Cin -> Sum
    circuit.connect("XOR1", 0, "XOR2", 0);
    circuit.connect("Cin", 0, "XOR2", 1);
    circuit.connect("XOR2", 0, "Sum", 0);

    // AND1 = XOR1 & Cin
    circuit.connect("XOR1", 0, "AND1", 0);
    circuit.connect("Cin", 0, "AND1", 1);

    // AND2 = A & B
    circuit.connect("A", 0, "AND2", 0);
    circuit.connect("B", 0, "AND2", 1);

    // OR1 = AND1 | AND2 -> Cout
    circuit.connect("AND1", 0, "OR1", 0);
    circuit.connect("AND2", 0, "OR1", 1);
    circuit.connect("OR1", 0, "Cout", 0);

    // Verify all 8 input combinations (2^3 truth table)
    struct TestCaseVector {
        LogicValue a, b, cin, sum, cout;
    };
    std::vector<TestCaseVector> vectors = {
        {LogicValue::LOW,  LogicValue::LOW,  LogicValue::LOW,  LogicValue::LOW,  LogicValue::LOW},
        {LogicValue::LOW,  LogicValue::LOW,  LogicValue::HIGH, LogicValue::HIGH, LogicValue::LOW},
        {LogicValue::LOW,  LogicValue::HIGH, LogicValue::LOW,  LogicValue::HIGH, LogicValue::LOW},
        {LogicValue::LOW,  LogicValue::HIGH, LogicValue::HIGH, LogicValue::LOW,  LogicValue::HIGH},
        {LogicValue::HIGH, LogicValue::LOW,  LogicValue::LOW,  LogicValue::HIGH, LogicValue::LOW},
        {LogicValue::HIGH, LogicValue::LOW,  LogicValue::HIGH, LogicValue::LOW,  LogicValue::HIGH},
        {LogicValue::HIGH, LogicValue::HIGH, LogicValue::LOW,  LogicValue::LOW,  LogicValue::HIGH},
        {LogicValue::HIGH, LogicValue::HIGH, LogicValue::HIGH, LogicValue::HIGH, LogicValue::HIGH}
    };

    for (const auto& vec : vectors) {
        circuit.setInputValue("A", vec.a);
        circuit.setInputValue("B", vec.b);
        circuit.setInputValue("Cin", vec.cin);

        ASSERT_EQ(circuit.evaluate(), CircuitStatus::OK);
        ASSERT_EQ(circuit.getOutputValue("Sum"), vec.sum);
        ASSERT_EQ(circuit.getOutputValue("Cout"), vec.cout);
    }
}

TEST_CASE(Validation, TestDriverContentionAndSelfLoops) {
    Circuit circuit;

    circuit.addGate(GateType::AND, "G1", 2);
    circuit.addGate(GateType::OR,  "G2", 2);
    circuit.addInput("IN1", "IN1");
    circuit.addInput("IN2", "IN2");

    // Valid connection
    auto res1 = circuit.connect("IN1", 0, "G1", 0);
    ASSERT_TRUE(res1.valid);

    // Driver contention: attempt to connect IN2 to G1 pin 0 (already driven!)
    auto res2 = circuit.connect("IN2", 0, "G1", 0);
    ASSERT_FALSE(res2.valid);

    // Self-loop: connect G1 output to G1 input pin 1
    auto res3 = circuit.connect("G1", 0, "G1", 1);
    ASSERT_FALSE(res3.valid);

    // Duplicate connection check: connect IN1 to G1 pin 0 again
    auto res4 = circuit.connect("IN1", 0, "G1", 0);
    ASSERT_FALSE(res4.valid);
}

TEST_CASE(Validation, TestDisconnectedInputsDetection) {
    Circuit circuit;

    circuit.addInput("IN1", "IN1", LogicValue::HIGH);
    circuit.addGate(GateType::AND, "G1", 2);
    circuit.addOutput("OUT1", "OUT1");

    circuit.connect("IN1", 0, "G1", 0);
    circuit.connect("G1", 0, "OUT1", 0);
    // Notice: G1 pin 1 is intentionally left disconnected!

    CircuitStatus status = circuit.evaluate();
    ASSERT_EQ(status, CircuitStatus::DISCONNECTED_INPUTS);
    // AND gate with High and Undefined evaluates to Undefined
    ASSERT_EQ(circuit.getOutputValue("OUT1"), LogicValue::UNDEFINED);
}

TEST_CASE(Validation, TestCycleAndFeedbackDetection) {
    Circuit circuit;

    // Create a 2-gate feedback loop: G1 -> G2 -> G1
    circuit.addGate(GateType::NOT, "G1", 1);
    circuit.addGate(GateType::NOT, "G2", 1);

    circuit.connect("G1", 0, "G2", 0);
    circuit.connect("G2", 0, "G1", 0); // Feedback cycle!

    std::vector<std::string> cyclePath;
    ASSERT_TRUE(circuit.hasCycles(&cyclePath));
    ASSERT_TRUE(cyclePath.size() >= 2);

    CircuitStatus status = circuit.evaluate();
    ASSERT_EQ(status, CircuitStatus::HAS_CYCLE);
}

TEST_CASE(Persistence, TestSaveLoadRoundTrip) {
    Circuit original;

    original.addInput("IN_A", "Input_A", LogicValue::HIGH, 100.0f, 150.0f);
    original.addInput("IN_B", "Input_B", LogicValue::LOW, 100.0f, 250.0f);
    original.addGate(GateType::XOR, "XOR_1", 2, 300.0f, 200.0f);
    original.addOutput("OUT_Y", "Output_Y", 500.0f, 200.0f);

    original.connect("IN_A", 0, "XOR_1", 0);
    original.connect("IN_B", 0, "XOR_1", 1);
    original.connect("XOR_1", 0, "OUT_Y", 0);

    // Serialize
    std::string serialized = Serializer::serializeToString(original);
    ASSERT_TRUE(!serialized.empty());

    // Deserialize into fresh circuit
    Circuit restored;
    auto res = Serializer::deserializeFromString(restored, serialized);
    ASSERT_TRUE(res.success);

    // Verify component counts
    ASSERT_EQ(restored.getGateCount(), original.getGateCount());
    ASSERT_EQ(restored.getInputs().size(), original.getInputs().size());
    ASSERT_EQ(restored.getOutputs().size(), original.getOutputs().size());
    ASSERT_EQ(restored.getWireCount(), original.getWireCount());

    // Verify truth table on restored circuit
    ASSERT_EQ(restored.evaluate(), CircuitStatus::OK);
    // 1 XOR 0 = 1
    ASSERT_EQ(restored.getOutputValue("OUT_Y"), LogicValue::HIGH);

    // Toggle input and verify propagation
    restored.setInputValue("IN_B", LogicValue::HIGH);
    ASSERT_EQ(restored.evaluate(), CircuitStatus::OK);
    // 1 XOR 1 = 0
    ASSERT_EQ(restored.getOutputValue("OUT_Y"), LogicValue::LOW);
}

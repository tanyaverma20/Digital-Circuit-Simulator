#include "test_framework.hpp"
#include "../engine/LogicGate.hpp"

using namespace eda;

TEST_CASE(GateTruthTables, TestNOTGate) {
    LogicGate notGate("not_1", GateType::NOT, 1);

    notGate.setInputValue(0, LogicValue::LOW);
    ASSERT_EQ(notGate.evaluate(), LogicValue::HIGH);

    notGate.setInputValue(0, LogicValue::HIGH);
    ASSERT_EQ(notGate.evaluate(), LogicValue::LOW);

    notGate.setInputValue(0, LogicValue::UNDEFINED);
    ASSERT_EQ(notGate.evaluate(), LogicValue::UNDEFINED);
}

TEST_CASE(GateTruthTables, TestANDGate) {
    LogicGate andGate("and_1", GateType::AND, 2);

    // 0 & 0 = 0
    andGate.setInputValue(0, LogicValue::LOW);
    andGate.setInputValue(1, LogicValue::LOW);
    ASSERT_EQ(andGate.evaluate(), LogicValue::LOW);

    // 0 & 1 = 0
    andGate.setInputValue(0, LogicValue::LOW);
    andGate.setInputValue(1, LogicValue::HIGH);
    ASSERT_EQ(andGate.evaluate(), LogicValue::LOW);

    // 1 & 0 = 0
    andGate.setInputValue(0, LogicValue::HIGH);
    andGate.setInputValue(1, LogicValue::LOW);
    ASSERT_EQ(andGate.evaluate(), LogicValue::LOW);

    // 1 & 1 = 1
    andGate.setInputValue(0, LogicValue::HIGH);
    andGate.setInputValue(1, LogicValue::HIGH);
    ASSERT_EQ(andGate.evaluate(), LogicValue::HIGH);

    // Dominant 0 with Undefined
    andGate.setInputValue(0, LogicValue::LOW);
    andGate.setInputValue(1, LogicValue::UNDEFINED);
    ASSERT_EQ(andGate.evaluate(), LogicValue::LOW);

    // 1 & Undefined = Undefined
    andGate.setInputValue(0, LogicValue::HIGH);
    andGate.setInputValue(1, LogicValue::UNDEFINED);
    ASSERT_EQ(andGate.evaluate(), LogicValue::UNDEFINED);
}

TEST_CASE(GateTruthTables, TestORGate) {
    LogicGate orGate("or_1", GateType::OR, 2);

    orGate.setInputValue(0, LogicValue::LOW);
    orGate.setInputValue(1, LogicValue::LOW);
    ASSERT_EQ(orGate.evaluate(), LogicValue::LOW);

    orGate.setInputValue(0, LogicValue::LOW);
    orGate.setInputValue(1, LogicValue::HIGH);
    ASSERT_EQ(orGate.evaluate(), LogicValue::HIGH);

    orGate.setInputValue(0, LogicValue::HIGH);
    orGate.setInputValue(1, LogicValue::LOW);
    ASSERT_EQ(orGate.evaluate(), LogicValue::HIGH);

    orGate.setInputValue(0, LogicValue::HIGH);
    orGate.setInputValue(1, LogicValue::HIGH);
    ASSERT_EQ(orGate.evaluate(), LogicValue::HIGH);

    // Dominant 1 with Undefined
    orGate.setInputValue(0, LogicValue::HIGH);
    orGate.setInputValue(1, LogicValue::UNDEFINED);
    ASSERT_EQ(orGate.evaluate(), LogicValue::HIGH);

    // 0 | Undefined = Undefined
    orGate.setInputValue(0, LogicValue::LOW);
    orGate.setInputValue(1, LogicValue::UNDEFINED);
    ASSERT_EQ(orGate.evaluate(), LogicValue::UNDEFINED);
}

TEST_CASE(GateTruthTables, TestNANDGate) {
    LogicGate nandGate("nand_1", GateType::NAND, 2);

    nandGate.setInputValue(0, LogicValue::LOW);
    nandGate.setInputValue(1, LogicValue::LOW);
    ASSERT_EQ(nandGate.evaluate(), LogicValue::HIGH);

    nandGate.setInputValue(0, LogicValue::LOW);
    nandGate.setInputValue(1, LogicValue::HIGH);
    ASSERT_EQ(nandGate.evaluate(), LogicValue::HIGH);

    nandGate.setInputValue(0, LogicValue::HIGH);
    nandGate.setInputValue(1, LogicValue::LOW);
    ASSERT_EQ(nandGate.evaluate(), LogicValue::HIGH);

    nandGate.setInputValue(0, LogicValue::HIGH);
    nandGate.setInputValue(1, LogicValue::HIGH);
    ASSERT_EQ(nandGate.evaluate(), LogicValue::LOW);
}

TEST_CASE(GateTruthTables, TestNORGate) {
    LogicGate norGate("nor_1", GateType::NOR, 2);

    norGate.setInputValue(0, LogicValue::LOW);
    norGate.setInputValue(1, LogicValue::LOW);
    ASSERT_EQ(norGate.evaluate(), LogicValue::HIGH);

    norGate.setInputValue(0, LogicValue::LOW);
    norGate.setInputValue(1, LogicValue::HIGH);
    ASSERT_EQ(norGate.evaluate(), LogicValue::LOW);

    norGate.setInputValue(0, LogicValue::HIGH);
    norGate.setInputValue(1, LogicValue::LOW);
    ASSERT_EQ(norGate.evaluate(), LogicValue::LOW);

    norGate.setInputValue(0, LogicValue::HIGH);
    norGate.setInputValue(1, LogicValue::HIGH);
    ASSERT_EQ(norGate.evaluate(), LogicValue::LOW);
}

TEST_CASE(GateTruthTables, TestXORGate) {
    LogicGate xorGate("xor_1", GateType::XOR, 2);

    xorGate.setInputValue(0, LogicValue::LOW);
    xorGate.setInputValue(1, LogicValue::LOW);
    ASSERT_EQ(xorGate.evaluate(), LogicValue::LOW);

    xorGate.setInputValue(0, LogicValue::LOW);
    xorGate.setInputValue(1, LogicValue::HIGH);
    ASSERT_EQ(xorGate.evaluate(), LogicValue::HIGH);

    xorGate.setInputValue(0, LogicValue::HIGH);
    xorGate.setInputValue(1, LogicValue::LOW);
    ASSERT_EQ(xorGate.evaluate(), LogicValue::HIGH);

    xorGate.setInputValue(0, LogicValue::HIGH);
    xorGate.setInputValue(1, LogicValue::HIGH);
    ASSERT_EQ(xorGate.evaluate(), LogicValue::LOW);
}

TEST_CASE(GateTruthTables, TestXNORGate) {
    LogicGate xnorGate("xnor_1", GateType::XNOR, 2);

    xnorGate.setInputValue(0, LogicValue::LOW);
    xnorGate.setInputValue(1, LogicValue::LOW);
    ASSERT_EQ(xnorGate.evaluate(), LogicValue::HIGH);

    xnorGate.setInputValue(0, LogicValue::LOW);
    xnorGate.setInputValue(1, LogicValue::HIGH);
    ASSERT_EQ(xnorGate.evaluate(), LogicValue::LOW);

    xnorGate.setInputValue(0, LogicValue::HIGH);
    xnorGate.setInputValue(1, LogicValue::LOW);
    ASSERT_EQ(xnorGate.evaluate(), LogicValue::LOW);

    xnorGate.setInputValue(0, LogicValue::HIGH);
    xnorGate.setInputValue(1, LogicValue::HIGH);
    ASSERT_EQ(xnorGate.evaluate(), LogicValue::HIGH);
}

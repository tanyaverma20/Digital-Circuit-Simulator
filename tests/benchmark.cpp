#include "../engine/Circuit.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>

using namespace eda;

int main() {
    std::cout << "========================================================\n";
    std::cout << "  DIGITAL CIRCUIT SIMULATOR - PERFORMANCE BENCHMARK\n";
    std::cout << "========================================================\n";

    // 1. Scalability Benchmark: Deep Logic Chain (Depth Stress Test)
    const size_t CHAIN_DEPTH = 1000;
    std::cout << "\n[1] Constructing Deep Logic Inverter Chain (Depth: " << CHAIN_DEPTH << " gates)...\n";
    
    Circuit chainCircuit;
    chainCircuit.addInput("in_0", "IN0", LogicValue::LOW);

    for (size_t i = 0; i < CHAIN_DEPTH; ++i) {
        std::string gId = "not_" + std::to_string(i);
        chainCircuit.addGate(GateType::NOT, gId, 1);
        if (i == 0) {
            chainCircuit.connect("in_0", 0, gId, 0);
        } else {
            std::string prevId = "not_" + std::to_string(i - 1);
            chainCircuit.connect(prevId, 0, gId, 0);
        }
    }
    std::string lastGate = "not_" + std::to_string(CHAIN_DEPTH - 1);
    chainCircuit.addOutput("out_final", "OUT");
    chainCircuit.connect(lastGate, 0, "out_final", 0);

    // Warm-up
    chainCircuit.evaluate();

    // Measure evaluation time over multiple runs
    const int RUNS = 20;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int r = 0; r < RUNS; ++r) {
        chainCircuit.setInputValue("in_0", (r % 2 == 0) ? LogicValue::HIGH : LogicValue::LOW);
        CircuitStatus status = chainCircuit.evaluate();
        if (status != CircuitStatus::OK) {
            std::cerr << "Evaluation failed!\n";
            return 1;
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    double totalTimeMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
    double avgTimePerPassMs = totalTimeMs / RUNS;
    double totalGateEvals = static_cast<double>(CHAIN_DEPTH) * RUNS;
    double throughputGatesPerSec = (totalGateEvals / (totalTimeMs / 1000.0));
    double latencyPerGateNs = (avgTimePerPassMs * 1e6) / CHAIN_DEPTH;

    std::cout << "  - Total Gates in DAG:      " << chainCircuit.getGateCount() << "\n";
    std::cout << "  - Total Wires in Netlist:  " << chainCircuit.getWireCount() << "\n";
    std::cout << "  - Circuit Depth:           " << CHAIN_DEPTH << " levels\n";
    std::cout << "  - Average Eval Latency:    " << std::fixed << std::setprecision(3) << avgTimePerPassMs << " ms / pass\n";
    std::cout << "  - Per-Gate Latency:        " << std::fixed << std::setprecision(1) << latencyPerGateNs << " ns / gate\n";
    std::cout << "  - Simulation Throughput:   " << std::fixed << std::setprecision(2) << (throughputGatesPerSec / 1e6) << " Million gate-evals/sec\n";
    std::cout << "  - Expected Output:         " << ((CHAIN_DEPTH % 2 == 1) ? "INVERTED" : "SAME") << " -> Verified: "
              << toChar(chainCircuit.getOutputValue("out_final")) << "\n";

    // 2. Wide Multi-Gate Topology Benchmark: 16-bit Full Adder / Ripple-Carry Structure
    const size_t NUM_ADDERS = 50; // 50 Full Adders = 250 gates
    std::cout << "\n[2] Constructing Multi-Level Ripple-Carry Circuit (" << NUM_ADDERS << " Full Adders = " << (NUM_ADDERS * 5) << " gates)...\n";

    Circuit rca;
    rca.addInput("cin_0", "CIN0", LogicValue::LOW);

    for (size_t i = 0; i < NUM_ADDERS; ++i) {
        std::string aId = "a_" + std::to_string(i);
        std::string bId = "b_" + std::to_string(i);
        rca.addInput(aId, aId, LogicValue::HIGH);
        rca.addInput(bId, bId, (i % 2 == 0) ? LogicValue::LOW : LogicValue::HIGH);

        std::string xor1 = "xor1_" + std::to_string(i);
        std::string xor2 = "xor2_" + std::to_string(i);
        std::string and1 = "and1_" + std::to_string(i);
        std::string and2 = "and2_" + std::to_string(i);
        std::string or1  = "or1_" + std::to_string(i);

        rca.addGate(GateType::XOR, xor1, 2);
        rca.addGate(GateType::XOR, xor2, 2);
        rca.addGate(GateType::AND, and1, 2);
        rca.addGate(GateType::AND, and2, 2);
        rca.addGate(GateType::OR,  or1,  2);

        rca.connect(aId, 0, xor1, 0);
        rca.connect(bId, 0, xor1, 1);
        rca.connect(aId, 0, and2, 0);
        rca.connect(bId, 0, and2, 1);

        std::string cinSrc = (i == 0) ? "cin_0" : ("or1_" + std::to_string(i - 1));
        rca.connect(xor1, 0, xor2, 0);
        rca.connect(cinSrc, 0, xor2, 1);

        rca.connect(xor1, 0, and1, 0);
        rca.connect(cinSrc, 0, and1, 1);

        rca.connect(and1, 0, or1, 0);
        rca.connect(and2, 0, or1, 1);

        std::string sumOut = "sum_" + std::to_string(i);
        rca.addOutput(sumOut, sumOut);
        rca.connect(xor2, 0, sumOut, 0);
    }
    std::string coutFinal = "cout_final";
    rca.addOutput(coutFinal, coutFinal);
    rca.connect("or1_" + std::to_string(NUM_ADDERS - 1), 0, coutFinal, 0);

    const int RCA_RUNS = 50;
    auto tRca0 = std::chrono::high_resolution_clock::now();
    for (int r = 0; r < RCA_RUNS; ++r) {
        rca.setInputValue("cin_0", (r % 2 == 0) ? LogicValue::LOW : LogicValue::HIGH);
        rca.evaluate();
    }

    auto tRca1 = std::chrono::high_resolution_clock::now();
    double rcaTimeMs = std::chrono::duration<double, std::milli>(tRca1 - tRca0).count();
    double avgRcaTimeMs = rcaTimeMs / RCA_RUNS;
    double rcaThroughput = ((NUM_ADDERS * 5.0) * RCA_RUNS) / (rcaTimeMs / 1000.0);

    std::cout << "  - Total Gates in Circuit:  " << rca.getGateCount() << "\n";
    std::cout << "  - Total Wires in Netlist:  " << rca.getWireCount() << "\n";
    std::cout << "  - Logic Depth:             " << (NUM_ADDERS * 2 + 1) << " gate levels\n";
    std::cout << "  - Average Eval Latency:    " << std::fixed << std::setprecision(3) << avgRcaTimeMs << " ms / pass\n";
    std::cout << "  - Throughput:              " << std::fixed << std::setprecision(2) << (rcaThroughput / 1e6) << " Million gate-evals/sec\n";
    std::cout << "  - Final Carry-Out:         " << toChar(rca.getOutputValue(coutFinal)) << "\n";

    std::cout << "\n========================================================\n";
    std::cout << "  BENCHMARK COMPLETED SUCCESSFULLY\n";
    std::cout << "========================================================\n";
    return 0;
}

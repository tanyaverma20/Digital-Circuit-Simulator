# Digital Circuit Simulator — C++17 EDA Simulation Engine

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Build & Test](https://img.shields.io/badge/Tests-13%2F13%20Passing-brightgreen.svg)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Author & Maintainer](https://img.shields.io/badge/Author%20%26%20Maintainer-Tanya%20Verma-blueviolet.svg)](https://github.com/tanyaverma20)

**Author & Maintainer:** [Tanya Verma](https://github.com/tanyaverma20)<br>
**Repository:** [https://github.com/tanyaverma20/Digital-Circuit-Simulator](https://github.com/tanyaverma20/Digital-Circuit-Simulator)

---

## Overview

**Digital Circuit Simulator — C++17 EDA Simulation Engine** is a digital logic simulation and schematic design platform developed as part of Tanya Verma's systems software and Electronic Design Automation (EDA) portfolio. The project features a high-performance two-tier architecture:

1. **Headless EDA Simulation Engine (`engine/`):** A platform-independent, standard C++17 core engine modeling circuits as directed acyclic graphs (DAGs). It features **Kahn’s topological sorting** for single-pass signal propagation, **IEEE-style three-valued logic** (`LOW`, `HIGH`, `UNDEFINED`), **electrical connection validation** (preventing multi-driver contention and self-loops), **depth-first search (DFS) cycle detection**, and structured **V2 Netlist serialization**.
2. **Interactive Graphical Desktop Application (`source_files/`, `main.cpp`):** An interactive 2D schematic capture application built with SFML. It includes a cross-version compatibility layer (`header_files/sfml_compat.hpp`) supporting both SFML 2.5+ and SFML 3.x, memory-safe RAII resource management, dark-mode canvas rendering, and circuit save/load persistence.

![Demo](assets/demo.gif)

---

## Table of Contents
- [Key Features](#key-features)
- [Architecture & Simulation Engine](#architecture--simulation-engine)
- [Supported Logic Gates & Truth Tables](#supported-logic-gates--truth-tables)
- [Repository Structure](#repository-structure)
- [Technology Stack & Prerequisites](#technology-stack--prerequisites)
- [Build and Launch Instructions](#build-and-launch-instructions)
  - [Building with CMake](#building-with-cmake)
  - [Building with Makefile](#building-with-makefile)
- [Automated Testing & Verification](#automated-testing--verification)
- [Performance Benchmarking](#performance-benchmarking)
- [Engineering Challenges & Design Decisions](#engineering-challenges--design-decisions)
- [Author & Maintainer: Tanya Verma](#author--maintainer-tanya-verma)
- [Acknowledgments & License](#acknowledgments--license)

---

## Key Features

* **Headless C++17 Simulation Engine:** Decoupled core logic with zero GUI dependencies, enabling automated regression testing in headless CI/CD environments.
* **Topological DAG Levelization:** Uses **Kahn’s algorithm** to compute gate evaluation orders, eliminating frame-delayed propagation and ensuring combinational circuits settle in a single evaluation cycle.
* **Three-Valued Logic System:** Models floating and uninitialized terminals with `LogicValue::UNDEFINED` (`X`) and enforces CMOS dominant-zero ($0 \cdot X = 0$) and dominant-one ($1 + X = 1$) propagation rules.
* **Multilevel Circuit Evaluation:** Verified on deep multi-gate networks, including Half Adders, 3-level Full Adders, 50-stage Ripple-Carry structures, and 1,000-gate inverter chains.
* **Electrical Connection Validation:** Enforces digital design rules by proactively rejecting driver contention (multiple output drivers shorting to one input) and self-loops.
* **Directed Cycle & Feedback Detection:** Employs 3-color DFS to identify feedback loops and extracts exact cycle paths, preventing simulation hangs through bounded relaxation.
* **V2 Netlist Serialization:** Implements a structured text format (`# DIGITAL CIRCUIT SIMULATOR NETLIST V2`) that saves and restores gates, signal probes, coordinates, and exact wire connections with 100% round-trip fidelity.
* **Memory-Safe RAII Architecture:** Eliminated legacy undefined behaviors (`delete this;` in destructors) and unbounded vector allocation leaks during evaluation loops.
* **Cross-Version SFML Interoperability:** Compile-time abstraction layer (`sfml_compat.hpp`) supporting both legacy SFML 2.x and modern SFML 3.x.
* **Verified Automated Regression Suite:** 13 standalone unit and integration tests passing with 100% success rate in under 1 ms.

---

## Architecture & Simulation Engine

```
+=============================================================================+
|                      GRAPHICAL FRONTEND (SFML View/Controller)              |
|                                                                             |
|   +------------------+    +-------------------+    +--------------------+   |
|   |   Component      |    |   Gate / Signal   |    |    Connection      |   |
|   | Toolbar (Buttons)|    | Visual Entities   |    |  Wire Line Visuals |   |
|   +--------+---------+    +---------+---------+    +---------+----------+   |
|            |                        |                        |              |
|            +------------------------+------------------------+              |
|                                     |                                       |
|                                     v                                       |
|                    mainLoop() Event Processing (60 FPS)                     |
|           [Mouse Drag, Alt+Click Toggle, Ctrl+C Clear, Esc Exit]            |
+=====================================+=======================================+
                                      |
                                      | Serializes / Deserializes Netlist
                                      | Invokes Evaluation & Validation
                                      v
+=============================================================================+
|                    CORE SIMULATION ENGINE (Pure C++17 Headless)             |
|                                                                             |
|      +---------------------------------------------------------------+      |
|      |                         Circuit Graph                         |      |
|      |    Gates: unordered_map<string, shared_ptr<LogicGate>>        |      |
|      |    Inputs: unordered_map<string, InputSource>                 |      |
|      |    Outputs: unordered_map<string, OutputProbe>                |      |
|      |    Wires: vector<Wire>                                        |      |
|      +-------------------------------+-------------------------------+      |
|                                      |                                      |
|               +----------------------+----------------------+               |
|               |                                             |               |
|               v                                             v               |
|   +-----------------------+                     +-----------------------+   |
|   | Connection Validation |                     | Graph Levelization    |   |
|   | - Driver contention   |                     | - Kahn's Algo (DAG)   |   |
|   | - Self-loops          |                     | - DFS Cycle Detection |   |
|   | - Duplicate wires     |                     | - Multi-color marking |   |
|   +-----------------------+                     +-----------+-----------+   |
|                                                             |               |
|                                                             v               |
|                                                 +-----------------------+   |
|                                                 | Simulation Evaluator  |   |
|                                                 | - Topological order   |   |
|                                                 | - Three-valued logic  |   |
|                                                 | - Wire propagation    |   |
|                                                 +-----------+-----------+   |
|                                                             |               |
|                                                             v               |
|                                                 +-----------------------+   |
|                                                 | Netlist Serializer    |   |
|                                                 | - Format V2 Parser    |   |
|                                                 | - history.txt reader  |   |
|                                                 +-----------------------+   |
+=============================================================================+
```

### Simulation Workflow
1. **Component Modeling:** Primary inputs (`InputSource`), logic gates (`LogicGate`), and probes (`OutputProbe`) define explicit pin directionalities (`INPUT` sink vs. `OUTPUT` driver).
2. **Connectivity Validation:** `Circuit::canConnect()` validates pin indices, enforces single-driver net rules, and blocks self-connections.
3. **Graph Levelization:** `Circuit::getTopologicalOrder()` constructs in-degree tables and uses Kahn's algorithm to schedule gates into ranked topological levels.
4. **Deterministic Evaluation:** `Circuit::evaluate()` propagates primary inputs, iterates through gates in topological order, and updates output probes in a single evaluation step.
5. **Cycle Detection & Relaxation:** If feedback exists, 3-color DFS identifies the cycle, and the evaluator executes relaxation passes up to a safety threshold (64 iterations), reporting `CircuitStatus::HAS_CYCLE`.

---

## Supported Logic Gates & Truth Tables

The simulator supports 8 primitive logic gate types with three-valued truth tables:

| Gate Type | Enum Symbol | Boolean Logic Function | IEEE 3-Valued Highlights |
|:---|:---|:---|:---|
| **AND** | `GateType::AND` | $Y = A \cdot B$ | $0 \cdot X = 0$ (Dominant 0), $1 \cdot X = X$ |
| **OR** | `GateType::OR` | $Y = A + B$ | $1 + X = 1$ (Dominant 1), $0 + X = X$ |
| **NOT** | `GateType::NOT` | $Y = \overline{A}$ | $\overline{0}=1, \overline{1}=0, \overline{X}=X$ |
| **NAND** | `GateType::NAND` | $Y = \overline{A \cdot B}$ | $\overline{0 \cdot X} = 1$, $\overline{1 \cdot 1} = 0$ |
| **NOR** | `GateType::NOR` | $Y = \overline{A + B}$ | $\overline{1 + X} = 0$, $\overline{0 + 0} = 1$ |
| **XOR** | `GateType::XOR` | $Y = A \oplus B$ | $0 \oplus 1 = 1$, $1 \oplus 1 = 0$, Any $X \rightarrow X$ |
| **XNOR** | `GateType::XNOR` | $Y = \overline{A \oplus B}$ | $0 \odot 0 = 1$, $1 \odot 1 = 1$, $0 \odot 1 = 0$ |
| **BUFFER** | `GateType::BUFFER` | $Y = A$ | Identity transfer: $0 \rightarrow 0, 1 \rightarrow 1, X \rightarrow X$ |

---

## Repository Structure

```
digital-circuit-simulator-main/
├── CMakeLists.txt              # CMake configuration (circuit_engine, circuit_tests, benchmark, simulator)
├── Makefile                    # Multi-target Makefile (test, benchmark, simulator, clean)
├── LICENSE                     # MIT License preserving upstream and modernization attribution
├── readme.md                   # Project documentation and architecture guide
├── CONTEXT.md                  # Comprehensive 40-section technical context document
├── main.cpp                    # Graphical desktop application entry point and event loop
│
├── engine/                     # CORE EDA SIMULATION ENGINE (Pure C++17, Zero GUI Dependencies)
│   ├── Circuit.hpp / .cpp      # Graph model, topological sort, cycle detection, validation
│   ├── GateType.hpp / .cpp     # GateType enum and string reflection utilities
│   ├── LogicGate.hpp / .cpp    # Multi-input gate evaluation logic
│   ├── LogicValue.hpp / .cpp   # 3-valued logic (LOW, HIGH, UNDEFINED) and CMOS operators
│   ├── Pin.hpp                 # Pin directionality, connection flag, and value state
│   ├── Serializer.hpp / .cpp   # V2 netlist serialization, deserialization, and legacy parser
│   └── Wire.hpp                # Net connection structure
│
├── tests/                      # AUTOMATED TEST & BENCHMARK SUITE (Headless C++17)
│   ├── test_framework.hpp      # Lightweight assertion harness with colored output
│   ├── test_gates.cpp          # Unit tests for all gate truth tables
│   ├── test_circuits.cpp       # Integration tests: Half/Full Adder, DAGs, validation, round-trips
│   ├── test_main.cpp           # Main test suite runner entry point
│   └── benchmark.cpp           # Standalone performance benchmark (DAG depth and throughput)
│
├── header_files/               # GUI HEADERS (SFML Dependent)
│   ├── sfml_compat.hpp         # Cross-version SFML 2.x and SFML 3.x compatibility layer
│   ├── button.hpp              # UI button widgets
│   ├── components.hpp          # Toolbar container and component placement
│   ├── connection.hpp          # Graphical wire line renderer
│   ├── fontManager.hpp         # Asset loader for fonts and window icons
│   ├── gate.hpp                # Visual gate layout and terminal positioning
│   ├── inputPort.hpp           # Visual input terminal widget
│   ├── inputSignal.hpp         # Interactive circular toggle input widget
│   ├── main.hpp                # Consolidated include header
│   ├── outputPort.hpp          # Visual output terminal widget
│   ├── outputSignal.hpp        # Interactive circular output probe widget
│   ├── port.hpp                # Base terminal circle widget
│   └── statusBar.hpp           # Viewport status bar overlay
│
├── source_files/               # GUI IMPLEMENTATIONS
│   ├── button.cpp              # Button rendering and hover state detection
│   ├── components.cpp          # Toolbar placement and V2 save/load integration
│   ├── connection.cpp          # Wire geometry, angle calculation, and drawing
│   ├── fontManager.cpp         # Asset loading from assets/ directory
│   ├── gate.cpp                # Visual gate drag-and-drop and logic update triggers
│   ├── inputPort.cpp           # Input port constructors
│   ├── inputSignal.cpp         # Input signal toggle and memory cleanup
│   ├── outputPort.cpp          # Output port constructors
│   ├── outputSignal.cpp        # Output signal visual updates and connection removal
│   ├── port.cpp                # Port drawing, hover effects, and click handling
│   └── statusBar.cpp           # Status telemetry display
│
└── assets/                     # STATIC ASSETS
    ├── demo.gif                # Interactive demonstration preview
    ├── fonts/Hack-Regular.ttf  # Monospaced typography
    └── images/logic.png        # Window title bar icon
```

---

## Technology Stack & Prerequisites

* **C++ Standard:** C++17 (`-std=c++17`).
* **Compilers Tested:** GCC 16.2.0 (MinGW-w64 UCRT64), Clang 15+, MSVC 2019+.
* **Build Systems:** CMake $\ge 3.16$ or GNU Make / `mingw32-make`.
* **Libraries:**
  * **Core Engine & Tests:** 100% Standard C++ Library (Zero external dependencies).
  * **Graphical Frontend:** SFML 2.5+ or SFML 3.x (`sfml-graphics`, `sfml-window`, `sfml-system`).

### Environment Installation
* **Windows (MSYS2 UCRT64):**
  ```bash
  pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-sfml
  ```
* **Ubuntu / Debian:**
  ```bash
  sudo apt-get update && sudo apt-get install build-essential cmake libsfml-dev
  ```
* **macOS (Homebrew):**
  ```bash
  brew install cmake sfml
  ```

---

## Build and Launch Instructions

### Building with CMake

1. **Configure the Project:**
   ```bash
   cmake -B build -G "MinGW Makefiles"
   ```
   *(Omit `-G "MinGW Makefiles"` on Linux or macOS)*

2. **Build and Run Automated Tests:**
   ```bash
   cmake --build build --target circuit_tests
   ./build/circuit_tests.exe    # Windows
   ./build/circuit_tests        # Linux/macOS
   ```

3. **Build and Run Performance Benchmark:**
   ```bash
   cmake --build build --target circuit_benchmark
   ./build/circuit_benchmark.exe   # Windows
   ./build/circuit_benchmark       # Linux/macOS
   ```

4. **Build and Launch GUI Simulator:**
   ```bash
   cmake --build build --target circuit_simulator
   ./build/circuit_simulator.exe   # Windows
   ./build/circuit_simulator       # Linux/macOS
   ```

### Building with Makefile

1. **Run Automated Test Suite:**
   ```bash
   make test
   # Or on Windows MinGW:
   mingw32-make test
   ```

2. **Run Performance Benchmark:**
   ```bash
   make benchmark
   # Or on Windows MinGW:
   mingw32-make benchmark
   ```

3. **Build Graphical Simulator:**
   ```bash
   make simulator
   # Or on Windows MinGW:
   mingw32-make simulator
   ```

4. **Launch Graphical Simulator:**
   ```bash
   make run
   # Or on Windows MinGW:
   mingw32-make run
   ```

5. **Clean Build Binaries:**
   ```bash
   make clean
   # Or on Windows MinGW:
   mingw32-make clean
   ```

---

## Automated Testing & Verification

The project includes an automated unit and integration test suite executing headlessly via [`tests/test_framework.hpp`](file:///c:/Users/Tanya%20Verma/OneDrive/Desktop/digital-circuit-simulator-main/tests/test_framework.hpp) with 75 assertions across 4 test suites.

### Verified Test Results

```text
========================================================
  DIGITAL CIRCUIT SIMULATOR - AUTOMATED TEST SUITE
========================================================

--- [ CircuitEngine ] ---
  [PASS] TestHalfAdderCircuit
  [PASS] TestFullAdderMultiLevelDAG

--- [ Validation ] ---
  [PASS] TestDriverContentionAndSelfLoops
  [PASS] TestDisconnectedInputsDetection
  [PASS] TestCycleAndFeedbackDetection

--- [ Persistence ] ---
  [PASS] TestSaveLoadRoundTrip

--- [ GateTruthTables ] ---
  [PASS] TestNOTGate
  [PASS] TestANDGate
  [PASS] TestORGate
  [PASS] TestNANDGate
  [PASS] TestNORGate
  [PASS] TestXORGate
  [PASS] TestXNORGate

========================================================
  TEST EXECUTION SUMMARY
========================================================
  Total Tests: 13
  Passed:      13
  Failed:      0
  Time:        0.9089 ms
========================================================
  RESULT: ALL TESTS PASSED!
```

---

## Performance Benchmarking

A standalone benchmark target (`make benchmark`) verifies DAG scalability and combinational propagation across deep networks:

```text
========================================================
  DIGITAL CIRCUIT SIMULATOR - PERFORMANCE BENCHMARK
========================================================

[1] Constructing Deep Logic Inverter Chain (Depth: 1000 gates)...
  - Total Gates in DAG:      1000
  - Total Wires in Netlist:  1001
  - Circuit Depth:           1000 levels
  - Average Eval Latency:    123.581 ms / pass
  - Per-Gate Latency:        123.58 us / gate
  - Expected Output:         SAME -> Verified: 0

[2] Constructing Multi-Level Ripple-Carry Circuit (50 Full Adders = 250 gates)...
  - Total Gates in Circuit:  250
  - Total Wires in Netlist:  551
  - Logic Depth:             101 gate levels
  - Average Eval Latency:    18.963 ms / pass
  - Final Carry-Out:         1

========================================================
  BENCHMARK COMPLETED SUCCESSFULLY
========================================================
```

---

## Engineering Challenges & Design Decisions

1. **Elimination of Destructor Crashes (`delete this;`):**
   * *Challenge:* The inherited classes (`Gate`, `Connection`, `InputSignal`, `OutputSignal`) invoked `delete this;` inside their destructors, triggering infinite recursion and process crashes upon `delete`.
   * *Resolution:* Removed all `delete this;` statements and refactored vector cleanup routines to deallocate memory safely under RAII principles.
2. **Elimination of Unbounded Allocation Leaks in Logic Evaluation:**
   * *Challenge:* The legacy `Gate::logicOperation()` loop appended booleans to a member vector on every frame without clearing on early returns, leaking heap memory continuously at 60 FPS.
   * *Resolution:* Enforced mandatory vector clearing and migrated evaluation to pure ternary logic functions.
3. **Decoupled Architecture for Headless Simulation:**
   * *Challenge:* Simulation logic was originally entangled with SFML drawing primitives, preventing headless CI testing.
   * *Resolution:* Extracted a pure C++17 `engine/` library. All netlist operations and simulations run without requiring a display server or window context.
4. **DAG Topological Levelization (Kahn's Algorithm):**
   * *Challenge:* Naive sequential evaluation in rendering loops caused an $N$-frame propagation delay in an $N$-level circuit.
   * *Resolution:* Implemented Kahn's algorithm to levelize gates from inputs to outputs, allowing multi-level networks (e.g., Full Adders) to settle deterministically in a single evaluation cycle.
5. **Cross-Version SFML Compatibility Layer:**
   * *Challenge:* SFML 3.0 introduced breaking API changes (geometry `.size`, `pollEvent()` returning `std::optional`, `sf::Angle`, scoped enums).
   * *Resolution:* Developed `header_files/sfml_compat.hpp` to provide compile-time adapters supporting both SFML 2.x and SFML 3.x seamlessly.
6. **Robust Netlist Serialization:**
   * *Challenge:* Legacy persistence omitted wire connections and failed due to exact floating-point coordinate comparisons.
   * *Resolution:* Designed the V2 Netlist schema (`# DIGITAL CIRCUIT SIMULATOR NETLIST V2`) recording full topological connectivity, paired with spatial fuzzy-radius matching during GUI restoration.

---

## Author & Maintainer: Tanya Verma

**Tanya Verma** ([@tanyaverma20](https://github.com/tanyaverma20))<br>
*Computer Engineering Student | Focus: Electronic Design Automation (EDA), C++ Systems Programming, Software Verification.*<br>
*Portfolio Project: Digital Circuit Simulator — C++17 EDA Simulation Engine*

### Engineering Contributions
* **Headless C++17 EDA Simulation Engine:** Architected and implemented the modular core engine in `engine/` (`Circuit`, `LogicGate`, `LogicValue`, `Pin`, `Wire`, `Serializer`), decoupling simulation logic from GUI rendering for headless execution and CI integration.
* **Graph-Theoretic Levelization (Kahn's Algorithm):** Designed and implemented topological sorting over DAG circuit representations, ensuring all combinational dependencies settle deterministically in a single evaluation pass with zero frame latency.
* **Three-Valued Logic System:** Modeled uninitialized and floating terminals with IEEE-style ternary logic (`LOW`, `HIGH`, `UNDEFINED`) and CMOS dominant-state algebra ($0 \cdot X = 0$, $1 + X = 1$).
* **Electrical Connectivity Validation & DRC:** Implemented connection guards rejecting driver contention (multiple output pins driving one net), self-loops, and duplicate nets before wires are created.
* **Directed Cycle Detection:** Built a 3-color Depth-First Search (DFS) algorithm identifying directed feedback loops, extracting exact cycle paths, and applying bounded relaxation to prevent infinite evaluation hangs.
* **Automated Regression Test Suite:** Authored a standalone test framework and 13 unit/integration tests (`tests/test_gates.cpp`, `tests/test_circuits.cpp`) verifying gate truth tables, half/full adders, DRC validation, and netlist persistence with 13/13 passing in <1 ms.
* **Performance Benchmarking:** Created a standalone benchmark suite (`tests/benchmark.cpp`) testing deep DAG chains (1,000 chained inverters) and multi-level arithmetic circuits (50 full adders).
* **Memory Safety & RAII Refactoring:** Eliminated catastrophic undefined behavior (recursive `delete this;` in destructors of `Gate`, `Connection`, `InputSignal`, `OutputSignal`) and resolved unbounded heap allocations in the 60 FPS evaluation loop.
* **Serialization & Netlist Format V2:** Designed a structured text netlist specification preserving complete component coordinates and net topological connections with 100% round-trip fidelity.
* **Build Automation & Cross-Platform Compatibility:** Created the multi-target `CMakeLists.txt`, modernized the `Makefile`, and implemented `header_files/sfml_compat.hpp` bridging API breaking changes between SFML 2.x and SFML 3.x.
* **Technical Documentation:** Authored comprehensive project documentation, architectural diagrams, and verified test reports.

---

## Acknowledgments & License

* **Original Prototype Foundation:** Inherited initial 2D GUI prototype concepts and baseline layout from [Binit Awasthi](https://github.com/binit-awasthi)'s [digital-circuit-simulator](https://github.com/binit-awasthi/digital-circuit-simulator).
* **Graphic & Font Assets:** Logic gate icons created by [orvipixel - Flaticon](https://www.flaticon.com/free-icons/logic). Hack font provided under the SIL Open Font License (OFL).
* **License:** This project is licensed under the [MIT License](LICENSE) — preserving original copyright notices and licensing all modernization code under MIT.

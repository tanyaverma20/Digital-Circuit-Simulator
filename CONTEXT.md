# Complete Project Context Document: Digital Circuit Simulator

---

## 1. Project Overview

### Project Name
**Digital Circuit Simulator — C++17 EDA Simulation Engine**  
**Maintainer & Modernization Author:** Tanya Verma (https://github.com/tanyaverma20/Digital-Circuit-Simulator)  
**Upstream Prototype Baseline:** Binit Awasthi (https://github.com/binit-awasthi/digital-circuit-simulator)

### One-Sentence Explanation
A modern C++17 digital logic simulation platform featuring a headless, zero-dependency Electronic Design Automation (EDA) engine with Kahn's topological DAG sorting and three-valued logic evaluation, coupled with an interactive desktop graphical interface powered by SFML.

### Interview-Ready Explanation
"This project is a high-performance digital circuit simulator designed for EDA software engineering. It decouples core digital logic modeling from graphical rendering by implementing a pure C++17 netlist simulation engine. The core engine models digital circuits as directed acyclic graphs (DAGs), performs topological levelization using Kahn’s algorithm to guarantee zero-frame propagation delay, evaluates dominant CMOS behavior using IEEE-style three-valued logic (`LOW`, `HIGH`, `UNDEFINED`), detects feedback cycles via depth-first search, and validates electrical connectivity to eliminate multi-driver contention. The system includes an interactive SFML desktop frontend with cross-version compatibility (SFML 2.5 through 3.0.2), full netlist serialization, and an automated regression test suite covering gate truth tables, multi-level adders, and error handling."

### Detailed Technical Explanation
The Digital Circuit Simulator is an interactive CAD/EDA software tool for digital hardware design and schematic capture. Originally derived from an open-source graphical prototype by Binit Awasthi, the codebase underwent architectural modernization to eliminate critical memory corruption bugs (including recursive `delete this;` invocations in destructors), unbounded vector allocation leaks during evaluation loops, and non-deterministic array-order simulation delays. 

The software is structured into two distinct architectural layers:
1. **Core EDA Simulation Engine (`engine/`):** A headless, platform-independent, standard C++17 library. It manages component instances (`LogicGate`), net connection endpoints (`Pin`, `Wire`), primary stimulus inputs (`InputSource`), and observation probes (`OutputProbe`). Circuits are represented as directed graphs where nodes are components and directed edges are nets. The engine validates connections against driver contention (short circuits) and self-loops, computes topological levels using Kahn's algorithm, detects feedback cycles using depth-first search (DFS) with three-color node marking, evaluates signals deterministically in topological order, and serializes/deserializes the netlist to a robust text-based specification.
2. **Interactive Graphical User Interface (`source_files/`, `header_files/`, `main.cpp`):** A 60-FPS desktop application written with the Simple and Fast Multimedia Library (SFML). It renders logic gates, handles mouse-driven drag-and-drop placement, interactive wire drawing, component duplication, deletion, and real-time canvas feedback. A dedicated cross-version compatibility layer (`sfml_compat.hpp`) abstracts differences between legacy SFML 2.x and modern SFML 3.x.

### Target Users
* **Computer Engineering & Electrical Engineering Students:** Learning digital logic design, Boolean algebra, CMOS truth tables, combinational logic synthesis, and computer architecture fundamentals (adders, multiplexers, decoders).
* **EDA Software Engineers & Interviewers:** Assessing systems programming in modern C++ (C++17), graph data structures, topological sorting, memory ownership/RAII, automated unit testing, and simulation correctness.
* **Hardware Hobbyists & Designers:** Prototyping basic combinational and sequential logic circuits prior to hardware description language (HDL) implementation.

### Main Use Cases
* Constructing and verifying fundamental logic gates (`AND`, `OR`, `NOT`, `NAND`, `NOR`, `XOR`, `XNOR`).
* Building multi-level arithmetic circuits such as Half Adders, Full Adders, and Ripple-Carry Adders.
* Validating multi-level DAG signal propagation and verifying truth tables.
* Detecting invalid circuit topologies: driver contention (multiple outputs connected to one input), self-connections, and directed feedback cycles.
* Saving circuit topologies and wire connections to disk (`history.txt`) and loading them back into memory.
* Running headless automated regressions via `make test` or `ctest`.

### Current Implementation Status
* **Core Engine (`engine/`):** Fully implemented, verified with 13 automated tests (100% passing), zero compiler warnings under `-Wall -Wextra -O2`.
* **Automated Tests (`tests/`):** Unit tests for all 7 gate truth tables with three-valued logic edge cases; integration tests for Half Adder and Full Adder; validation tests for driver contention, disconnected inputs, and cycle detection; round-trip save/load persistence verification.
* **Performance Benchmark (`tests/benchmark.cpp`):** Standalone benchmark testing DAG depth scalability (1,000 chained gates) and multi-level adder netlists.
* **GUI Frontend (`main.cpp`, `source_files/`):** Operational with SFML 3.0.2 on MSYS2 UCRT64 and SFML 2.5 on Linux/macOS. Memory leaks and destructor recursion bugs have been resolved.
* **Build Automation:** Modern `CMakeLists.txt` and `Makefile` supporting separate `test`, `benchmark`, and `simulator` targets.

### Major Technical Highlights
* **Decoupled Architecture:** Core logic simulation has zero dependencies on SFML or GUI graphics, enabling headless execution in CI/CD pipelines.
* **Graph-Theoretic Levelization:** Eliminates frame-rate-dependent propagation delays through topological sorting.
* **Three-Valued Logic System:** Models floating or uninitialized pins with `UNDEFINED` (`X`) and enforces CMOS dominant-zero ($0 \cdot X = 0$) and dominant-one ($1 + X = 1$) propagation.
* **Strict RAII & Memory Safety:** Eradicated recursive destructor crashes and memory leaks; enforce clean heap management.
* **Cross-Version SFML Abstraction:** Bridges structural breaking changes between SFML 2.x and SFML 3.x using compile-time templates and preprocessor detection.

### Important Limitations
* **Sequential Logic Scope:** The simulator models combinational logic and level-sensitive loops; edge-triggered sequential elements (D-type Flip-Flops, Master-Slave JK Flip-Flops) and explicit clock generators are not yet implemented as primitive components.
* **Timing Simulation:** The engine uses an ideal zero-delay gate model; physical propagation delays ($t_{pd}$), inertial delays, and glitch modeling are not supported.
* **Graphical Canvas Scaling:** Canvas panning and zooming are not implemented; components reside on a fixed 900x650 viewport.

---

## 2. Project Goals and Problem Statement

### Motivating Problem
In digital electronics, software simulators are used to verify logic design before fabricating physical silicon. However, naive digital simulators often implement logic updates by evaluating gates in the arbitrary order in which they were placed into a global array or list during each GUI render frame. 

This causes two severe flaws:
1. **Frame-Lagged Signal Propagation:** In an $N$-level logic network (e.g., a Ripple-Carry Adder), a signal takes $N$ frames (at 60 FPS, $N \times 16.6\text{ ms}$) to propagate from inputs to final outputs. Intermediate gates temporarily display stale, incorrect logic values.
2. **Erratic Feedback Oscillations:** In loops or latches, the evaluation order determines which gate updates first, leading to non-deterministic oscillations or freezing based on array memory layout.

Furthermore, the inherited open-source codebase contained severe memory bugs:
* Every core class (`Gate`, `Connection`, `InputSignal`, `OutputSignal`) had `delete this;` inside its destructor. Calling `delete` caused infinite recursion and process termination.
* To avoid crashing, the original code simply erased pointers from vectors without deallocating heap memory, leaking 100% of deleted elements.
* `Gate::logicOperation()` accumulated booleans in a member vector each frame without clearing them on early returns, causing unbounded memory expansion at 60 Hz.
* Saving and loading circuits ignored connections entirely, leaving saved circuits broken.

### Project Goals
1. **Establish a Headless EDA Simulation Engine:** Decouple all logic representation and evaluation from SFML graphics shapes.
2. **Implement Topological Levelization (DAG):** Use Kahn’s algorithm to order gate evaluations from primary inputs to primary outputs so that signals settle instantaneously in a single evaluation pass.
3. **Incorporate Three-Valued Logic:** Support `LOW` (0), `HIGH` (1), and `UNDEFINED` (X) according to IEEE standard digital logic semantics.
4. **Detect Graph Cycles & Validate Connections:** Proactively reject driver contention (multiple drivers shorting an input) and self-loops; detect directed feedback cycles using DFS.
5. **Ensure Memory Safety & Robust RAII:** Remove recursive destructors and memory leaks; implement proper heap resource management.
6. **Provide Cross-Version Build Infrastructure:** Ensure seamless builds with modern CMake and Makefile on both legacy SFML 2.x and contemporary SFML 3.x.
7. **Build an Automated Regression Test Suite:** Implement automated tests with clear pass/fail assertions covering truth tables, multi-level circuits, and persistence.

### What the System Does NOT Attempt to Solve
* Analog SPICE simulation (transistor voltage/current curve solving, differential equations).
* VHDL / Verilog compilation or synthesis to FPGA bitstreams.
* Placement and routing (P&R) optimization for physical silicon layout.
* Full synchronous clock-tree synthesis.

### Expected Inputs and Outputs
* **Inputs:**
  * Interactive mouse clicks and drags on the SFML canvas.
  * Primary input pin logic toggles (`LOW` $\leftrightarrow$ `HIGH`).
  * Text-based netlist files (`history.txt` or custom `.netlist` files).
* **Outputs:**
  * Graphical render of logic gate states (color-coded pins and wires).
  * Evaluated logic levels at primary output probes.
  * Status diagnostics (gate counts, wire counts, cycle warnings, disconnected pin alerts).
  * Serialized netlist files.
  * Automated test results and execution metrics.

---

## 3. Technology Stack

### Programming Languages
* **C++17:** Core language used for both the simulation engine (`engine/`) and the desktop frontend (`source_files/`, `main.cpp`). Uses structured bindings, `std::unique_ptr`, `std::shared_ptr`, `std::optional`, `<chrono>`, `<queue>`, `<unordered_map>`, and template meta-programming.

### Graphics & GUI Framework
* **SFML (Simple and Fast Multimedia Library):**
  * **Version Tested:** SFML 3.0.2 (via MSYS2 UCRT64 `mingw-w64-ucrt-x86_64-sfml`) and SFML 2.5+.
  * **Modules Used:**
    * `sfml-graphics`: `sf::RenderWindow`, `sf::RectangleShape`, `sf::CircleShape`, `sf::Text`, `sf::Font`, `sf::Image`, `sf::Color`.
    * `sfml-window`: `sf::Event`, `sf::Mouse`, `sf::Keyboard`, `sf::VideoMode`, `sf::ContextSettings`.
    * `sfml-system`: `sf::Vector2f`, `sf::Vector2u`, `sf::Angle` (SFML 3), `sf::Time`.
  * **Role:** Desktop window creation, hardware-accelerated 2D rendering, font glyph rendering, and mouse/keyboard event polling.

### Build Systems & Toolchains
* **CMake (>= 3.16):** Modular configuration defining target libraries (`circuit_engine`), test executables (`circuit_tests`), benchmarks (`circuit_benchmark`), and GUI applications (`circuit_simulator`). Integrated with CTest.
* **GNU Make / MinGW32-Make:** Direct compilation support via `Makefile` with targets `make test`, `make benchmark`, `make simulator`, and `make clean`.
* **Compiler:** GCC 16.2.0 (MinGW-w64 MSYS2 UCRT64), compatible with Clang and MSVC.

### Testing Framework
* **Custom Lightweight Test Runner (`tests/test_framework.hpp`):**
  * Zero external dependencies.
  * Features assertion macros: `ASSERT_TRUE`, `ASSERT_FALSE`, `ASSERT_EQ`, `ASSERT_NE`.
  * Automatic test discovery using static registrar pattern (`TEST_CASE`).
  * High-resolution wall-clock execution timing via `std::chrono::high_resolution_clock`.

### Assets
* **Hack Font (`assets/fonts/Hack-Regular.ttf`):** Monospaced typeface for gate labels, button text, and status bar metrics.
* **Logic Icon (`assets/images/logic.png`):** Window title bar application icon.

### Categorized Technology Inventory

| Category | Technology | Version | Location Used | Purpose / Role |
|:---|:---|:---|:---|:---|
| **Language** | C++ | C++17 (`-std=c++17`) | All `.cpp` and `.hpp` | Core simulation, graph algorithms, GUI |
| **GUI Framework** | SFML | 3.0.2 / 2.5+ | `header_files/`, `source_files/`, `main.cpp` | Window management, 2D graphics, user input |
| **Build System** | CMake | >= 3.16 | `CMakeLists.txt` | Target generation, dependency linkage, CTest |
| **Build System** | GNU Make | 4.4+ | `Makefile` | Rapid terminal building, test automation |
| **Testing** | Custom C++ Harness | Custom | `tests/test_framework.hpp` | Headless unit and regression testing |
| **Asset** | Hack Font | SIL OFL | `assets/fonts/` | Monospaced text rendering in GUI |
| **Asset** | Logic PNG | Flaticon | `assets/images/` | Application window icon |

---

## 4. Complete Repository Structure

```
digital-circuit-simulator-main/
├── .gitignore                    # Git ignore file for build binaries, objects, and temp files
├── CMakeLists.txt                # CMake project definition (engine, tests, benchmark, simulator)
├── CONTEXT.md                    # This comprehensive project context documentation
├── LICENSE                       # MIT License with upstream attribution and asset credits
├── Makefile                      # Make build script supporting test, benchmark, simulator, clean
├── main.cpp                      # Graphical desktop application entry point and event loop
├── readme.md                     # User-facing README with setup, architecture, and metrics
│
├── engine/                       # CORE EDA SIMULATION ENGINE (Headless, Pure C++17)
│   ├── Circuit.hpp               # Circuit graph declaration: nodes, wires, DAG sort, cycle detection
│   ├── Circuit.cpp               # Graph algorithms, topological levelization, evaluation logic
│   ├── GateType.hpp              # GateType enum: AND, OR, NOT, NAND, NOR, XOR, XNOR, BUFFER
│   ├── GateType.cpp              # String conversion and reflection for gate types
│   ├── LogicGate.hpp             # LogicGate class: input pins, output pin, truth evaluation
│   ├── LogicGate.cpp             # Truth table evaluation implementation across all gate types
│   ├── LogicValue.hpp            # Three-valued logic: LOW (0), HIGH (1), UNDEFINED (X)
│   ├── LogicValue.cpp            # Boolean algebra with CMOS dominant states and stream operators
│   ├── Pin.hpp                   # Pin structure: direction (INPUT/OUTPUT), state, connection flag
│   ├── Serializer.hpp            # Netlist serialization interface
│   ├── Serializer.cpp            # Netlist parser and file reader/writer (V2 format)
│   └── Wire.hpp                  # Net structure connecting source output pin to target input pin
│
├── header_files/                 # GRAPHICAL USER INTERFACE HEADERS (SFML Dependent)
│   ├── button.hpp                # Toolbar button widget class
│   ├── components.hpp            # Toolbar component container, placement, save/load UI hooks
│   ├── connection.hpp            # Graphical wire line representation between visual ports
│   ├── fontManager.hpp           # Static font and application icon loader
│   ├── gate.hpp                  # Graphical logic gate rectangle, label, and port layout
│   ├── includes.hpp              # Common standard library and SFML header inclusions
│   ├── inputPort.hpp             # Input port terminal widget (derived from Port)
│   ├── inputSignal.hpp           # Interactive circular input driver toggle widget
│   ├── main.hpp                  # Consolidated header inclusions for main.cpp
│   ├── outputPort.hpp            # Output port terminal widget (derived from Port)
│   ├── outputSignal.hpp          # Interactive circular output probe monitor widget
│   ├── port.hpp                  # Base terminal circle widget for connections
│   ├── sfml_compat.hpp           # Cross-version compatibility adapters for SFML 2.x and 3.x
│   └── statusBar.hpp             # Viewport bottom status string overlay
│
├── source_files/                 # GRAPHICAL USER INTERFACE IMPLEMENTATIONS
│   ├── button.cpp                # Button layout, hover detection, and rendering
│   ├── components.cpp            # Toolbar initialization, new component creation, save/load
│   ├── connection.cpp            # Visual wire rendering, angle calculation, state propagation
│   ├── fontManager.cpp           # Font loading from assets/fonts and icon setting
│   ├── gate.cpp                  # Gate rendering, port positioning, drag actions, logicOperation
│   ├── inputPort.cpp             # Input port dummy/constructors
│   ├── inputSignal.cpp           # InputSignal toggle, deletion, memory cleanup
│   ├── outputPort.cpp            # OutputPort dummy/constructors
│   ├── outputSignal.cpp          # OutputSignal visual update, connection deletion
│   ├── port.cpp                  # Port color update, hover effects, click detection
│   └── statusBar.cpp             # Status text formatting and drawing
│
├── tests/                        # AUTOMATED REGRESSION & BENCHMARK SUITE
│   ├── benchmark.cpp             # Performance benchmark measuring throughput and DAG depth
│   ├── test_circuits.cpp         # Integration tests: Half Adder, Full Adder, DAG, Cycles, Save/Load
│   ├── test_framework.hpp        # Lightweight assertion and test harness header
│   ├── test_gates.cpp            # Unit truth table tests for all 7 logic gates with 3-valued logic
│   └── test_main.cpp             # Main test suite runner entry point
│
└── assets/                       # STATIC APPLICATION ASSETS
    ├── demo.gif                  # Animated GIF showing interactive simulator usage
    ├── fonts/
    │   └── Hack-Regular.ttf      # TrueType monospaced font
    └── images/
        └── logic.png             # Application window icon image
```

---

## 5. System Architecture

### Architectural Overview
The system employs a strict separation of concerns between logic graph computation and visual presentation:

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

### Module Responsibilities
1. **`eda::LogicValue`:** Defines the ternary logic system (`LOW`, `HIGH`, `UNDEFINED`) and truth functions for seven Boolean operations.
2. **`eda::Pin` & `eda::Wire`:** Defines endpoints and directed edges in the electrical netlist. An input pin can have at most one driving net (enforcing short-circuit protection).
3. **`eda::LogicGate`:** Evaluates Boolean functions over an arbitrary number of inputs.
4. **`eda::Circuit`:** Encapsulates the entire circuit netlist, maintains graph consistency, computes topological order, runs simulations, and identifies cyclic subgraphs.
5. **`eda::Serializer`:** Serializes circuits into human-readable netlists and deserializes saved text files with error reporting.
6. **GUI Controllers (`main.cpp`, `components.cpp`):** Handles rendering, event routing, coordinate-to-component mapping, and user interaction.

---

## 6. End-to-End System Workflow

### Workflow 1: Application Startup
1. **`main()` Entry:**
   - Initializes viewport dimensions ($900 \times 650$) and antialiasing context settings.
   - Instantiates `sf::RenderWindow window` in windowed desktop mode.
   - Calls `FontManager::loadAssets(window)`:
     * Reads `assets/images/logic.png` and sets the window icon.
     * Reads `assets/fonts/Hack-Regular.ttf` into static `FontManager::font`.
2. **`mainLoop()` Initialization:**
   - Instantiates `Component components(window)` which constructs the bottom toolbar buttons: `AND`, `OR`, `NOT`, `NAND`, `NOR`, `XOR`, `XNOR`, `INPUT`, `OUTPUT`, `SAVE`, `LOAD`.
   - Positions buttons along the bottom row with padding.
   - Instantiates `StatusBar status(FontManager::font)` at the bottom-left corner.
   - Enters the 60 FPS window loop.

### Workflow 2: Placing and Connecting Logic Gates
1. **User Placement:**
   - User clicks the `AND` button in the toolbar.
   - `Component::newComponent()` detects mouse containment and pushes `new Gate("AND", pos)` onto `Gate::gates`.
   - The gate constructor creates two `InputPort` objects on the left and one `OutputPort` on the right.
2. **Wiring:**
   - User presses the left mouse button over an `OutputPort` or `InputSignal`.
   - `main.cpp` sets `isConnecting = true`, records `startPoint`, and starts tracking an elastic connection line.
   - User releases the left mouse button over an `InputPort` on another gate.
   - `main.cpp` checks `port.isConnected`. If false, creates `new Connection(&port, prevPort)` and appends it to `Connection::connections`.
   - `Connection::connect()` marks the input port as connected, registers parent-child relationships, and updates state.
3. **Simulation Update (Continuous 60 Hz Loop):**
   - For every connection: `connection->drawTo(window)` updates the target port's state to match the driver's state.
   - For every gate: `gate->logicOperation()` reads connected input states, evaluates the Boolean truth function, and sets `oPort.setState(...)`.
   - Canvas clears to dark slate (`18, 18, 24`), components draw themselves, and the display buffers swap.

### Workflow 3: Headless Automated Test Execution
1. Terminal invokes `make test` or `./run_tests.exe`.
2. `test_main.cpp` executes `test_fw::TestRegistry::instance().runAll()`.
3. The test registry iterates through all registered `TEST_CASE` instances:
   - Evaluates all truth tables across binary and three-valued inputs.
   - Instantiates `eda::Circuit`, constructs multi-gate topologies (Half Adder, 3-level Full Adder), runs topological simulation, and asserts output equivalence.
   - Tests error handling: asserts rejection of driver contention and self-loops.
   - Tests cycle detection: creates feedback rings and asserts `CircuitStatus::HAS_CYCLE`.
   - Tests persistence: serializes circuits to string, restores into fresh circuit instances, and verifies truth table preservation.
4. Test runner prints individual test pass/fail results, wall-clock execution time, and summary exit codes (0 for pass, 1 for fail).

---

## 7. Feature-by-Feature Breakdown

### Feature 1: Three-Valued Logic Evaluation
* **Purpose:** Digital simulation conforming to IEEE 1164 logic standards, correctly handling disconnected and high-impedance pins.
* **Implementation:** `engine/LogicValue.hpp` and `engine/LogicValue.cpp`.
* **Values:** `LogicValue::LOW` (0), `LogicValue::HIGH` (1), `LogicValue::UNDEFINED` (X).
* **CMOS Dominant State Logic:**
  * AND Gate: $0 \text{ AND } X = 0$ (a known 0 input forces output to 0 regardless of unknown inputs).
  * OR Gate: $1 \text{ OR } X = 1$ (a known 1 input forces output to 1 regardless of unknown inputs).
  * XOR Gate: Any $X$ input propagates an $X$ output.
* **Edge Cases Handled:** Disconnected or floating pins evaluate to `UNDEFINED` rather than silently defaulting to false.

### Feature 2: Topological DAG Levelization
* **Purpose:** Guarantees deterministic, instant signal propagation across multi-level logic networks in a single evaluation cycle.
* **Implementation:** `Circuit::getTopologicalOrder()` in `engine/Circuit.cpp`.
* **Algorithm:** Kahn's Algorithm.
  1. Computes in-degree for every gate based on incoming nets from other gates.
  2. Enqueues all gates with in-degree 0 into a FIFO queue.
  3. Iteratively extracts gates, appends to the evaluation order, and decrements the in-degree of downstream dependent gates.
  4. If the number of sorted gates equals total gates, the circuit is an acyclic DAG.
* **Output:** Ordered sequence of gates. `Circuit::evaluate()` propagates signals through this sequence, ensuring inputs settle before dependent gates execute.

### Feature 3: Cycle & Feedback Loop Detection
* **Purpose:** Identifies directed feedback loops (e.g., ring oscillators, cross-coupled latches) to prevent infinite loops during simulation.
* **Implementation:** `Circuit::hasCycles()` in `engine/Circuit.cpp`.
* **Algorithm:** Three-color Depth-First Search (DFS).
  * State 0: Unvisited.
  * State 1: Visiting (currently on active recursion call stack).
  * State 2: Visited.
  * If a DFS traversal encounters a neighbor with State 1, a directed cycle exists. The back-edge trace reconstructs the exact cycle path (`vector<string>* cyclePath`).
* **Behavior:** When cycles are present, `Circuit::evaluate()` runs an iterative relaxation loop bounded by a maximum iteration guard (64 iterations), avoiding process hangs and returning `CircuitStatus::HAS_CYCLE`.

### Feature 4: Electrical Connection Validation
* **Purpose:** Enforces physical digital design rules and prevents invalid net connections.
* **Implementation:** `Circuit::canConnect()` and `Circuit::connect()` in `engine/Circuit.cpp`.
* **Validation Rules:**
  1. **Component Existence:** Verifies source and destination component IDs exist in the netlist.
  2. **Pin Bounds:** Validates pin indices against available inputs/outputs.
  3. **Pin Directionality:** Requires source to be an output pin and destination to be an input pin.
  4. **Self-Loop Prevention:** Rejects connections where `sourceId == targetId`.
  5. **Driver Contention Rejection:** Verifies the destination input pin is not already driven by an existing net (protecting against short circuits).
  6. **Duplicate Rejection:** Rejects redundant wires between the same pin pairs.

### Feature 5: Netlist Serialization & Deserialization
* **Purpose:** Saves circuit diagrams and restores full component and connection state to/from disk.
* **Implementation:** `engine/Serializer.cpp` and `source_files/components.cpp`.
* **Format:** V2 structured netlist text format:
  ```
  # DIGITAL CIRCUIT SIMULATOR NETLIST V2
  GATE gate_1 AND 2 300.0 200.0
  INPUT in_1 A 1 100.0 150.0
  OUTPUT out_1 Y 500.0 200.0
  WIRE wire_1 in_1 0 gate_1 0
  ```
* **Resilient Deserialization:** Defers wire connection until all gates and signal probes are constructed. In the GUI, uses a fuzzy coordinate tolerance radius (15 pixels) to reconnect ports reliably without floating-point mismatch.

---

## 8. Frontend Architecture

### Framework & Setup
The frontend is built with **SFML** (`sf::RenderWindow`, `sf::RectangleShape`, `sf::CircleShape`, `sf::Text`). It runs an event-driven loop capped at 60 FPS.

### Cross-Version SFML Compatibility Layer (`header_files/sfml_compat.hpp`)
SFML 3 introduced several breaking changes relative to SFML 2. `sfml_compat.hpp` bridges these versions at compile time:
* **Geometry Dimensions:** SFML 3 replaced `sf::Rect::width` and `height` with `sf::Rect::size.x` and `size.y`. `compat::rectWidth()` and `compat::rectHeight()` handle this abstraction.
* **Transformations:** Replaced two-float parameter methods with `sf::Vector2f` calls (`compat::setOrigin`, `compat::setPosition`, `compat::setScale`).
* **Rotations:** SFML 3 expects `sf::Angle` constructed via `sf::degrees(angle)`. `compat::setRotationDegrees()` encapsulates this conversion.
* **Event Polling:** SFML 3 uses `std::optional<sf::Event> pollEvent()` with `.is<T>()` and `.getIf<T>()` type queries, whereas SFML 2 uses `bool pollEvent(sf::Event&)`. Preprocessor switches in `main.cpp` support both patterns.

### GUI Entities
* **`Gate` (`source_files/gate.cpp`):** Visual rectangle displaying logic type, maintaining dynamic input ports (`std::vector<InputPort> iPorts`) and an output port (`OutputPort oPort`).
* **`Port` / `InputPort` / `OutputPort` (`source_files/port.cpp`):** Circular terminals. Turn green when driven `HIGH` and white when `LOW` or un-driven.
* **`InputSignal` (`source_files/inputSignal.cpp`):** Circular toggleable driver with blue outline. `Alt + Click` toggles state between 0 and 1.
* **`OutputSignal` (`source_files/outputSignal.cpp`):** Circular probe monitor with red outline. Reflects the driving wire's state.
* **`Button` (`source_files/button.cpp`):** Interactive UI widget with hover highlights and centered label.
* **`StatusBar` (`source_files/statusBar.cpp`):** Displays real-time gate and wire count metrics at the bottom-left of the viewport.

---

## 9. Backend Architecture (Core Simulation Engine)

The core engine resides in `engine/` and acts as the headless backend:

### Engine Request Lifecycle
```
User/Test API Call
       ↓
Circuit::connect(srcId, srcPin, dstId, dstPin)
       ↓
Circuit::canConnect() [Validates driver contention, self-loops, pin bounds]
       ↓
Add Wire to m_wires & Mark Pin::isConnected = true
       ↓
Circuit::evaluate()
       ↓
Circuit::getTopologicalOrder() [Kahn's algorithm calculates execution order]
       ↓
Propagate Primary Inputs to Nets
       ↓
For each Gate in Topological Order:
   Fetch input pin values from incoming wires
   LogicGate::evaluate()
   Propagate output pin value to outgoing wires
       ↓
Update Output Probes
       ↓
Return CircuitStatus (OK, HAS_CYCLE, DISCONNECTED_INPUTS, EMPTY)
```

### Resource Management & RAII
* Components are owned by `std::unordered_map<std::string, std::shared_ptr<LogicGate>>` and explicit value-type maps (`InputSource`, `OutputProbe`).
* Wires are stored in `std::vector<Wire>`.
* Removing a component automatically sweeps and disconnects all associated incoming and outgoing wires, updating pin connection flags.
* Memory deallocation is automatic via RAII when `Circuit` is destroyed or `clear()` is invoked.

---

## 10. API Documentation (Core Engine API)

### Class: `eda::Circuit`

#### `std::shared_ptr<LogicGate> addGate(GateType type, const std::string& id = "", size_t numInputs = 0, float x = 0.0f, float y = 0.0f)`
* **Purpose:** Instantiates and inserts a logic gate into the netlist.
* **Parameters:** Gate type enum, optional unique ID (auto-generated if empty), input pin count (defaults to gate type standard), (x, y) coordinates.
* **Returns:** Shared pointer to the created `LogicGate`.

#### `bool addInput(const std::string& id, const std::string& label = "", LogicValue initialVal = LogicValue::LOW, float x = 0.0f, float y = 0.0f)`
* **Purpose:** Adds a primary input stimulus source.
* **Returns:** `true` on success, `false` if ID already exists.

#### `void setInputValue(const std::string& id, LogicValue val)`
* **Purpose:** Sets the logic state of a primary input driver.

#### `bool addOutput(const std::string& id, const std::string& label = "", float x = 0.0f, float y = 0.0f)`
* **Purpose:** Adds a primary output observation probe.

#### `LogicValue getOutputValue(const std::string& id) const`
* **Purpose:** Reads the simulated logic state of an output probe.

#### `ValidationResult canConnect(const std::string& sourceId, size_t sourcePin, const std::string& targetId, size_t targetPin) const`
* **Purpose:** Validates whether a connection is electrically and topologically valid.
* **Returns:** `ValidationResult { bool valid; std::string message; }`.

#### `ValidationResult connect(const std::string& sourceId, size_t sourcePin, const std::string& targetId, size_t targetPin)`
* **Purpose:** Connects an output pin to an input pin and creates a net.

#### `bool disconnect(const std::string& wireId)`
* **Purpose:** Removes a wire net and updates connected pin states.

#### `bool hasCycles(std::vector<std::string>* cyclePath = nullptr) const`
* **Purpose:** Checks the circuit graph for directed cycles via 3-color DFS.

#### `std::vector<std::string> getTopologicalOrder(bool* hasCycleOut = nullptr) const`
* **Purpose:** Computes the levelized topological sequence of gates using Kahn’s algorithm.

#### `CircuitStatus evaluate()`
* **Purpose:** Runs simulation over the entire circuit.
* **Returns:** `CircuitStatus::OK`, `CircuitStatus::HAS_CYCLE`, `CircuitStatus::DISCONNECTED_INPUTS`, or `CircuitStatus::EMPTY`.

---

## 11. Database & Persistence Architecture

The project uses a structured flat-file netlist representation instead of an external database engine.

### Netlist Specification (`V2 Format`)
* **Header:** `# DIGITAL CIRCUIT SIMULATOR NETLIST V2`
* **Gate Record:** `GATE <id> <type> <numInputs> <posX> <posY>`
* **Input Record:** `INPUT <id> <label> <initialValue: 0|1|X> <posX> <posY>`
* **Output Record:** `OUTPUT <id> <label> <posX> <posY>`
* **Wire Record:** `WIRE <id> <sourceId> <sourcePin> <targetId> <targetPin>`

### Legacy Compatibility
The parser (`Serializer::deserializeFromString`) inspects line tags. If a line matches the legacy 3-column format (`<GateType> <posX> <posY>`), it automatically falls back to legacy component loading, maintaining backwards compatibility with older files.

---

## 12. Authentication, Authorization, & Security
* **Desktop Application:** As a standalone local desktop and CLI simulation tool, user authentication, sessions, tokens, and role-based access control are not applicable.
* **File I/O Safety:** Netlist file deserialization checks token counts, catches string-to-enum conversion exceptions, and reports line numbers on parse errors.

---

## 13–17. AI, ML, LLM, & Agent Architectures
* **Status:** This project is a deterministic software system based on discrete mathematics, Boolean algebra, and graph theory. It contains no artificial intelligence, machine learning models, neural networks, or LLM integrations.

---

## 18. Data Flow

### Combinational Logic Simulation Flow
```
[User / Test Harness]
       │
       ▼
setInputValue("A", HIGH), setInputValue("B", HIGH)
       │
       ▼
Circuit::evaluate()
       │
       ├─► Kahn's Algorithm Levelization:
       │     In-degrees calculated from wire dependencies
       │     Queue processes gates in topological rank
       │
       ├─► Wire Propagation (Level 0):
       │     "A" drives Pin 0 of XOR1 and Pin 0 of AND1
       │     "B" drives Pin 1 of XOR1 and Pin 1 of AND1
       │
       ├─► Level 1 Gate Evaluation:
       │     XOR1 evaluates (HIGH ^ HIGH) = LOW
       │     AND1 evaluates (HIGH & HIGH) = HIGH
       │
       ├─► Wire Propagation (Level 1):
       │     XOR1 drives Pin 0 of XOR2
       │     Cin drives Pin 1 of XOR2
       │
       ├─► Level 2 Gate Evaluation:
       │     XOR2 evaluates (LOW ^ LOW) = LOW  --> Propagates to Sum Output Probe
       │
       └─► Final State Query:
             getOutputValue("Sum") == LOW
             getOutputValue("Cout") == HIGH
```

---

## 19. Important Classes, Functions, and Modules

| Class / Module | File Location | Purpose & Important Logic |
|:---|:---|:---|
| `eda::Circuit` | `engine/Circuit.hpp` / `.cpp` | Graph container; Kahn's topological sort; 3-color DFS cycle detector; evaluation loop; connection validator. |
| `eda::LogicGate` | `engine/LogicGate.hpp` / `.cpp` | Gate node; stores `vector<Pin> inputPins` and `Pin outputPin`; evaluates truth operations. |
| `eda::LogicValue` | `engine/LogicValue.hpp` / `.cpp` | Defines `LogicValue` enum (`LOW`, `HIGH`, `UNDEFINED`) and IEEE CMOS dominant logic operations. |
| `eda::Serializer` | `engine/Serializer.hpp` / `.cpp` | Serializes and parses V2 Netlist text format; handles deferred wire resolution. |
| `compat` | `header_files/sfml_compat.hpp` | Cross-version adapter for SFML 2 vs 3 geometry, origins, angles, and events. |
| `Component` | `source_files/components.cpp` | GUI toolbar management; component factory; UI save/load integration with fuzzy port matching. |
| `Gate` | `source_files/gate.cpp` | Visual gate widget; port layout; hover/selection highlight; calls logic evaluation. |
| `Connection` | `source_files/connection.cpp` | Visual wire line calculation (hypotenuse length and trigonometric angle); state propagation. |
| `test_fw::TestRegistry` | `tests/test_framework.hpp` | Singleton test discovery registry; runs assertions and outputs colored execution telemetry. |

---

## 20–22. Configuration & Environment Variables

* **Configuration:** No external configuration servers or `.env` files are required. All parameters are compiled in or configured via CMake build flags (`CMAKE_BUILD_TYPE=Release`, `CMAKE_CXX_STANDARD=17`).
* **External APIs:** None. Fully self-contained local desktop and CLI application.

---

## 23. Error Handling

* **Driver Contention:** Detected in `Circuit::canConnect()`; rejected with diagnostic: `"Target input pin (comp:pin) is already driven by wire X"`.
* **Self-Loops:** Detected in `Circuit::canConnect()`; rejected with diagnostic: `"Self-connections are invalid"`.
* **Cycle Oscillations:** Detected in `Circuit::hasCycles()`; `Circuit::evaluate()` applies an iteration limit (64 passes) and returns `CircuitStatus::HAS_CYCLE` to prevent process lockups.
* **Floating Pins:** Detected in `Circuit::evaluate()`; returns `CircuitStatus::DISCONNECTED_INPUTS`.
* **Corrupted Netlist Files:** Caught in `Serializer::deserializeFromString()`; reports parsing failures with line numbers and preserves the previous circuit state.

---

## 24. Security
* Operates locally with standard user privileges.
* Does not open network sockets or execute external shell strings.
* Input validation on file reads prevents buffer overruns through standard C++ string stream parsing.

---

## 25–26. Deployment & Containerization
* **Distribution:** Built as a standalone native binary (`simulator.exe` on Windows, `simulator` on Linux/macOS).
* **Dependencies:** Relies on SFML dynamic shared libraries (`sfml-graphics`, `sfml-window`, `sfml-system`).
* **CI/CD Compatibility:** Headless unit tests (`run_tests.exe`) require zero display servers or X11 forwarding, allowing clean execution inside Docker containers or GitHub Actions runners.

---

## 27. Build and Runtime Process

### Prerequisites
* C++17 compliant compiler (`g++`, `clang++`, `cl.exe`).
* CMake $\ge 3.16$ or GNU Make.
* SFML 2.5+ or SFML 3.x (only required for the graphical simulator).

### Exact Terminal Commands

#### Building and Running Automated Tests (No SFML Required)
```bash
# Using Make:
make test
# On Windows MinGW:
mingw32-make test

# Using CMake:
cmake -B build
cmake --build build --target circuit_tests
./build/circuit_tests
```

#### Running Performance Benchmarks
```bash
# Using Make:
make benchmark
# On Windows MinGW:
mingw32-make benchmark

# Using CMake:
cmake --build build --target circuit_benchmark
./build/circuit_benchmark
```

#### Building and Launching GUI Simulator
```bash
# Using Make:
make simulator
./simulator

# Using CMake:
cmake --build build --target circuit_simulator
./build/circuit_simulator
```

#### Cleaning Build Artifacts
```bash
make clean
# On Windows MinGW:
mingw32-make clean
```

---

## 28. Testing

### Test Framework
Lightweight, header-only framework located in [`tests/test_framework.hpp`](file:///c:/Users/Tanya%20Verma/OneDrive/Desktop/digital-circuit-simulator-main/tests/test_framework.hpp). Provides assertion macros (`ASSERT_TRUE`, `ASSERT_FALSE`, `ASSERT_EQ`, `ASSERT_NE`) with detailed error reporting.

### Automated Test Suites & Verified Status

```
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

## 29. Performance and Optimization

### Measured Benchmark Metrics (Verified on GCC 16.2.0, Windows 11, MinGW UCRT64)

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

### Optimizations Implemented
1. **Topological Levelization Cache:** Levelizes gates once per evaluation pass rather than re-evaluating each gate across multiple rendering frames.
2. **Short-Circuit Dominant Evaluation:** Evaluates AND/OR truth operations immediately when a dominant 0 or 1 is encountered.
3. **Fuzzy Port Matching:** Employs spatial radial thresholds rather than exact float comparisons when resolving port connections during deserialization.

---

## 30. Important Design Decisions & Trade-offs

1. **Decoupled Engine vs. Unified Architecture:**
   * *Decision:* Extracted a pure C++17 `engine/` library completely independent of SFML graphics.
   * *Rationale:* Allows running headless regression tests and benchmarks in automated CI environments where display servers are unavailable.
2. **Kahn's Algorithm vs. DFS for Topological Sort:**
   * *Decision:* Used Kahn’s algorithm (in-degree tracking + queue) for topological sort, combined with a separate 3-color DFS pass for cycle detection.
   * *Rationale:* Kahn's algorithm produces levelized execution orders naturally suited for breadth-first hardware propagation, while DFS provides exact cycle path traces.
3. **Three-Valued Logic vs. Binary (bool) Logic:**
   * *Decision:* Used ternary enum `LogicValue` (`LOW`, `HIGH`, `UNDEFINED`).
   * *Rationale:* Standard digital design must account for floating, high-impedance, or un-driven nets.

---

## 31. Current Limitations & Technical Debt

1. **Sequential Elements:** Flip-Flops and clocks are not yet primitive library components.
2. **Gate Delay Timing:** Simulation assumes ideal zero-delay combinational propagation.
3. **Canvas Viewport:** Lacks scrollable or zoomable infinite virtual coordinates.

---

## 32. Interview Perspective (Siemens EDA Student Intern Profile)

### 30-Second Elevator Pitch
"I modernized a C++ digital circuit simulator into a modular EDA platform. I extracted a headless C++17 netlist simulation engine with three-valued logic, implemented Kahn’s topological sort to eliminate frame-lagged evaluation, built automated regression test suites, resolved severe destructor memory corruption bugs, and added cross-version SFML compatibility."

### 1-Minute Technical Pitch
"As a software engineering student preparing for Siemens EDA, I re-architected this digital circuit simulator to follow industry EDA standards. Previously, the project evaluated gates in arbitrary array order inside a 60 FPS rendering loop, introducing artificial frame propagation delays and memory leaks. I decoupled the engine into a headless C++17 library that models circuits as directed acyclic graphs (DAGs). I implemented Kahn’s topological sorting algorithm to ensure deterministic level-by-level evaluation in a single pass, added DFS-based feedback cycle detection, and enforced IEEE 1164-style three-valued logic. I resolved undefined behavior where destructors invoked `delete this;`, built a 13-test automated regression suite, and engineered a compatibility layer bridging SFML 2 and SFML 3."

### Key Questions and Model Answers

#### Q: How does your topological sorting improve simulation accuracy?
* **Answer:** Naive simulators update gates in arbitrary storage order once per GUI frame. In a circuit with $N$ sequential gate levels, an input transition takes $N$ frames (up to hundreds of milliseconds) to ripple through. By computing the topological order via Kahn’s algorithm, the engine levelizes gates from primary inputs to outputs, evaluating every dependency before downstream gates execute. This guarantees signals settle in a single evaluation pass with zero extra latency.

#### Q: How did you fix the memory management issues?
* **Answer:** The legacy code invoked `delete this;` inside destructors for `Gate`, `Connection`, and signal classes, triggering infinite recursive destruction and crashes when deleted. Because of this, the original author avoided calling `delete`, leaking memory on every removal. I eliminated all `delete this;` calls, introduced proper RAII cleanup in removal methods, and fixed an unbounded `std::vector<bool>` leak in the gate evaluation loop.

---

## 33. Source-of-Truth Rules

1. **Codebase as Primary Authority:** Implementation details in `engine/`, `source_files/`, and `tests/` supersede any legacy comments or old README descriptions.
2. **Attribution Preservation:** Original graphical framework attribution belongs to Binit Awasthi; modernization, EDA engine extraction, graph algorithms, testing, and memory fixes belong to Tanya Verma.
3. **No Fabricated Data:** All test passes (13/13) and benchmark latencies are empirically measured directly from the executable.

---

## 34. Final Project Summary

* **Project:** Digital Circuit Simulator (EDA Modernization).
* **Architecture:** Decoupled two-tier architecture: Headless C++17 Simulation Engine + SFML Interactive GUI.
* **Core Algorithms:** Kahn's Topological Sort (DAG Levelization), Three-Color DFS Cycle Detection, IEEE Three-Valued Logic Evaluation, V2 Netlist Serialization.
* **Test Metrics:** 13/13 passing automated unit and integration tests (0 failures, executed in under 1 ms).
* **Performance:** Verified on deep DAGs up to 1,000 logic levels and 50-stage full adders.
* **Status:** Complete, warning-free under `-Wall -Wextra -O2`, clean build automation via both CMake and Makefile.

---
*Generated: October 2026 | Digital Circuit Simulator Project Context*

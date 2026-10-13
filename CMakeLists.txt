cmake_minimum_required(VERSION 3.16)
# Maintainer: Tanya Verma (https://github.com/tanyaverma20/Digital-Circuit-Simulator)
project(DigitalCircuitSimulator
    VERSION 2.0.0
    DESCRIPTION "Digital Circuit Simulator — C++17 EDA Simulation Engine"
    HOMEPAGE_URL "https://github.com/tanyaverma20/Digital-Circuit-Simulator"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /WX-)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# ==========================================
# 1. Core EDA Simulation Engine (Headless)
# ==========================================
set(ENGINE_SOURCES
    engine/LogicValue.cpp
    engine/GateType.cpp
    engine/LogicGate.cpp
    engine/Circuit.cpp
    engine/Serializer.cpp
)

set(ENGINE_HEADERS
    engine/LogicValue.hpp
    engine/GateType.hpp
    engine/Pin.hpp
    engine/Wire.hpp
    engine/LogicGate.hpp
    engine/Circuit.hpp
    engine/Serializer.hpp
)

add_library(circuit_engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})
target_include_directories(circuit_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# ==========================================
# 2. Automated Test Suite (Zero GUI Dependency)
# ==========================================
enable_testing()

set(TEST_SOURCES
    tests/test_main.cpp
    tests/test_gates.cpp
    tests/test_circuits.cpp
)

add_executable(circuit_tests ${TEST_SOURCES} tests/test_framework.hpp)
target_link_libraries(circuit_tests PRIVATE circuit_engine)
target_include_directories(circuit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME CircuitEngineTests COMMAND circuit_tests)

add_executable(circuit_benchmark tests/benchmark.cpp)
target_link_libraries(circuit_benchmark PRIVATE circuit_engine)
target_include_directories(circuit_benchmark PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


# ==========================================
# 3. GUI Simulator (SFML Frontend)
# ==========================================
find_package(SFML 3 COMPONENTS Graphics Window System QUIET)
if(NOT SFML_FOUND)
    find_package(SFML 2.5 COMPONENTS graphics window system QUIET)
endif()

if(SFML_FOUND)
    message(STATUS "Found SFML: Building graphical circuit_simulator target")
    set(GUI_SOURCES
        main.cpp
        source_files/button.cpp
        source_files/components.cpp
        source_files/connection.cpp
        source_files/fontManager.cpp
        source_files/gate.cpp
        source_files/inputPort.cpp
        source_files/inputSignal.cpp
        source_files/outputPort.cpp
        source_files/outputSignal.cpp
        source_files/port.cpp
        source_files/statusBar.cpp
    )

    add_executable(circuit_simulator ${GUI_SOURCES})
    target_link_libraries(circuit_simulator PRIVATE circuit_engine SFML::Graphics SFML::Window SFML::System)
    target_include_directories(circuit_simulator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/header_files)
else()
    message(STATUS "SFML not found by CMake find_package: Check manual flags or use Makefile for GUI build")
endif()

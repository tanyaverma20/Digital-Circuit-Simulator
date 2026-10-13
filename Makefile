CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2 -I. -I./header_files

# SFML Libraries
SFML_LIBS ?= -lsfml-graphics -lsfml-window -lsfml-system

ifeq ($(OS),Windows_NT)
    EXE = .exe
    RM = del /f /q
else
    EXE =
    RM = rm -f
endif

# Binary targets
SIMULATOR_BIN = simulator$(EXE)
TEST_BIN = run_tests$(EXE)

ENGINE_SRCS = $(wildcard engine/*.cpp)
TEST_SRCS = $(wildcard tests/*.cpp)
GUI_SRCS = main.cpp $(wildcard source_files/*.cpp)

.PHONY: all test simulator run clean help

all: test simulator

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(ENGINE_SRCS) $(TEST_SRCS)
	$(CXX) $(CXXFLAGS) $(ENGINE_SRCS) $(TEST_SRCS) -o $(TEST_BIN)

simulator: $(SIMULATOR_BIN)

$(SIMULATOR_BIN): $(GUI_SRCS) $(ENGINE_SRCS)
	$(CXX) $(CXXFLAGS) $(GUI_SRCS) $(ENGINE_SRCS) -o $(SIMULATOR_BIN) $(SFML_LIBS)

run: $(SIMULATOR_BIN)
	./$(SIMULATOR_BIN)

clean:
ifeq ($(OS),Windows_NT)
	-@del /f /q $(SIMULATOR_BIN) $(TEST_BIN) *.o 2>nul || exit 0
else
	-@rm -f $(SIMULATOR_BIN) $(TEST_BIN) *.o
endif

help:
	@echo "Available targets:"
	@echo "  make test      - Build and run headless test suite (pure C++17, no SFML required)"
	@echo "  make simulator - Build GUI application (requires SFML)"
	@echo "  make run       - Build and launch GUI simulator"
	@echo "  make clean     - Remove compiled binaries"

# Digital Circuit Simulator

![Demo](assets/demo.gif)

## Overview

The Digital Circuit Simulator is a powerful C++ program designed to help users design and simulate digital circuits with ease. With its intuitive drag-and-drop interface, users can effortlessly add logic gates, input/output signals, and create connections between them to visualize and test circuit functionality.

## Features

- **Drag and Drop Elements**: Easily add and position logic gates and signals.
- **Intuitive Shortcuts**:
  - **Move Input Signal**: `Ctrl + Drag with Left Mouse Button`
  - **Remove Connections**: `Ctrl + Right Click` (for removing connections from gates and signals)
  - **Toggle State**: `Alt + Left Click`
  - **Move Output Signal**: `Left Click and Drag`
  - **Clear All Connections**: `Ctrl + C`
  - **Remove All Elements**: `Ctrl + X`
  - **Remove Specific Element**: `Right Click`
  - **Duplicate Element**: `Shift + Left Click`
  - **Exit program**: `Esc`
- **Creating Connections**: Click and drag from the input signal or output port to input port or output signal.
- **Connection Rules**:
  - **Connections**: Can only be drawn from the output port or input signal.

## Installation and Configuration

### Prerequisites

- C++ Compiler (e.g., g++, clang++)
- Make
- SFML (Simple and Fast Multimedia Library)

### Steps

1. **Install SFML**:

   - **Linux**:
     ```bash
     sudo apt-get install libsfml-dev
     ```

   - **MacOS** (using Homebrew):
     ```bash
     brew install sfml
     ```

   - **Windows**:
     Download the SFML library from the [official website](https://www.sfml-dev.org/download.php) and follow the installation instructions for your specific development environment.

2. **Clone the Repository**:
   ```bash
   git clone https://github.com/binit-awasthi/digital-circuit-simulator.git
   cd digital-circuit-simulator
   ```

3. **Build the Project**:

   Add path to sfml include files and binary to the Makefile then run the following command on terminal
   
   ```bash
     make
     ```

4. **Run the Simulator**:
   ```bash
   ./simulator
   ```

 ## Attribution:

<a href="https://www.flaticon.com/free-icons/logic" title="logic icons">Logic icons created by orvipixel - Flaticon</a>

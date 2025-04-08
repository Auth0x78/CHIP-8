# CHIP-8 Emulator

This project is a CHIP-8 emulator developed in C and C++. It aims to accurately emulate the behavior of the CHIP-8 virtual machine, allowing users to run classic CHIP-8 programs and games.
![image](https://github.com/user-attachments/assets/42fbaf24-cb59-46af-8ef4-cb22f75d082e)

## Features

- Accurate emulation of CHIP-8 instructions
- Support for loading and executing CHIP-8 programs
- Simple and intuitive interface

## Getting Started

### Prerequisites

- CMake
- A C/C++ compiler

### Building the Project

1. Clone the repository:

   ```bash
   git clone https://github.com/Auth0x78/CHIP-8.git
   ```

2. Navigate to the project directory:

   ```bash
   cd CHIP-8
   ```

3. Create a build directory and navigate into it:

   ```bash
   mkdir build
   cd build
   ```

4. Run CMake to configure the project:

   ```bash
   cmake ..
   ```

5. Build the project:

   ```bash
   make
   ```

## Usage

After building the project, you can run the emulator by executing the generated binary. Ensure you have a CHIP-8 program file to load into the emulator.

```bash
./chip8
```

An windows with load rom button will be there, click on it and load a CHIP-8 ROM File. Then unselect the PAUSE check button to start emulation.

## License

This project is licensed under the MIT License. See the [LICENSE.txt](LICENSE.txt) file for details.

## Acknowledgments

This project utilizes third-party libraries and resources located in the `thirdparty` and `resources` directories. We acknowledge and thank the authors of these resources for their contributions.

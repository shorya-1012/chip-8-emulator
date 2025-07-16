## CHIP-8 Emulator in C++

This repository contains my implementation of a CHIP-8 emulator written in C++. It's a simple virtual machine capable of running classic CHIP-8 programs.

### Dependencies

* **SDL3**
  Included as a git submodule

### Installation 

- Clone the repository : 
  ```bash
  git clone --recurse-submodules https://github.com/shorya-1012/chip-8-emulator.git
  ```
- Change to project directory and Make a build directory :
  ```bash
   cd chip-8-emulator && mkdir build && cd build
    ```
- Run cmake
    ```bash
   cmake ..
    ```
- Build and run
   ```bash
   make 
    ```
     ```bash
   ./Chip_8_Emulator path/to/rom
    ```

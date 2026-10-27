# C JSON Parser (`json_parser`)

A lightweight, dependency-free JSON lexer, parser, and query explorer implemented in C (C11 standard) with a dynamic array (`ArrayList`) data structure.

---

## Table of Contents

- [Project Structure](#project-structure)
- [Prerequisites](#prerequisites)
  - [Linux](#linux)
  - [Windows](#windows)
- [Building & Running on Linux](#building--running-on-linux)
  - [Method 1: CMake (Recommended)](#method-1-cmake-recommended)
  - [Method 2: Direct Compilation (GCC / Clang)](#method-2-direct-compilation-gcc--clang)
- [Building & Running on Windows](#building--running-on-windows)
  - [Method 1: MinGW-w64 / MSYS2 (Recommended)](#method-1-mingw-w64--msys2-recommended)
  - [Method 2: Visual Studio / MSVC](#method-2-visual-studio--msvc)
  - [Method 3: Windows Subsystem for Linux (WSL)](#method-3-windows-subsystem-for-linux-wsl)
- [Important Runtime Notes](#important-runtime-notes)
- [Cleaning Build Artifacts](#cleaning-build-artifacts)

---

## Project Structure

```text
c_json/
├── CMakeLists.txt        # CMake build configuration
├── test.json             # Sample JSON dataset for testing
├── inc/                  # Header files
│   ├── array_list.h      # Generic dynamic array implementation
│   ├── json_lexer.h      # Token definitions and lexer interface
│   ├── json_parser.h     # AST/DOM node definitions and tree builder
│   └── json_explorer.h   # Key search functions (e.g., json_findkey)
└── src/                  # Source files
    ├── array_list.c      # Array list logic
    ├── json_lexer.c      # Lexical analysis / tokenizer
    ├── json_parser.c     # Parser and memory cleanup
    ├── json_explorer.c   # JSON traversal utilities
    └── main.c            # Driver reading and querying test.json
```

---

## Prerequisites

### Linux

- **C Compiler**: GCC (`gcc`) 9+ or Clang (`clang`) 10+ with C11 support.
- **Build System**: CMake 3.20+ and Make (or Ninja).
- On Debian/Ubuntu:
  ```bash
  sudo apt update
  sudo apt install build-essential cmake
  ```

### Windows

You can build on Windows using either **MinGW-w64 (GCC)**, **Clang**, or **MSVC**:

- **Recommended: MinGW-w64 (via MSYS2 or standalone)**
  - Supports C99/C11 Variable Length Arrays (VLAs) used in the lexer string slicing routines.
  - Install via [MSYS2](https://www.msys2.org/):
    ```bash
    pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja
    ```
- **Alternative: Visual Studio (MSVC / clang-cl)**
  - Visual Studio 2019/2022 with the "Desktop development with C++" workload and CMake tools installed.
  - *Note*: MSVC `cl.exe` does not natively support C99 Variable Length Arrays (`char str[length]`). When using Visual Studio, compiling with the LLVM / Clang-cl toolset or MinGW is recommended.

---

## Building & Running on Linux

### Method 1: CMake (Recommended)

1. **Configure the project:**
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Debug
   ```

2. **Compile the executable:**
   ```bash
   cmake --build build
   ```

3. **Run the program:**
   > **Note:** The program looks for `test.json` in the current working directory. Always launch the binary from the repository root:
   ```bash
   ./build/json_parser
   ```

### Method 2: Direct Compilation (GCC / Clang)

If you do not want to use CMake, compile directly using GCC:

```bash
gcc -std=c11 -Iinc src/main.c src/json_parser.c src/json_lexer.c src/array_list.c src/json_explorer.c -o json_parser
./json_parser
```

Or using Clang:

```bash
clang -std=c11 -Iinc src/main.c src/json_parser.c src/json_lexer.c src/array_list.c src/json_explorer.c -o json_parser
./json_parser
```

---

## Building & Running on Windows

### Method 1: MinGW-w64 / MSYS2 (Recommended)

Open a **MinGW64 / UCRT64** terminal (or PowerShell with MinGW `bin` added to your `PATH`):

#### Using CMake:

1. **Generate build files (using MinGW Makefiles or Ninja):**
   ```cmd
   cmake -B build -G "MinGW Makefiles"
   ```
   *(Or `-G Ninja` if you have Ninja installed)*

2. **Build:**
   ```cmd
   cmake --build build
   ```

3. **Run from the project root:**
   ```cmd
   .\build\json_parser.exe
   ```

#### Direct Compilation:

```cmd
gcc -std=c11 -Iinc src/main.c src/json_parser.c src/json_lexer.c src/array_list.c src/json_explorer.c -o json_parser.exe
.\json_parser.exe
```

---

### Method 2: Visual Studio / MSVC

Open the **x64 Native Tools Command Prompt for VS 2022** (or VS Developer PowerShell):

#### Using CMake with Visual Studio generator:

1. **Configure:**
   ```cmd
   cmake -B build
   ```

2. **Build (Multi-configuration generator requires `--config`):**
   ```cmd
   cmake --build build --config Debug
   ```

3. **Run from the project root:**
   ```cmd
   .\build\Debug\json_parser.exe
   ```

> **Compiler Compatibility Note for MSVC:**
> The code currently uses C99 variable-length arrays (`char str[length];`) in `src/json_lexer.c`. MSVC's standard C compiler (`cl.exe`) treats VLAs as unsupported (compiler error `C2057`). If using Visual Studio, configure CMake to use `Clang-cl` or build using the MinGW-w64 GCC toolchain.

---

### Method 3: Windows Subsystem for Linux (WSL)

If you have WSL (Ubuntu) installed on Windows:

1. Open your WSL terminal in the project directory:
   ```bash
   cd /mnt/c/path/to/c_json
   ```
2. Follow the standard [Linux build instructions](#building--running-on-linux):
   ```bash
   cmake -B build
   cmake --build build
   ./build/json_parser
   ```

---

## Important Runtime Notes

1. **File Location (`test.json`)**:
   `src/main.c` contains:
   ```c
   const char filename[10] = "test.json";
   FILE *fptr = fopen(filename, "r");
   ```
   Because the file path is relative (`"test.json"`), the executable must be run with the project root as the current working directory, or `test.json` must be present in the directory from which you execute the binary.

2. **`CMakeLists.txt` Languages**:
   `CMakeLists.txt` defines `project(C_JSON_PARSER LANGUAGES C ASM)`. Since all source files are standard `.c`, the `ASM` language is not strictly required. If your Windows toolchain raises an error detecting an assembler, you can safely remove `ASM` from the `project()` declaration in `CMakeLists.txt`.

---

## Cleaning Build Artifacts

To remove build files and start fresh:

- **Linux / macOS / Git Bash / WSL**:
  ```bash
  rm -rf build
  ```

- **Windows PowerShell**:
  ```powershell
  Remove-Item -Recurse -Force build
  ```

- **Windows Command Prompt (cmd)**:
  ```cmd
  rmdir /s /q build
  ```

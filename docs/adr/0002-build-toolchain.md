# ADR 0002: Build Toolchain Policy

## Context
Tasqly is a cross-platform Qt Quick application targeting both Windows and Linux.  
To ensure consistent builds across developers and CI environments, we need a standardized build toolchain policy.  
A mismatch between compilers and Qt builds (e.g., MinGW vs MSVC) can cause ABI conflicts.  
Additionally, sanitizers are not universally available on all platforms.

## Decision
- **Windows (Baseline):**
  - Compiler: MinGW-w64 GCC (≥13, currently 15.2 via MSYS2)
  - Build system: CMake + Ninja
  - Qt SDK: Qt 6.8.2 MinGW 64-bit

- **Linux:**
  - Compilers: GCC (latest stable) and Clang (latest stable)
  - Build system: CMake + Ninja

- **Optional:**
  - MSVC 2022 (x64) supported in CI for compatibility checks, not baseline.

### Qt Libraries Policy
- Qt libraries must match the compiler toolchain.
  - Qt-MinGW ↔ MinGW GCC only
  - Qt-MSVC ↔ MSVC only
- Mixing Qt-MinGW with MSVC binaries is not supported (ABI conflict).

### Presets & Configurations (CMakePresets.json)
- Windows MinGW (Baseline):
  - mingw-debug → Debug build (no sanitizers on Windows)
  - mingw-release → Optimized Release build
- Linux GCC/Clang:
  - gcc-debug, gcc-release
  - clang-debug, clang-release
- Optional MSVC:
  - msvc-debug, msvc-release

### CI/CD Matrix
- Ubuntu-latest:
  - Build & test with GCC (baseline)
  - Build & test with Clang (baseline)
- Windows-latest:
  - Build & test with MinGW-w64 GCC (baseline)
  - Build & test with MSVC 2022 (optional compatibility check)

### Constraints
- Qt-MinGW and Qt-MSVC are not ABI compatible.
- Sanitizers (-fsanitize=address,undefined) enabled only on Linux (GCC/Clang).
- On Windows MinGW, sanitizers are disabled (not available).

## Consequence
- Developers and CI jobs use consistent compilers across platforms.
- Baseline builds are reproducible and predictable.
- CI validates both baseline (MinGW/GCC/Clang) and optional (MSVC).
- Clear avoidance of ABI conflicts ensures reliable Qt integration.

# 🏗️ Building Tasqly (Reorganized)

Tasqly uses **CMake Presets** + **Toolchain files** for consistent cross-platform builds.  
This document is the **single reference** for building, testing, and generating reports locally or in CI.

---

## 🚀 Quick Start (Most Common Flow)

```bash
# Clean build directory
rm -rf build

# Configure (Linux: gcc/clang, Windows: mingw/msvc)
cmake --preset=mingw-debug -DENABLE_COVERAGE=ON

# Build
cmake --build --preset=build-mingw-debug --parallel

# Run all tests
ctest --preset=test-mingw-debug --output-on-failure

# Run benchmarks
cmake --build --preset=build-mingw-debug --target TasqlyBenchmarksRunner.exe

# Generate coverage reports
cmake --build --preset=build-mingw-debug --target coverage
```

---

## ⚙️ Requirements

### Linux
- GCC or Clang
- CMake >= 3.25
- Ninja
- Qt 6.9.2 (auto-installed in CI, install manually if local)

### Windows
- [MSYS2](https://www.msys2.org/) with MinGW64 (baseline, cross-platform)
- Visual Studio 2019/2022 (for optional MSVC-only branch)
- CMake >= 3.25
- Ninja (for MinGW builds)
- Qt 6.9.2 (install matching ABI: `mingw_64` for MinGW, `msvc2019_64` or `msvc2022_64` for MSVC)

---

## 📂 Directory Structure

```
Tasqly_V1/
  cmake/
    toolchains/
      mingw.cmake       # Toolchain file for Windows MinGW
  CMakePresets.json     # Unified presets for all platforms
  reports/              # Reports output (tests, benchmarks, coverage)
```

---

## 🔘 CMake Options

| Option              | Default | Description                                |
|---------------------|---------|--------------------------------------------|
| ENABLE_CPP_TESTS    | ON      | GoogleTest unit/integration tests          |
| ENABLE_BENCHMARKS   | ON      | Google Benchmarks                          |
| ENABLE_QML_TESTS    | OFF     | QML UI tests (Phase 1+)                    |
| ENABLE_E2E_TESTS    | OFF     | End-to-End tests (Phase 2+)                |
| ENABLE_STRESS_TESTS | OFF     | Stress tests (Phase 3+)                    |
| ENABLE_COVERAGE     | OFF     | Generate coverage reports                  |

---

## 🔧 Local Builds

### Linux (GCC / Clang)
```bash
rm -rf build
cmake --preset=gcc-debug    # or clang-debug
cmake --build --preset=build-gcc-debug
ctest --preset=test-gcc-debug --output-on-failure
```

### Windows (MSYS2 MinGW64 shell)
```bash
# Use MSYS2 MinGW64 shell ONLY (not PowerShell/cmd)
export PATH="/mingw64/bin:$PATH"
export QT_PREFIX="/mingw64"

rm -rf build
cmake --preset=mingw-debug
cmake --build --preset=build-mingw-debug --parallel
ctest --preset=test-mingw-debug --output-on-failure
```

### Windows (MSVC, optional)
```powershell
# Use x64 Native Tools Command Prompt for VS 2022 shell
cd C:\Users\Olavenko\Desktop\TasqlyQt\Tasqly_V1
set "CMAKE_PREFIX_PATH=C:\Qt\6.9.2\msvc2022_64"
set PATH=C:\Qt\6.9.2\msvc2022_64\bin;%PATH%
cmake --preset=msvc-debug
cmake --preset=msvc-release
cmake --build --preset=build-msvc-debug --config Debug --parallel
cmake --build --preset=build-msvc-debug --config Debug --target TasqlyTestsRunner --parallel
cmake --build --preset=build-msvc-debug --config Debug --target TasqlyBenchmarksRunner --parallel
cmake --build --preset=build-msvc-release --config Release --parallel
cmake --build --preset=build-msvc-release --config Release --target TasqlyTestsRunner --parallel
cmake --build --preset=build-msvc-release --config Release --target TasqlyBenchmarksRunner --parallel
ctest --preset=test-msvc-release -R FallbackOnRotationFailure -VV ## to run single test in file
ctest --preset=test-msvc-release -R RotationPolicyTests -VV ## to run all file tests
ctest --preset=test-msvc-release --output-on-failure ## to run all tests
ctest --preset=test-msvc-debug --output-on-failure
.\build\msvc-debug\appTasqly_V1.exe
```

---

## 🧪 Tests & Reports

### GoogleTest
```bash
ctest --preset=test-mingw-debug --output-on-failure
# Output: reports/tests/gtest/gtest_report.xml
```

### Benchmarks
```bash
cmake --build --preset=build-mingw-debug --target run_benchmarks
# Output: reports/benchmarks/benchmarks.json
```

### Coverage (MinGW → lcov)
```bash
cmake --preset=mingw-debug -DENABLE_COVERAGE=ON
cmake --build --preset=build-mingw-debug --target coverage
# Output: reports/coverage/html/index.html
```

### Coverage (MSVC → OpenCppCoverage)
OpenCppCoverage works **only with MSVC builds** (it requires PDB debug symbols).

```powershell
# Open MSVC Developer Command Prompt (x64)
set "CMAKE_PREFIX_PATH=C:\Qt\6.9.2\msvc2022_64"
set PATH=C:\Qt\6.9.2\msvc2022_64\bin;%PATH%

# Clean and configure
rmdir /S /Q build
cmake --preset=msvc-debug -DENABLE_COVERAGE=ON

# Build and run coverage
cmake --build build/msvc-debug --config Debug --target coverage

# Reports:
# HTML → reports/coverage/html/index.html
# XML  → reports/coverage/coverage.xml
```

⚠️ On MinGW/Clang, OpenCppCoverage will not work (no PDB symbols). Use `lcov` instead.

---

## 🤖 Continuous Integration (CI)

- **Linux (GCC + Clang)**:
  - Installs Qt via `jurplel/install-qt-action@v3`
  - Runs `cmake --preset=gcc-debug` / `clang-debug`
- **Windows (MinGW)**:
  - Uses MSYS2 MinGW64 toolchain
  - Installs Qt (6.9.2, win64_mingw)
  - Runs `cmake --preset=mingw-debug`
- **Windows (MSVC)**:
  - Uses Visual Studio Build Tools (2022), x64 Native Tools Command Prompt for VS 2022
  - Installs Qt (6.9.2, msvc2019_64 or msvc2022_64)
  - Runs `cmake --preset=msvc-debug`

Both Linux & Windows CI print:
- `Qt6_DIR`
- `CMAKE_PREFIX_PATH`
- `qmake --version`

---

## 🛠️ Troubleshooting

### 1. Wrong compiler detected (e.g., FPC `gcc.exe`)
Ensure MSYS2 MinGW64 comes first in your PATH:
```bash
where gcc
where g++
```
They must point to `C:/msys64/mingw64/bin`.

### 2. CMake cannot find Qt6
- Linux: set `CMAKE_PREFIX_PATH` or install Qt.
- Windows MinGW: ensure `C:/Qt/6.9.3/mingw_64/bin` (or `/mingw64/bin`) is in PATH.
- Windows MSVC: ensure `msvc2019_64/bin` or `msvc2022_64/bin` is in PATH.

### 3. Wrong shell on Windows
- MinGW builds → **MSYS2 MinGW64 shell**
- MSVC builds → **Developer Command Prompt** or PowerShell

---

## ✅ Summary

- Use **presets** → `cmake --preset=...`
- Control features with **CMake Options** (tests, benchmarks, coverage)
- Windows MinGW → build inside **MSYS2 MinGW64 shell**
- Windows MSVC → build inside **x64 Native Tools Command Prompt for VS 2022**
- Linux → GCC or Clang
- CI covers MinGW (baseline), GCC/Clang, and optional MSVC
- Reports saved in `reports/`:  
  - `tests/gtest/` → XML test reports  
  - `benchmarks/` → JSON benchmark results  
  - `coverage/html/` → HTML coverage dashboard  

---

## 🌀 Parallel Build Environments (MinGW + MSVC)

CMake presets generate **isolated build directories** per toolchain, so you can
configure and build the same codebase for multiple environments **in parallel**
without conflicts.

Example with both MinGW (MSYS2) and MSVC (Visual Studio):

```bash
# MSYS2 MinGW64 shell
cmake --preset=mingw-debug
cmake --build --preset=build-mingw-debug --parallel
ctest --preset=test-mingw-debug --output-on-failure
```

```bat
:: Visual Studio Developer Command Prompt (x64)
cmake --preset=msvc-debug -DCMAKE_PREFIX_PATH="C:\Qt\6.9.2\msvc2022_64"
cmake --build --preset=build-msvc-debug --config Debug --parallel
ctest --preset=test-msvc-debug --output-on-failure
```

This produces:

```
build/
  mingw-debug/   # MinGW artifacts
  msvc-debug/    # MSVC artifacts
```

- Each subdirectory is fully independent.
- No need to clean or reconfigure when switching between compilers.
- Same source code, tested across multiple environments.

---

### 📝 Note: Why only MinGW has a Toolchain file?

- **MinGW (MSYS2)** requires an explicit toolchain file (`cmake/toolchains/mingw.cmake`)
  because CMake may otherwise confuse `gcc`/`g++` with other compilers (e.g. FPC).
  This ensures we always pick the correct MinGW compilers from MSYS2.

- **MSVC (Visual Studio)** does **not** need a toolchain file.  
  When you open the **Developer Command Prompt (x64)**, CMake automatically
detects `cl.exe` and sets up the environment correctly.  
  Keeping MSVC without a toolchain file avoids extra maintenance and follows
the default CMake workflow.

In summary:
- 🟢 MinGW → always use `mingw.cmake`
- 🟢 MSVC → rely on the Developer Command Prompt (no toolchain file needed)
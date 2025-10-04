# 🏗️ Building Tasqly (Reorganized)

Tasqly uses **CMake Presets** + **Toolchain files** for consistent cross-platform builds.  
This document is the **single reference** for building, testing, benchmarking, and generating reports locally or in CI.

---

## 🚀 Quick Start

```bash
# Clean build directory
rm -rf build

# Configure (Linux: gcc/clang, Windows: mingw/msvc)
cmake --preset=msvc-tests-reports-release -DENABLE_COVERAGE=ON

# Build with coverage
cmake --build --preset=build-msvc-tests-reports-release --target coverage --parallel
```

---

## ⚙️ Requirements

### Linux
- GCC or Clang
- CMake >= 3.25
- Ninja
- Qt 6.9.2

### Windows
- [MSYS2](https://www.msys2.org/) with MinGW64  
- Visual Studio 2019/2022 (for optional MSVC builds)  
- CMake >= 3.25  
- Qt 6.9.2 (matching ABI: `mingw_64` or `msvc2022_64`)  

---

## 📂 Directory Structure

```
Tasqly_V1/
  cmake/
    toolchains/
      mingw.cmake
  CMakePresets.json
  reports/
    coverage/
    benchmarks/
    tests/
```

---

## 🔘 CMake Options

| Option              | Default | Description                                |
|---------------------|---------|--------------------------------------------|
| ENABLE_CPP_TESTS    | ON      | GoogleTest unit/integration tests          |
| ENABLE_BENCHMARKS   | ON      | Google Benchmarks (Release only)           |
| ENABLE_QML_TESTS    | OFF     | QML UI tests (Phase 1+)                    |
| ENABLE_E2E_TESTS    | OFF     | End-to-End tests (Phase 2+)                |
| ENABLE_STRESS_TESTS | OFF     | Stress tests (Phase 3+)                    |
| ENABLE_COVERAGE     | OFF     | Generate coverage reports                  |

---

## 🔧 Local Builds

### Linux (GCC / Clang)
```bash
rm -rf build
cmake --preset=gcc-debug
cmake --build --preset=build-gcc-debug --parallel
ctest --preset=test-gcc-debug --output-on-failure
```

### Windows (MSYS2 MinGW64 shell)
```bash
export PATH="/mingw64/bin:$PATH"
export QT_PREFIX="/mingw64"

rm -rf build
cmake --preset=mingw-tests-reports-release -DENABLE_COVERAGE=ON
cmake --build --preset=build-mingw-tests-reports-release --target coverage --parallel
```

### Windows (MSVC)
```powershell
set "CMAKE_PREFIX_PATH=C:\Qt.9.2\msvc2022_64"
set PATH=C:\Qt.9.2\msvc2022_64in;%PATH%

# Tests Coverage (Debug)
cmake --preset=msvc-tests-reports-debug -DENABLE_COVERAGE=ON
cmake --build --preset=build-msvc-tests-reports-debug --target coverage --parallel

# Tests Coverage (Release)
cmake --preset=msvc-tests-reports-release -DENABLE_COVERAGE=ON
cmake --build --preset=build-msvc-tests-reports-release --target coverage --parallel

# Benchmarks Coverage (Release only)
cmake --preset=msvc-benchmarks-reports-release -DENABLE_COVERAGE=ON
cmake --build --preset=build-msvc-benchmarks-reports-release --target coverage --parallel
```

---

## 🧪 Tests & Reports

### GoogleTest
```bash
ctest --preset=test-mingw-debug --output-on-failure
# → reports/tests/gtest_report.xml
```

### Benchmarks
```bash
cmake --build --preset=build-msvc-release --target TasqlyBenchmarksRunner --parallel
# → reports/benchmarks/benchmarks.json
```

### Coverage
- **MSVC → OpenCppCoverage**
- **MinGW/Clang → lcov**

Outputs:
```
reports/coverage/html/index.html
reports/coverage/coverage.xml
```

---

## 🤖 Continuous Integration (CI)

- **Linux (GCC + Clang)** → baseline coverage + tests.  
- **Windows (MinGW)** → default CI builds.  
- **Windows (MSVC)** → optional branch, with OpenCppCoverage.  

Artifacts produced:
- `reports/tests/`  
- `reports/benchmarks/`  
- `reports/coverage/`  

---

## ✅ Summary

- Use **presets** to build and test consistently.  
- **Benchmarks always Release only**.  
- Reports split into `tests`, `benchmarks`, and `coverage`.  
- CI runs coverage jobs separately for Tests + Benchmarks.  

---

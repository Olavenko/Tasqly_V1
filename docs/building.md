# 🏗️ Building Tasqly

Tasqly uses **CMake Presets** + **Toolchain files** to ensure consistent builds across Linux and Windows.  
This document explains how to build locally and how CI is configured.

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
```

---

## 🔧 Local Build

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
export QT_PREFIX="/mingw64"   # Or point to your official Qt install root

rm -rf build
cmake --preset=mingw-debug
cmake --build --preset=build-mingw-debug --parallel
cmake --build --preset=build-mingw-debug --target appTasqly_V1 --parallel
cmake --build --preset=build-mingw-debug --target TasqlyTestsRunner --parallel
cmake --build --preset=build-mingw-debug --target TasqlyBenchmarksRunner --parallel
ctest --preset=test-mingw-debug -R FallbackOnRotationFailure -VV ## to run single test in file
ctest --preset=test-mingw-debug -R RotationPolicyTests -VV ## to run all file tests
ctest --preset=test-mingw-debug --output-on-failure ## to run all tests

cmake --preset=mingw-release
cmake --build --preset=build-mingw-release --parallel
cmake --build --preset=build-mingw-release --target appTasqly_V1 --parallel
cmake --build --preset=build-mingw-release --target TasqlyTestsRunner --parallel
cmake --build --preset=build-mingw-release --target TasqlyBenchmarksRunner --parallel
ctest --preset=test-mingw-release -R FallbackOnRotationFailure -VV ## to run single test in file
ctest --preset=test-mingw-release -R RotationPolicyTests -VV ## to run all file tests
ctest --preset=test-mingw-release --output-on-failure ## to run all tests
```

### Windows (MSVC, optional)
```powershell
# use x64 Native Tools Command Prompt for VS 2022 shell
cd C:\Users\Olavenko\Desktop\TasqlyQt\Tasqly_V1
set "CMAKE_PREFIX_PATH=C:\Qt\6.9.2\msvc2022_64"
set PATH=C:\Qt\6.9.2\msvc2022_64\bin;%PATH%
rmdir /S /Q build
cmake --preset=msvc-debug
cmake --build --preset=build-msvc-debug --parallel
cmake --build --preset=build-msvc-debug --target appTasqly_V1 --parallel
cmake --build --preset=build-msvc-debug --target TasqlyTestsRunner --parallel
cmake --build --preset=build-msvc-debug --target TasqlyBenchmarksRunner --parallel
ctest --preset=test-msvc-debug -R FallbackOnRotationFailure -VV ## to run single test in file
ctest --preset=test-msvc-debug -R RotationPolicyTests -VV ## to run all file tests
ctest --preset=test-msvc-debug --output-on-failure ## to run all tests

cmake --preset=msvc-release
cmake --build --preset=build-msvc-release --parallel
cmake --build --preset=build-msvc-release --target appTasqly_V1 --parallel
cmake --build --preset=build-msvc-release --target TasqlyTestsRunner --parallel
cmake --build --preset=build-msvc-release --target TasqlyBenchmarksRunner --parallel
ctest --preset=test-msvc-release -R FallbackOnRotationFailure -VV ## to run single test in file
ctest --preset=test-msvc-release -R RotationPolicyTests -VV ## to run all file tests
ctest --preset=test-msvc-release --output-on-failure ## to run all tests
```

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
- Windows MinGW → build inside **MSYS2 MinGW64 shell**
- Windows MSVC → build inside **x64 Native Tools Command Prompt for VS 2022**
- Linux → GCC or Clang
- CI covers MinGW (baseline), GCC/Clang, and optional MSVC

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
cmake --build --preset=build-msvc-debug --parallel
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

# 🏗️ Building Tasqly

Tasqly uses **CMake Presets** + **Toolchain files** to ensure consistent builds across Linux and Windows.  
This document explains how to build locally and how CI is configured.

---

## ⚙️ Requirements

### Linux
- GCC or Clang
- CMake >= 3.25
- Ninja
- Qt 6.9.3 (auto-installed in CI, install manually if local)

### Windows
- [MSYS2](https://www.msys2.org/) with MinGW64 (for cross-platform baseline)
- Visual Studio 2019/2022 (for optional MSVC-only branch)
- CMake >= 3.25
- Ninja (for MinGW builds)
- Qt 6.9.3 (install matching ABI: `mingw_64` for MinGW, `msvc2019_64` or `msvc2022_64` for MSVC)

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
# From project root
rm -rf build
cmake --preset=gcc-debug    # or clang-debug
cmake --build --preset=build-gcc-debug
ctest --preset=test-gcc-debug --output-on-failure
```

### Windows (MSYS2 MinGW64 shell, Cross-Platform Baseline)
```bash
# From project root
rmdir /S /Q build
cmake --preset=mingw-debug
cmake --build --preset=build-mingw-debug
ctest --preset=test-mingw-debug --output-on-failure
```
> **Note:** Ensure you are inside the **MSYS2 MinGW64 shell**, not PowerShell.

### Windows (Optional: MSVC + Qt-MSVC, Windows-only Branch)
```powershell
# From project root (Visual Studio Developer Command Prompt or PowerShell)
rmdir /S /Q build
cmake --preset=msvc-debug
cmake --build --preset=msvc-debug
ctest --preset=msvc-debug
```
> **Note:** MSVC builds must be run from Visual Studio Developer Command Prompt or PowerShell, not MSYS2.

---

## 🧰 Toolchain (MinGW)

File: `cmake/toolchains/mingw.cmake`

```cmake
# Compilers (use from PATH)
set(CMAKE_C_COMPILER gcc CACHE STRING "C compiler" FORCE)
set(CMAKE_CXX_COMPILER g++ CACHE STRING "C++ compiler" FORCE)

# System Root
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Build Options
set(CMAKE_CXX_EXTENSIONS OFF CACHE BOOL "Disable compiler-specific extensions" FORCE)
```

This ensures:
- CMake uses `gcc` and `g++` from PATH (avoids conflicts with FPC or MSVC).
- Consistent build setup for CI and local environments.

---

## 🤖 Continuous Integration (CI)

File: `.github/workflows/ci.yml`

- **Linux (GCC + Clang)**:
  - Installs Qt using `jurplel/install-qt-action@v3`
  - Runs `cmake --preset=gcc-debug` / `clang-debug`
- **Windows (MinGW)**:
  - Sets up MSYS2 + MinGW64
  - Installs Qt (6.9.3, win64_mingw)
  - Runs `cmake --preset=mingw-debug` from **MSYS2 shell**
- **Windows (MSVC)**:
  - Installs Visual Studio Build Tools (2019/2022)
  - Installs Qt (6.9.3, msvc2019_64 or msvc2022_64)
  - Runs `cmake --preset=msvc-debug` from **Developer Command Prompt**

### Debug Info in CI
Both Linux & Windows CI print:
- `Qt6_DIR`
- `CMAKE_PREFIX_PATH`
- `qmake --version`

This helps troubleshoot if Qt is not found.

---

## 🛠️ Troubleshooting

### 1. Wrong compiler detected (e.g., FPC `gcc.exe`)
**Symptom:**
```
C:/FPC/.../bin/i386-win32/gcc.exe: cannot exec `cc1`: No such file or directory
```
**Fix:**  
Make sure `C:/msys64/mingw64/bin` comes **first** in your PATH.  
Run:
```bash
where gcc
where g++
```
and confirm they point to MSYS2 MinGW.

---

### 2. CMake cannot find Qt6
**Symptom:**
```
Could not find a package configuration file provided by "Qt6"
```
**Fix:**  
- On Linux: install Qt with `install-qt-action` or set `CMAKE_PREFIX_PATH` to your Qt install.  
- On Windows MinGW: ensure `C:/Qt/6.9.3/mingw_64/bin` is in PATH.  
- On Windows MSVC: ensure `C:/Qt/6.9.3/msvc2019_64/bin` (or `msvc2022_64/bin`) is in PATH.

---

### 3. Using the wrong shell on Windows
**Symptom:** Build fails with "compiler not found".  
**Fix:**  
- For MinGW builds: Always use **MSYS2 MinGW64 shell**, not PowerShell or cmd.exe.  
- For MSVC builds: Always use **Visual Studio Developer Command Prompt** or PowerShell, not MSYS2.

---

## ✅ Summary

- Use **presets** → `cmake --preset=...`
- **Windows (MinGW)** builds must run inside **MSYS2 MinGW64 shell**
- **Windows (MSVC)** builds must run inside **Visual Studio Developer Command Prompt** or PowerShell
- **Linux** builds work with GCC or Clang
- CI covers both cross-platform baseline (MinGW + GCC/Clang) and optional Windows-only MSVC


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
- [MSYS2](https://www.msys2.org/) with MinGW64 (baseline, cross-platform)
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
cmake --build --preset=build-mingw-debug
ctest --preset=test-mingw-debug --output-on-failure
```

### Windows (MSVC, optional)
```powershell
# Run from Visual Studio Developer Command Prompt or PowerShell
rmdir /S /Q build
cmake --preset=msvc-debug
cmake --build --preset=msvc-debug
ctest --preset=msvc-debug
```

---

## 🤖 Continuous Integration (CI)

- **Linux (GCC + Clang)**:
  - Installs Qt via `jurplel/install-qt-action@v3`
  - Runs `cmake --preset=gcc-debug` / `clang-debug`
- **Windows (MinGW)**:
  - Uses MSYS2 MinGW64 toolchain
  - Installs Qt (6.9.3, win64_mingw)
  - Runs `cmake --preset=mingw-debug`
- **Windows (MSVC)**:
  - Uses Visual Studio Build Tools (2019/2022)
  - Installs Qt (6.9.3, msvc2019_64 or msvc2022_64)
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
- Windows MSVC → build inside **Developer Command Prompt** or PowerShell
- Linux → GCC or Clang
- CI covers MinGW (baseline), GCC/Clang, and optional MSVC


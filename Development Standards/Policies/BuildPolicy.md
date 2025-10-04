# 🏗️ Build Policy — Tasqly

## 🎯 Purpose
This document defines the **official Build Policy** for the Tasqly project.  
The goal is to ensure:
- Consistent build process across different platforms (Linux / Windows).  
- Clear reference for the official **CI/CD (GitHub Actions)** workflow.  
- Additional options for developers working on other environments (MSVC, etc.).  

---

## 📌 Baseline Build Policy

### 🟢 Windows (Baseline — MinGW)
- **Environment**: MSYS2 MinGW64 + Ninja.  
- **Qt**: ABI-compatible version (`mingw_64`).  
- **Toolchain**: `cmake/toolchains/mingw.cmake`.  
- **Official Presets**: `mingw-debug` / `mingw-release`.  
- **Core Commands**:
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

### 🟢 Linux (Baseline)
- **Compilers**: GCC or Clang (latest stable).  
- **Qt**: ABI-compatible with MinGW.  
- **Official Presets**: `gcc-debug` / `clang-debug`.  
- **Core Commands**:
  ```bash
  cmake --preset=gcc-debug    # or clang-debug
  cmake --build --preset=build-gcc-debug
  ctest --preset=test-gcc-debug --output-on-failure
  ```

---

## ⚙️ Optional Build Policy

### 🟡 Windows (MSVC-only)
- **Environment**: Visual Studio 2019/2022 (x64 Native Tools Command Prompt).  
- **Qt**: ABI-compatible version (`msvc2019_64` or `msvc2022_64`).  
- **Official Presets**: `msvc-debug` / `msvc-release`.  
- **Core Commands**:
  ```powershell
  cmake --preset=msvc-debug
  cmake --build --preset=build-msvc-debug --parallel
  ctest --preset=test-msvc-debug --output-on-failure
  ```
- 🔴 **Note**: Qt-MinGW and Qt-MSVC **must not be mixed** (⚠️ ABI conflict).  

---

## 🚦 CI Integration (GitHub Actions)

### Linux (GCC + Clang)
- Installs Qt using `jurplel/install-qt-action@v3`.  
- Runs builds via Presets: `gcc-debug`, `clang-debug`.  

### Windows (MinGW — Baseline)
- Runs inside **MSYS2 MinGW64 shell**.  
- Uses Preset: `mingw-debug`.  

### Windows (MSVC — Optional)
- Runs inside **x64 Developer Command Prompt**.  
- Uses Preset: `msvc-debug`.  

✅ All CI environments print: `Qt6_DIR`, `CMAKE_PREFIX_PATH`, and `qmake --version`.  

---

## ❌ Constraints & Rules
- Mixing **Qt-MinGW ↔ Qt-MSVC** is strictly forbidden.  
- Sanitizers are enabled **only on Linux** (GCC/Clang).  
- All targets must be built with **C++20** (enforced in `CMakeLists.txt`).  
- Warnings and Sanitizers are applied **per-target** only (to avoid affecting external dependencies).  

---

## 🛠️ Developer Guide (Quick Reference)

### Run the Application (MinGW — Official)
```bash
cmake --preset=mingw-debug
cmake --build --preset=build-mingw-debug --target appTasqly_V1 --parallel
```

### Run Tests
```bash
ctest --preset=test-mingw-debug --output-on-failure
```

### Parallel Builds (MinGW + MSVC)
```bash
# Inside MSYS2 MinGW64 shell
cmake --preset=mingw-debug
cmake --build --preset=build-mingw-debug

# Inside Visual Studio Developer Command Prompt (x64)
cmake --preset=msvc-debug
cmake --build --preset=build-msvc-debug
```
- Each environment builds into `build/<preset-name>/` independently.  
- No need to clean or reconfigure when switching toolchains.  

---

## 📦 Toolchain Notes
- **MinGW**: Requires a Toolchain file (`cmake/toolchains/mingw.cmake`) to guarantee correct compiler selection from MSYS2.  
- **MSVC**: Does not require a Toolchain file — the Developer Command Prompt configures `cl.exe` automatically.  

---

## ✅ Summary
- 🟢 **Baseline**: Windows MinGW + Linux GCC/Clang.  
- 🟡 **Optional**: Windows MSVC.  
- 🤖 **CI**: Built on GitHub Actions (MinGW + GCC/Clang).  
- 🔒 **Constraints**: Qt ABI strict, C++20 enforced, Sanitizers Linux-only.  
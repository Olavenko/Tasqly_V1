# 🧪 Tasqly — Test Execution Cheat Sheet

## 📌 Overview
This cheat sheet provides quick commands to run **GoogleTest-based tests**  
for Tasqly using **CTest** with predefined CMake presets.

Presets used:
- `test-msvc-debug` (Run from **x64 Native Tools Command Prompt**)
- `test-msvc-release` (Run from **x64 Native Tools Command Prompt**)
- `test-mingw-debug` (Run from **Cmder / MSYS2 shell**)
- `test-mingw-release` (Run from **Cmder / MSYS2 shell**)

---

## 🚀 Run All Tests

### 🔹 MSVC — Debug
```bash
cd C:\Users\Olavenko\Desktop\TasqlyQt\Tasqly_V1
set "CMAKE_PREFIX_PATH=C:\Qt\6.9.2\msvc2022_64"
set PATH=C:\Qt\6.9.2\msvc2022_64\bin;%PATH%
cmake --preset=msvc-debug
cmake --build --preset=build-msvc-debug --config Debug --parallel
cmake --build --preset=build-msvc-debug --config Debug --target TasqlyTestsRunner --parallel
cmake --build --preset=build-msvc-debug --config Debug --target TasqlyBenchmarksRunner --parallel
ctest --preset=test-msvc-debug --output-on-failure
ctest --preset=test-msvc-debug -R FallbackOnRotationFailure -VV ## to run single test in file
ctest --preset=test-msvc-debug -R RotationPolicyTests -VV ## to run all file tests
ctest --preset=test-msvc-debug --output-on-failure ## to run all tests
---
PowerShell 7.5.3
$env:CMAKE_PREFIX_PATH = "C:\Qt\6.9.2\msvc2022_64"
$env:PATH = "C:\Qt\6.9.2\msvc2022_64\bin;" + $env:PATH
cmake --preset=msvc-debug
cmake --build --preset=build-msvc-debug --config Debug --parallel
cmake --build --preset=build-msvc-debug --config Debug --target TasqlyTestsRunner --parallel
cmake --build --preset=build-msvc-debug --config Debug --target TasqlyBenchmarksRunner --parallel
```

### 🔹 MSVC — Release
```bash
cd C:\Users\Olavenko\Desktop\TasqlyQt\Tasqly_V1
set "CMAKE_PREFIX_PATH=C:\Qt\6.9.2\msvc2022_64"
set PATH=C:\Qt\6.9.2\msvc2022_64\bin;%PATH%
cmake --preset=msvc-release
cmake --build --preset=build-msvc-release --config Release --parallel
cmake --build --preset=build-msvc-release --config Release --target TasqlyTestsRunner --parallel
cmake --build --preset=build-msvc-release --config Release --target TasqlyBenchmarksRunner --parallel
ctest --preset test-msvc-release --output-on-failure
ctest --preset=test-msvc-release -R FallbackOnRotationFailure -VV ## to run single test in file
ctest --preset=test-msvc-release -R RotationPolicyTests -VV ## to run all file tests
ctest --preset=test-msvc-release --output-on-failure ## to run all tests
```

### 🔹 MinGW — Debug
```bash
ctest --preset test-mingw-debug
```

### 🔹 MinGW — Release
```bash
ctest --preset test-mingw-release
```

---

## 🎯 Run Tests by Name (Regex Filter)

### Example: Run tests matching `LogManager`
```bash
ctest --preset test-msvc-release -R LogManager
```

### Example: Run single test `FallbackOnRotationFailure`
```bash
ctest --preset test-msvc-release -R FallbackOnRotationFailure
```

---

## 🔍 Verbose Output

Add `-VV` to see **full test logs** (including GoogleTest output):

### Example:
```bash
ctest --preset test-msvc-release -R FallbackOnRotationFailure -VV
```

---

## 📦 Combined Usage

| Mode          | Run all 							 | Run by name 							          | Verbose                                 |
|---------------|------------------------------------|------------------------------------------------|-----------------------------------------|
| MSVC Debug    | `ctest --preset test-msvc-debug`   | `ctest --preset test-msvc-debug -R TestName`   | `ctest --preset test-msvc-debug -VV`    |
| MSVC Release  | `ctest --preset test-msvc-release` | `ctest --preset test-msvc-release -R TestName` | `ctest --preset test-msvc-release -VV`  |
| MinGW Debug   | `ctest --preset test-mingw-debug`  | `ctest --preset test-mingw-debug -R TestName`  | `ctest --preset test-mingw-debug -VV`   |
| MinGW Release | `ctest --preset test-mingw-release`| `ctest --preset test-mingw-release -R TestName`| `ctest --preset test-mingw-release -VV` |

---

✅ Use **x64 Native Tools Command Prompt** for MSVC builds  
✅ Use **Cmder/MSYS2 shell** for MinGW builds

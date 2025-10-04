# 🧱 Tasqly Build & Test Cheat Sheet

## 🟢 MinGW 64-bit (MSYS2)

### 🔨 Build
- **Build all (incremental):**
  ```bash
  cmake --build --preset=build-mingw-debug --parallel
  ```

- **Build application only:**
  ```bash
  set PATH=C:\msys64\mingw64\bin;%PATH%
  cmake --build --preset=build-mingw-debug --target appTasqly_V1 --parallel
  ```

- **Run application only:**
  ```bash
  set PATH=C:\msys64\mingw64\bin;%PATH%
  .\build\mingw-debug\appTasqly_V1.exe
  .\build\mingw-debug\TasqlyTests.exe
  ```

- **Build tests only:**
  ```bash
  set PATH=C:\msys64\mingw64\bin;%PATH%
  cmake --build --preset=build-mingw-debug --target TasqlyTests --parallel
  ```

### 🧪 Run Tests
- **Run all tests:**
  ```bash
  ctest --preset=test-mingw-debug --output-on-failure
  ```

- **Run one test suite (all tests in file/class):**
  ```bash
  ctest --preset=test-mingw-debug -R RotationPolicyTests -VV
  ```

- **Run one test case only:**
  ```bash
  ctest --preset=test-mingw-debug -R FallbackOnRotationFailure -VV
  ```

- **Run directly via GTest binary (advanced):**
  ```bash
  .\build\mingw-debug\TasqlyTests.exe --gtest_filter=RotationPolicyTests.FallbackOnRotationFailure
  .\build\mingw-debug\TasqlyTests.exe --gtest_list_tests   # list all
  ```

---

## 🟣 MSVC 64-bit 2022

### 🔨 Build
- **Build all (incremental):**
  ```powershell
  cmake --build --preset=build-msvc-debug --parallel
  ```

- **Build application only:**
  ```powershell
  set "CMAKE_PREFIX_PATH=C:\Qt\6.9.2\msvc2022_64"
  set PATH=C:\Qt\6.9.2\msvc2022_64\bin;%PATH%
  cmake --build --preset=build-msvc-debug --target appTasqly_V1 --parallel
  ```

- **Run application only:**
  ```bash
  set "CMAKE_PREFIX_PATH=C:\Qt\6.9.2\msvc2022_64"
  set PATH=C:\Qt\6.9.2\msvc2022_64\bin;%PATH%
  .\.\build\msvc-debug\appTasqly_V1.exe
  .\.\build\msvc-debug\TasqlyTests.exe
  ```

- **Build tests only:**
  ```powershell
  set "CMAKE_PREFIX_PATH=C:\Qt\6.9.2\msvc2022_64"
  set PATH=C:\Qt\6.9.2\msvc2022_64\bin;%PATH%
  cmake --build --preset=build-msvc-debug --target TasqlyTests --parallel
  ```

### 🧪 Run Tests
- **Run all tests:**
  ```powershell
  ctest --preset=test-msvc-debug --output-on-failure
  ```

- **Run one test suite (all tests in file/class):**
  ```powershell
  ctest --preset=test-msvc-debug -R RotationPolicyTests -VV
  ```

- **Run one test case only:**
  ```powershell
  ctest --preset=test-msvc-debug -R FallbackOnRotationFailure -VV
  ```

- **Run directly via GTest binary (advanced):**
  ```powershell
  .\build\msvc-debug\TasqlyTests.exe --gtest_filter=RotationPolicyTests.FallbackOnRotationFailure
  .\build\msvc-debug\TasqlyTests.exe --gtest_list_tests   # list all
  ```

---

## 📌 Notes
- Always prefer **incremental build** instead of deleting `build/`.
- Use `--target` to build **only what you need** (app or tests).
- Use `-R` with `ctest` to filter by suite/test name.
- Use `--gtest_filter` with the binary directly for fine-grained debugging.
- For release builds, replace `*-debug` with `*-release` presets.

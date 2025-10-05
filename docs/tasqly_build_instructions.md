# Tasqly Build Instructions (MinGW + MSVC)

This document provides ready-to-use build commands for **Debug**, **Release**, **Reports**, and **Benchmarks** 
using both **MinGW** and **MSVC** toolchains. All commands use CMake presets and are safe to copy-paste.

---

## 🟢 MinGW Build Commands

### 1. Debug build (default daily development test + app)
```bash

cd C:\Users\Olavenko\Desktop\TasqlyQt\Tasqly_V1

set "CMAKE_PREFIX_PATH=C:\Qt\6.9.2\mingw_64"
set PATH=C:\Qt\6.9.2\mingw_64\bin;%PATH%

cd /c/Users/Olavenko/Desktop/TasqlyQt/Tasqly_V1
./scripts/check-format.sh

cmake --preset=mingw-debug
cmake --build --preset=build-mingw-debug --parallel

# Run unit tests
ctest --preset=test-mingw-debug --output-on-failure ## to run all tests
ctest --preset=test-mingw-debug -R WriteFailureDuringLoadThrows -VV ## to run single test in file
ctest --preset=test-mingw-debug -R AppSettingsManagerTest -VV ## to run all file tests
```

### 2. Benchmarks build (performance benchmarks, always Release mode)
```bash

cd /c/Users/Olavenko/Desktop/TasqlyQt/Tasqly_V1
./scripts/check-format.sh

cmake --preset=mingw-benchmarks-release
cmake --build --preset=build-mingw-benchmarks-release --parallel
py -3.13 scripts/run_benchmarks.py --phase Phase0 --compiler mingw --timeout 1800

```

---

## 🔵 MSVC Build Commands

### 1. Debug build (default daily development test + app)
```bash

cd C:\Users\Olavenko\Desktop\TasqlyQt\Tasqly_V1

set "CMAKE_PREFIX_PATH=C:\Qt\6.9.2\msvc2022_64"
set PATH=C:\Qt\6.9.2\msvc2022_64\bin;%PATH%

cd /c/Users/Olavenko/Desktop/TasqlyQt/Tasqly_V1
./scripts/check-format.sh

cmake --preset=msvc-debug
cmake --build --preset=build-msvc-debug --parallel

# Run unit tests
ctest --preset=test-msvc-debug --output-on-failure ## to run all tests
ctest --preset=test-msvc-debug -R AppContextTest -VV ## to run single test in file
ctest --preset=test-msvc-debug -R AppSettingsManagerTest -VV ## to run all file tests
```

### 2. Reports build (tests + coverage reports)
```bash

cd C:\Users\Olavenko\Desktop\TasqlyQt\Tasqly_V1

set "CMAKE_PREFIX_PATH=C:\Qt\6.9.2\msvc2022_64"
set PATH=C:\Qt\6.9.2\msvc2022_64\bin;%PATH%

git update-index --chmod=+x scripts/check-format.sh
cd /c/Users/Olavenko/Desktop/TasqlyQt/Tasqly_V1
./scripts/check-format.sh

cmake --preset=msvc-tests-reports-debug -DENABLE_COVERAGE=ON
cmake --build --preset=build-msvc-tests-reports-debug --target coverage_tests --parallel
py -3.13 scripts\coverage_report_updated.py reports\coverage\tests\coverage.xml all --archive Phase0
py -3.13 scripts\coverage_report_updated.py reports\coverage\tests\coverage.xml all

```

### 3. Reports build (benchmarks performance)
```bash

cd C:\Users\Olavenko\Desktop\TasqlyQt\Tasqly_V1

set "CMAKE_PREFIX_PATH=C:\Qt\6.9.2\msvc2022_64"
set PATH=C:\Qt\6.9.2\msvc2022_64\bin;%PATH%

cd /c/Users/Olavenko/Desktop/TasqlyQt/Tasqly_V1
./scripts/check-format.sh

cmake --preset=msvc-benchmarks-release
cmake --build --preset=build-msvc-benchmarks-release --parallel
py -3.13 scripts/run_benchmarks.py --phase Phase0 --compiler msvc --timeout 1800

```

---

## ✅ Explanation

- **Debug** → used for daily development (includes unit tests).  
- **Release** → optimized build for deployment (no tests/benchmarks).  
- **Reports** → runs tests **with coverage enabled** (manual step to run ctest).  
- **Benchmarks** → built in Release mode to measure performance accurately (run manually after build).  
- `--parallel` → uses all available CPU cores automatically.

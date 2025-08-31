# 📂 Project Structure — Tasqly_V1

This document describes the current folder structure and baseline build policy  
for the **Tasqly_V1** project. It reflects the Phase 0 setup (Build + Test + Docs + ADRs).

---

## 📑 Table of Contents
1. [Root Layout](#root-layout)
2. [Notes](#notes)
3. [Build Toolchain Policy (Summary)](#build-toolchain-policy-summary)
4. [Next Steps](#next-steps)

---

## Root Layout
```
Tasqly_V1/
 ├── build/                   # Local build output (ignored in VCS)
 ├── src/                     # Application source code
 │    ├── main.cpp            # App entry point (C++20)
 │    └── Main.qml            # Root QML file
 ├── tests/                   # Unit & integration tests
 │    └── test_result.cpp     # Smoke test for Result<T,Error>
 ├── docs/                    # Documentation (UML + ADRs + Build Guides)
 │    ├── adr/                # Architecture Decision Records
 │    │    ├── 0001-plantuml-adoption.md
 │    │    ├── 0002-build-toolchain.md
 │    │    ├── 0003-google-test.md
 │    │    ├── 0004-ci-workflows.md
 │    │    ├── 0005-git-hygiene.md
 │    │    ├── 0006-debugger-policy.md
 │    │    ├── 0007-Build-Toolchain-CI-Policy.md
 │    │    └── README.md      # ADR index
 │    ├── uml/                # UML Diagrams & Config
 │    │    ├── config/        # Shared style + includes
 │    │    │    └── style.puml
 │    │    ├── phase0/        # Phase 0 diagrams
 │    │    │    └── s0/
 │    │    │         ├── component.puml
 │    │    │         ├── sequence.puml
 │    │    │         ├── usecase.puml
 │    │    │         └── exports/      # Generated .svg diagrams
 │    │    ├── tools/        # Local tools (plantuml.jar)
 │    │    ├── uml-export.sh # Export script (SVG + index.html)
 │    │    └── README.md     # UML Authoring Guidelines
 │    └── building.md        # Build instructions (local + CI + troubleshooting)
 ├── cmake/                   # Toolchain & build scripts
 │    └── toolchains/
 │         └── mingw.cmake   # Toolchain definitions (baseline MinGW; MSVC uses built-in generator)
 ├── .clang-format            # Coding style config
 ├── .clang-tidy              # Static analysis config
 ├── .gitignore               # Git ignore rules
 ├── .gitattributes           # Git attributes (line endings, etc.)
 ├── MyAllmanClassic.xml      # Qt Creator formatting preset
 ├── CMakeLists.txt           # Root CMake config
 ├── CMakePresets.json        # Standardized CMake presets (with debuggerPresets)
 ├── .github/
 │    └── workflows/
 │         ├── ci.yml         # CI build & test (multi-platform matrix: Linux + Windows-MinGW; Windows-MSVC optional)
 │         ├── uml.yml        # UML artifact export
 │         └── uml-pages.yml  # UML GitHub Pages deployment
 └── Structure.md             # (This file)
```

---

## Notes
- **src/** → Application entry point (`main.cpp`, `Main.qml`).  
- **tests/** → Unit tests, starting with smoke test for `Result<T,Error>`.  
- **docs/adr/** → ADRs for traceability (toolchain, git hygiene, debugger, etc.).  
- **docs/building.md** → Complete build guide (local & CI + troubleshooting).  
- **docs/uml/** → UML diagrams, with corporate style and CI export (`uml-export.sh`).  
- **.github/workflows/** → CI/CD automation (build + UML diagrams + Pages).  
- **cmake/toolchains/** → Toolchain definitions (baseline MinGW; MSVC uses built-in generator).  

---

## Build Toolchain Policy (Summary)

- **Windows (Baseline):** MSYS2 MinGW-w64 GCC 15.2 + Ninja + Qt-MinGW  
- **Linux (Baseline):** GCC / Clang (latest stable) + Qt-Linux (ABI-compatible with MinGW).  
- **Optional (Windows-only):** MSVC (Visual Studio 2019/2022) + Qt-MSVC, maintained in the `msvc-only` branch.  
- **Qt ABI constraint:**
  - Qt-MinGW ↔ MinGW/GCC/Clang only (cross-platform baseline)  
  - Qt-MSVC ↔ MSVC only (Windows-only branch)  
  - ❌ Mixing Qt-MinGW with MSVC is not supported (ABI conflict).  
- **Sanitizers:** Enabled on Linux only, disabled on Windows.  

**Build Method (Windows baseline):**
```powershell
rmdir /S /Q build
cmake --preset=mingw-debug
cmake --build --preset=build-mingw-debug
ctest --preset=test-mingw-debug
```

**Build Method (Windows optional, MSVC-only):**
```powershell
rmdir /S /Q build
cmake --preset=msvc-debug
cmake --build --preset=msvc-debug
ctest --preset=msvc-debug
```

**Build Method (Linux baseline):**
```bash
rm -rf build
cmake --preset=gcc-debug    # or clang-debug
cmake --build --preset=build-gcc-debug
ctest --preset=test-gcc-debug --output-on-failure
```

---

## Next Steps
- Future phases (phase1, phase2, …) will add new modules under `src/` and `tests/`.  
- CI workflows already cover multi-platform toolchain matrix.  
- GitHub Pages (UML Diagrams) live at:  
  👉 https://olavenko.github.io/Tasqly_V1/  


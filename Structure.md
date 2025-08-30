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
 ├── docs/                    # Documentation (UML + ADRs)
 │    ├── adr/                # Architecture Decision Records
 │    │    ├── 0001-plantuml-adoption.md
 │    │    ├── 0002-build-toolchain.md
 │    │    ├── 0003-google-test.md
 │    │    ├── 0004-ci-workflows.md
 │    │    ├── 0005-git-hygiene.md
 │    │    ├── 0006-debugger-policy.md
 │    │    └── README.md      # ADR index
 │    └── uml/                # UML Diagrams & Config
 │         ├── config/        # Shared style + includes
 │         │    └── style.puml
 │         ├── phase0/        # Phase 0 diagrams
 │         │    └── s0/
 │         │         ├── component.puml
 │         │         ├── sequence.puml
 │         │         ├── usecase.puml
 │         │         └── exports/      # Generated .svg diagrams
 │         ├── tools/        # Local tools
 │         │    └── plantuml.jar
 │         ├── uml-export.sh
 │         └── README.md      # UML Authoring Guidelines
 ├── cmake/                   # Toolchain & build scripts
 │    └── toolchains/
 │         └── mingw.cmake
 ├── .clang-format            # Coding style config
 ├── .clang-tidy              # Static analysis config
 ├── .gitignore               # Git ignore rules
 ├── .gitattributes           # Git attributes (line endings, etc.)
 ├── MyAllmanClassic.xml      # Qt Creator formatting preset
 ├── CMakeLists.txt           # Root CMake config
 ├── CMakePresets.json        # Standardized CMake presets
 └── Structure.md             # (This file)
```

---

## Notes
- **src/** → Application entry point (`main.cpp`, `Main.qml`), to be expanded in future phases.  
- **tests/** → Contains unit tests, starting with smoke test for `Result<T,Error>`.  
- **docs/adr/** → Architecture Decision Records (ADRs) for traceability.  
- **docs/uml/** → UML diagrams, with corporate style (`style.puml`) and CI export (`uml-export.sh`).  
- **cmake/toolchains/** → Toolchain definitions, currently `mingw.cmake` forcing MSYS2 MinGW GCC 15.2.  
- **.clang-format / .clang-tidy / .gitignore / .gitattributes** → Git hygiene & coding standards.  
- **CMakePresets.json** → Defines official build configurations (MinGW baseline, GCC/Clang Linux, MSVC optional).  

---

## Build Toolchain Policy (Summary)

- **Windows (Baseline):** MSYS2 MinGW-w64 GCC 15.2 + Ninja + Qt 6.8.2 MinGW.  
- **Linux:** GCC / Clang (latest stable).  
- **Optional:** MSVC (CI only, not baseline).  
- **Qt ABI constraint:** Qt-MinGW ↔ MinGW only, Qt-MSVC ↔ MSVC only (not interchangeable).  
- **Sanitizers:** Enabled on Linux, disabled on Windows MinGW.  

**Build Method (Windows baseline):**
```powershell
rmdir /S /Q build
cmake --preset=mingw-debug
cmake --build --preset=build-debug
ctest --preset=test-debug
```

---

## Next Steps
- Future phases (phase1, phase2, …) will add new subfolders under `src/` and `tests/`.  
- CI workflows (`.github/workflows/ci.yml`, `uml.yml`, `uml-pages.yml`) will be integrated.  
- All generated files (`build/`, `exports/`) remain excluded from version control.  

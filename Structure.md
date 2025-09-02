# 📂 Project Structure — Tasqly\_V1 (Updated for Phase 0 / Slice 1)

This document updates the folder structure to match the agreed **Clean Architecture** tree for **Slice 1 — Logging & Error Backbone**. 
It preserves existing sections and build policy while extending the `src/`, `docs/uml/`, and `tests/` layout.

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
 ├── build/                               # Local build output (ignored in VCS)
 ├── src/                                  # Application source code (Clean Architecture)
 │    ├── domain/
 │    │    └── core/
 │    │         ├── Error.h               # Error struct (code, message, context)
 │    │         └── Result.h              # Result<T,Error> + helpers (header-only)
 │    ├── app/
 │    │    ├── logging/
 │    │    │    ├── ILogManager.h         # Logging interface
 │    │    │    ├── ErrorReporter.h       # ErrorReporter Use Case (header)
 │    │    │    └── ErrorReporter.cpp     # ErrorReporter Use Case (impl)
 │    │    └── errors/
 │    │         ├── AppErrors.h           # Registry of common app errors (QML singleton)
 │    │         └── AppErrors.cpp         # App error registry implementation
 │    ├── infra/
 │    │    └── logging/
 │    │         ├── LogManager.h          # Console + rotating file + masking + levels
 │    │         └── LogManager.cpp        # Logging backend implementation
 │    └── ui/
 │         └── qml/
 │              └── components/
 │                   ├── Toast.qml        # Queue + severity + auto-dismiss + rate-limit
 │                   └── SmartFallback.qml# Placeholder visuals for generic failure
 ├── tests/                                # Unit & integration tests
 │    ├── test_result.cpp                  # Smoke test for Result<T,Error>
 │    ├── unit/
 │    │    └── logging/
 │    │         ├── test_log_level_parsing.cpp   # Parses & validates log levels
 │    │         ├── test_rotation_policy.cpp     # Verifies rotating file policy thresholds
 │    │         └── test_masking.cpp             # Ensures sensitive fields are masked
 │    └── integration/
 │         └── logging/
 │              └── test_qml_error_to_toast_and_log.cpp  # QML error → toast + log flow
 ├── docs/
 │    ├── adr/
 │    │    ├── 0001-plantuml-adoption.md          # ADR: PlantUML usage & conventions
 │    │    ├── 0002-build-toolchain.md            # ADR: Build toolchain decisions
 │    │    ├── 0003-google-test.md                # ADR: Testing framework choice
 │    │    ├── 0004-ci-workflows.md               # ADR: CI pipeline & artifacts
 │    │    ├── 0005-git-hygiene.md                # ADR: Branching & commit policy
 │    │    ├── 0006-debugger-policy.md            # ADR: Debugging standards
 │    │    ├── 0007-Build-Toolchain-CI-Policy.md  # ADR: ABI/presets policy
 │    │    └── README.md                          # ADR index
 │    ├── uml/                                     # UML Diagrams & Config
 │    │    ├── config/
 │    │    │    └── style.puml                     # Corporate style include (dark theme)
 │    │    ├── exports
 │    │    │    └── index.html                     # Diagrams index (navigable gallery)
 │    │    ├── phase0/
 │    │    │    ├── s0/
 │    │    │    │    ├── usecase.puml              # Use case — project initialization
 │    │    │    │    ├── sequence.puml             # Sequence — first CI/test flow
 │    │    │    │    ├── component.puml            # Component — initial structure
 │    │    │    │    └── exports/
 │    │    │    │         ├── usecase.svg          # Exported use case (s0)
 │    │    │    │         ├── sequence.svg         # Exported sequence (s0)
 │    │    │    │         └── component.svg        # Exported component (s0)
 │    │    │    └── s1/                            # Slice 1 diagrams
 │    │    │         ├── usecase.puml              # Use case — Report Error & Notify User
 │    │    │         ├── sequence.puml             # Sequence — QML → UC → LogManager → Toast → Fallback
 │    │    │         ├── component.puml            # Component — QML facades → app interfaces → infra impls
 │    │    │         └── exports/                   # Generated SVG diagrams
 │    │    │              ├── usecase.svg           # Report Error & Notify User (SVG)
 │    │    │              ├── sequence.svg          # Error flow to UI & logs (SVG)
 │    │    │              └── component.svg         # Layered components overview (SVG)
 │    │    ├── tools/                               # Local tools (plantuml.jar)
 │    │    ├── uml-export.sh                        # Export script (SVG + index.html)
 │    │    └── README.md                            # UML Authoring Guidelines
 │    └── building.md                               # Build instructions (local + CI + troubleshooting)
 ├── cmake/
 │    └── toolchains/
 │         └── mingw.cmake                          # Toolchain definitions (MinGW baseline)
 ├── .clang-format                                   # Coding style config
 ├── .clang-tidy                                     # Static analysis config
 ├── .gitignore                                      # Git ignore rules
 ├── .gitattributes                                  # Git attributes (line endings, etc.)
 ├── MyAllmanClassic.xml                             # Qt Creator formatting preset
 ├── CMakeLists.txt                                  # Root CMake config
 ├── CMakePresets.json                               # Standardized CMake presets (with debuggerPresets)
 ├── .github/
 │    └── workflows/
 │         ├── ci.yml                                # CI build & test (multi-platform matrix)
 │         ├── uml.yml                               # UML artifact export
 │         └── uml-pages.yml                         # UML GitHub Pages deployment
 ├── main.cpp                                        # App entry point (C++20)
 ├── Main.qml                                        # Root QML file
 ├── Structure.md                                    # Project structure (documentation)
 ├── CMakeLists.txt.user                             # Qt Creator local user settings (ignored)
 ├── cppcheck-suppressions.txt                       # Static analysis suppressions
 ├── Subsystems.md                                   # Subsystems notes/overview (original)
 └── Subsystems_EN_Enhanced_ASCII.md                 # Subsystems overview (enhanced + ASCII)
```

---

## Notes

* **New in this update**:

  * Clean Architecture subtrees under `src/` (`domain/`, `app/`, `infra/`, `ui/qml/components/`).
  * `docs/uml/phase0/s1/` with `.puml` sources + exported `.svg` outputs.
  * Extended `tests/` layout for unit + integration coverage of Slice 1.
* **No behavior change**; this is a structural update to guide implementation in the next steps.

---

## Build Toolchain Policy (Summary)

* **Windows (Baseline):** MSYS2 MinGW-w64 GCC 15.2 + Ninja + Qt-MinGW
* **Linux (Baseline):** GCC / Clang (latest stable) + Qt-Linux (ABI-compatible with MinGW).
* **Optional (Windows-only):** MSVC (Visual Studio 2019/2022) + Qt-MSVC, maintained in the `msvc-only` branch.
* **Qt ABI constraint:**

  * Qt-MinGW ↔ MinGW/GCC/Clang only (cross-platform baseline)
  * Qt-MSVC ↔ MSVC only (Windows-only branch)
  * ❌ Mixing Qt-MinGW with MSVC is not supported (ABI conflict).
* **Sanitizers:** Enabled on Linux only, disabled on Windows.

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

* Proceed to **DESIGN** for Slice 1 and add the three diagrams under `docs/uml/phase0/s1/exports/` as `.svg`.
* Use the above structure as the canonical layout for upcoming patches and tests.

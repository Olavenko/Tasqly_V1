# 📂 Project Structure — Tasqly_V1 (Auto-generated Snapshot)

Generated on **2025-09-04 02:00:14** using `scripts/gen_structure.py`.

## 📑 Table of Contents
1. [Root Layout](#root-layout)
2. [Notes](#notes)
3. [Build Toolchain Policy (Summary)](#build-toolchain-policy-summary)

---

## Root Layout
Tasqly_V1/
├── .github/
│   └── workflows/
│       ├── ci.yml [CI Config]
│       ├── uml-pages.yml [CI Config]
│       └── uml.yml [CI Config]
├── cmake/
│   └── toolchains/
│       └── mingw.cmake [Build Config]
├── docs/
│   ├── adr/
│   │   ├── phase0/
│   │   │   ├── Slice0/
│   │   │   │   ├── 0000-analysis.md [Markdown Doc]
│   │   │   │   ├── 0001-plantuml-adoption.md [Markdown Doc]
│   │   │   │   ├── 0002-build-toolchain.md [Markdown Doc]
│   │   │   │   ├── 0003-google-test.md [Markdown Doc]
│   │   │   │   ├── 0004-ci-workflows.md [Markdown Doc]
│   │   │   │   ├── 0005-git-hygiene.md [Markdown Doc]
│   │   │   │   ├── 0006-debugger-policy.md [Markdown Doc]
│   │   │   │   ├── 0007-Build-Toolchain-CI-Policy.md [Markdown Doc]
│   │   │   │   └── 0008-docker-dev-container.md [Markdown Doc]
│   │   │   └── Slice1/
│   │   │       ├── 0001-Result-Everywhere-Error-Propagation-Policy.md [Markdown Doc]
│   │   │       ├── 0002-Feature-Flags-Manager.md [Markdown Doc]
│   │   │       └── 0003-FeatureFlagsManager.md [Markdown Doc]
│   │   └── index-adr.md [Markdown Doc]
│   ├── uml/
│   │   ├── config/
│   │   │   └── style.puml [UML Diagram]
│   │   ├── exports/
│   │   ├── phase0/
│   │   │   ├── s0/
│   │   │   │   ├── exports/
│   │   │   │   ├── component.puml [UML Diagram]
│   │   │   │   ├── sequence.puml [UML Diagram]
│   │   │   │   └── usecase.puml [UML Diagram]
│   │   │   └── s1/
│   │   │       ├── exports/
│   │   │       ├── component.puml [UML Diagram]
│   │   │       ├── component_with_flags.puml [UML Diagram]
│   │   │       ├── sequence.puml [UML Diagram]
│   │   │       ├── sequence_with_flags.puml [UML Diagram]
│   │   │       ├── usecase.puml [UML Diagram]
│   │   │       └── usecase_with_flags.puml [UML Diagram]
│   │   ├── tools/
│   │   │   └── plantuml.jar
│   │   ├── Diagrams-README.md [Markdown Doc]
│   │   └── uml-export.sh
│   └── building.md [Markdown Doc]
├── scripts/
│   ├── gen_adr_index.py
│   └── gen_structure.py
├── src/
│   ├── app/
│   │   ├── errors/
│   │   │   ├── AppErrors.cpp [C++ Source]
│   │   │   └── AppErrors.h [C++ Header]
│   │   ├── logging/
│   │   │   ├── ErrorReporter.cpp [C++ Source]
│   │   │   ├── ErrorReporter.h [C++ Header]
│   │   │   └── ILogManager.h [C++ Header]
│   │   └── settings/
│   │       ├── FeatureFlagsManager.cpp [C++ Source]
│   │       └── FeatureFlagsManager.h [C++ Header]
│   ├── domain/
│   │   └── core/
│   │       ├── Error.h [C++ Header]
│   │       └── Result.h [C++ Header]
│   ├── infra/
│   │   └── logging/
│   │       ├── LogManager.cpp [C++ Source]
│   │       └── LogManager.h [C++ Header]
│   └── ui/
│       └── qml/
│           └── components/
│               ├── SmartFallback.qml [QML UI]
│               └── Toast.qml [QML UI]
├── tests/
│   ├── integration/
│   │   └── logging/
│   │       ├── test_error_reporter_feature_flag.cpp [C++ Source]
│   │       ├── test_feature_flag_logging.cpp [C++ Source]
│   │       └── test_qml_error_to_toast_and_log.cpp [C++ Source]
│   ├── unit/
│   │   └── logging/
│   │       ├── test_concurrency.cpp [C++ Source]
│   │       ├── test_log_level_parsing.cpp [C++ Source]
│   │       ├── test_masking.cpp [C++ Source]
│   │       └── test_rotation_policy.cpp [C++ Source]
│   └── test_result.cpp [C++ Source]
├── .clang-format
├── .clang-tidy
├── .gitattributes
├── .gitignore
├── CMakeLists.txt [Build Config]
├── CMakePresets.json
├── cppcheck-suppressions.txt
├── main.cpp [C++ Source]
├── Main.qml [Root QML UI]
├── MyAllmanClassic.xml
├── Project_Structure.md [Markdown Doc]
├── Subsystems.md [Markdown Doc]
└── Subsystems_EN_Enhanced_ASCII.md [Markdown Doc]

📊 Summary: 36 directories, 67 files
- C++ Files: 20
- QML Files: 3
- Docs: 28
- Build/Config: 5
- Other: 11

---

## Notes
⚡ This snapshot is auto-generated.
It only reflects the current filesystem and may differ from design docs (e.g., ADRs).

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

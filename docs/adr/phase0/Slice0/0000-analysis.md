# Phase 0 — Slice 0: Project Initialization & Testing Foundation

## Analysis Document

### Goal

Prepare the full project skeleton with continuous integration (CI) and testing infrastructure. This ensures the project is production-ready from the first commit, with clear build policies, automated verification, and testing capabilities.

### Scope

The scope of this slice covers the following tasks:

* **CMake Setup**: Define root build configuration with policies, toolchains, and project targets.
* **Google Test Integration**: Add official testing framework and a basic smoke test to validate build + test cycle.
* **Clang Tools**: Configure `clang-tidy`, `clang-format`, and `cppcheck` to enforce static analysis and style checks.
* **PlantUML Integration**: Define diagrams configuration under `docs/uml` for use case, sequence, and component views.
* **CI Pipeline**: Configure GitHub Actions workflow for multi-platform builds (Ubuntu GCC/Clang, Windows MinGW baseline, optional MSVC) and automated test execution.

### Dependencies

* **External Tools**:

  * CMake ≥ 3.20
  * Qt 6 (Qt Quick modules, though CI should allow fallback without Qt)
  * Google Test (fetched via CMake `FetchContent`)
  * Clang tooling (`clang-format`, `clang-tidy`), `cppcheck`
  * PlantUML (for diagrams, to be integrated in CI)
* **CI Environment**:

  * GitHub Actions runners with Ubuntu & Windows
  * Docker (planned for Phase 1, not in this slice)
* **Project Context**:

  * No prior dependencies since this is the initialization slice
  * Repository hygiene enforced via `.gitignore`, `.gitattributes`, and coding standards configuration

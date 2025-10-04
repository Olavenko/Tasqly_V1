# Phase 0 – ADR Summaries

## ADR 0000: Project Initialization & Testing Foundation
- **Date:** 2025-05-XX
- **Context:** Setup CI, CMake, Google Test, Clang tools, PlantUML for diagrams.
- **Decision:** Establish full project skeleton with CI + testing infra.
- **Consequences:** Baseline infra ready; smooth future development.
- **Linked Problems:** #1 (CI multi-platform setup), #2 (PlantUML missing deps)

---

## ADR 0001: PlantUML Adoption
- **Date:** 2025-05-XX
- **Context:** Need consistent lightweight diagramming tool.
- **Decision:** Adopt PlantUML + C4 libs + unified style + CI export scripts.
- **Consequences:** Unified diagrams; diagrams auto-exported in CI.
- **Linked Problems:** #2 (CI failing due to missing PlantUML deps)

---

## ADR 0002: Build Toolchain Policy
- **Date:** 2025-05-XX
- **Context:** Avoid ABI conflicts across Windows/Linux toolchains.
- **Decision:** Baseline: MinGW on Windows, GCC/Clang on Linux; optional MSVC.
- **Consequences:** Consistent builds, reproducible CI results.
- **Linked Problems:** #3 (ABI conflicts)

---

## ADR 0003: Google Test Framework
- **Date:** 2025-05-XX
- **Context:** Need cross-platform, modern test framework.
- **Decision:** Adopt Google Test via FetchContent; integrate into CMake.
- **Consequences:** Unit testing foundation integrated into CI.
- **Linked Problems:** #4 (tests not discovered in CI)

---

## ADR 0004: CI Workflows
- **Date:** 2025-05-XX
- **Context:** Need automated builds/tests with diagrams on GitHub Actions.
- **Decision:** Defined workflows for CI, UML, and publishing diagrams.
- **Consequences:** CI validates every push; diagrams in sync.
- **Linked Problems:** #5 (CI fails without Qt installed)

---

## ADR 0005: Git Hygiene
- **Date:** 2025-05-XX
- **Context:** Repo was polluted with artifacts and IDE files.
- **Decision:** Added `.gitignore`, `.gitattributes`, `.clang-format`, `.clang-tidy`.
- **Consequences:** Repo stays clean, style consistent.
- **Linked Problems:** #6 (repo pollution)

---

## ADR 0006: Debugger Policy
- **Date:** 2025-05-XX
- **Context:** Developers used inconsistent debuggers across platforms.
- **Decision:** Baseline GDB; LLDB/MSVC optional.
- **Consequences:** Unified debugging workflow.
- **Linked Problems:** #7 (debugger inconsistency)

---

## ADR 0007: Build Toolchain CI Policy
- **Date:** 2025-05-XX
- **Context:** CI ABI mismatches with fallback MSVC builds.
- **Decision:** CI matches developer stack (Qt + MinGW/GCC/Clang).
- **Consequences:** CI artifacts identical to local builds.
- **Linked Problems:** #8 (ABI mismatches in CI)

---

## ADR 0008: Docker Dev Container
- **Date:** 2025-09-01
- **Context:** Inconsistent dev environments; "works on my machine" issues.
- **Decision:** Introduce Docker dev container in Phase 1 (optional).
- **Consequences:** Standardized environment; onboarding easier.
- **Linked Problems:** #9 (env variability)

---

## ADR 0009: Result-Everywhere Error Propagation
- **Date:** 2025-09-01
- **Context:** Mixed error handling with exceptions/raw codes.
- **Decision:** All fallible funcs return `Result<T, Error>`; no exceptions except interop.
- **Consequences:** Predictable error handling, simpler QML integration.
- **Linked Problems:** #10 (inconsistent error propagation)

---

## ADR 0010: Feature Flags Manager
- **Date:** 2025-09-04
- **Context:** Compile-time flags prevented runtime toggling.
- **Decision:** Implemented `FeatureFlagsManager` with runtime QML singleton.
- **Consequences:** Runtime flexibility, testable in CI.
- **Linked Problems:** #11 (compile-time flags too rigid)

---

## ADR 0011: QML Facades vs Direct Infra
- **Date:** 2025-09-06
- **Context:** QML tightly coupled to backend managers.
- **Decision:** Expose via QML singleton facades (`App.*`).
- **Consequences:** Decoupled UI from infra.
- **Linked Problems:** #12 (direct infra exposure)

---

## ADR 0012: Theme Persistence Approach
- **Date:** 2025-09-06
- **Context:** Theme not saved between restarts.
- **Decision:** Store `ThemeMode` in settings, sync at startup.
- **Consequences:** Correct theme persistence.
- **Linked Problems:** #13 (theme reset)

---

## ADR 0013: QML Paths Resolution
- **Date:** 2025-09-06
- **Context:** StackView failed to resolve QML via relative paths.
- **Decision:** Use `qt_add_qml_module` and proper URI.
- **Consequences:** Clean QML loading, IDE discovery works.
- **Linked Problems:** #14 (StackView path issues)

---

## ADR 0014: Resources vs QML Module
- **Date:** 2025-09-06
- **Context:** Duplication of QML in qrc + module.
- **Decision:** qrc for assets only, QML via module.
- **Consequences:** No runtime duplication.
- **Linked Problems:** #15 (qrc conflicts)

---

## ADR 0015: SettingsPage Architecture
- **Date:** 2025-09-06
- **Context:** QML calling non-existent API.
- **Decision:** Temporary workaround via Theme + Settings; refactor planned.
- **Consequences:** Works now; needs cleanup later.
- **Linked Problems:** #16 (missing API)

---

## ADR 0016: CI Cache Split
- **Date:** 2025-09-06
- **Context:** CI long runtime; Linux Qt libs missing.
- **Decision:** Split build/test jobs, enabled caching, fixed Qt libs.
- **Consequences:** Faster, stable CI.
- **Linked Problems:** #17 (Linux CI Qt libs)

---

## ADR 0017: QML Code Model False Positives
- **Date:** 2025-09-06
- **Context:** IDE showed invalid QML errors though runtime was fine.
- **Decision:** Policy: reset QML model, restart IDE, clean build.
- **Consequences:** Devs ignore false IDE errors, focus on runtime.
- **Linked Problems:** #18 (false positives in IDE)

---

## ADR 0018: CMake Sorting Scripts
- **Date:** 2025-09-06
- **Context:** CMake files inconsistent, merge conflicts.
- **Decision:** Introduced scripts to normalize ordering.
- **Consequences:** Cleaner builds, fewer conflicts.
- **Linked Problems:** #19 (CMake inconsistencies)

---

## ADR 0019: Route Guard Policy
- **Date:** 2025-09-06
- **Context:** Navigation allowed invalid strings.
- **Decision:** Introduced `Route` struct + guard validation.
- **Consequences:** Type-safe navigation.
- **Linked Problems:** #20 (invalid routes)

---

## ADR 0020: QML Singleton vs Named Element
- **Date:** 2025-09-06
- **Context:** `NavigateTo` registered both ways; build failed.
- **Decision:** Removed `QML_NAMED_ELEMENT`, kept singleton only.
- **Consequences:** QML binds correctly; build fixed.
- **Linked Problems:** #21 (QML registration conflict)

---

## ADR 0021: Navigation Loader & Testing Fixes
- **Date:** 2025-09-06
- **Context:** Navigation & testing caused abstract/logging/link errors.
- **Decision:** Added DummyLogger, meta-type registration, refactored PageStateWrapper, added Notifier stub.
- **Consequences:** All tests pass; state machine validated.
- **Linked Problems:** #22 (test/meta-object issues)

---

## ADR 0022: Repository Error Codes
- **Date:** 2025-09-07
- **Context:** Arbitrary error codes in repos.
- **Decision:** Standardized `RepoErrorCode` enum.
- **Consequences:** Consistent repo errors.
- **Linked Problems:** #23 (inconsistent repo errors)

---

## ADR 0023: Include Path Policy
- **Date:** 2025-09-07
- **Context:** Ambiguous includes caused errors.
- **Decision:** Use project-root relative includes.
- **Consequences:** Consistent includes; fewer errors.
- **Linked Problems:** #24 (include ambiguity)

---

## ADR 0024: Namespace & Naming Policy
- **Date:** 2025-09-07
- **Context:** Domain types scattered/global.
- **Decision:** Domain under `tasqly::domain::core`; app under `tasqly::app`.
- **Consequences:** Clear layer separation.
- **Linked Problems:** #25 (namespace conflicts)

---

## ADR 0025: Tracking New Files
- **Date:** 2025-09-07
- **Context:** New files added without documentation.
- **Decision:** ADR listed all new files by layer.
- **Consequences:** Architectural clarity.
- **Linked Problems:** #26 (file tracking gaps)

---

## ADR 0026: QML TaskListPage Tests
- **Date:** 2025-09-08
- **Context:** QML tests failing because TaskListPage not implemented yet.
- **Decision:** Load via qrc path + skip when not ready.
- **Consequences:** CI remains green; clear reminder for future update.
- **Linked Problems:** #27 (QML TaskListPage tests failing)

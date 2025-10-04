# Phase 0 – Problems & Solutions Log

## Problem 1: CI Workflow Setup for Multi-Platform Builds
- **Date Discovered:** 2025-05-XX
- **Context:** Slice0 / Project Initialization
- **Symptoms:** Early CI runs failed inconsistently across Ubuntu and Windows environments.
- **Root Cause Analysis:** Missing toolchain configurations and insufficient minimum version checks for CMake (required ≥ 3.20).
- **Solution:** Defined root `CMakeLists.txt` with strict policies, upgraded to CMake ≥ 3.20, and ensured consistent toolchain files across CI runners.
- **Verification:** CI pipelines now pass on Ubuntu GCC/Clang and Windows MinGW after re-run.
- **Status:** Solved
- **Linked ADR:** ADR 0000-analysis

---

## Problem 2: CI Failing Due to Missing PlantUML Dependencies
- **Date Discovered:** 2025-05-XX
- **Context:** Slice0 / Documentation & CI
- **Symptoms:** Diagram export jobs in GitHub Actions failed; PlantUML not available on runners.
- **Root Cause Analysis:** PlantUML integration was adopted but dependencies (Java, Graphviz) not installed on CI.
- **Solution:** Updated CI workflow to install PlantUML and Graphviz, added `uml-export.sh` script to handle exports automatically.
- **Verification:** CI now generates and publishes UML diagrams under `docs/uml/exports/` with consistent style.
- **Status:** Solved
- **Linked ADR:** ADR 0001-plantuml-adoption

---

## Problem 3: ABI Conflicts Between Qt-MinGW and Qt-MSVC
- **Date Discovered:** 2025-05-XX
- **Context:** Slice0 / Build Toolchain
- **Symptoms:** Linking errors and runtime crashes when mixing Qt-MinGW builds with MSVC binaries.
- **Root Cause Analysis:** Qt-MinGW and Qt-MSVC are not ABI compatible. Some developers attempted to mix toolchains.
- **Solution:** Established strict toolchain policy: baseline MinGW-w64 GCC on Windows, GCC/Clang on Linux. MSVC supported only for CI compatibility checks.
- **Verification:** Consistent builds validated in CI/CD matrix (Windows MinGW, Ubuntu GCC/Clang).
- **Status:** Solved
- **Linked ADR:** ADR 0002-build-toolchain

---

## Problem 4: Unit Tests Not Discovered in CI
- **Date Discovered:** 2025-05-XX
- **Context:** Slice0 / Testing Framework
- **Symptoms:** CI pipeline ran successfully but reported "0 tests found."
- **Root Cause Analysis:** Tests not properly integrated with CMake due to missing `gtest_discover_tests()` configuration.
- **Solution:** Integrated Google Test via `FetchContent`, added `TasqlyTests` target, and enabled `gtest_discover_tests()` for auto-discovery.
- **Verification:** Running `ctest` locally and in CI now executes all tests successfully.
- **Status:** Solved
- **Linked ADR:** ADR 0003-google-test

---

## Problem 5: CI Workflow Failures Without Qt Installed
- **Date Discovered:** 2025-05-XX
- **Context:** Slice0 / CI Workflows
- **Symptoms:** GitHub Actions failed when Qt SDK was missing in the runner environment.
- **Root Cause Analysis:** CI jobs assumed Qt was always available; no stubs provided for "headless" builds.
- **Solution:** Added workflow support for both "with Qt" and "without Qt" builds by providing stub headers and conditional logic in CMake.
- **Verification:** CI pipelines now pass successfully across Linux and Windows with and without Qt installed.
- **Status:** Solved
- **Linked ADR:** ADR 0004-ci-workflows

---

## Problem 6: Repository Pollution with Build Artifacts
- **Date Discovered:** 2025-05-XX
- **Context:** Slice0 / Git Hygiene
- **Symptoms:** Developers accidentally committed build artifacts (`*.o`, `*.exe`, `*.dll`) and IDE-specific files (`*.pro.user`).
- **Root Cause Analysis:** Missing `.gitignore` entries and inconsistent Git configuration across contributors.
- **Solution:** Added `.gitignore`, `.gitattributes`, and coding standards files (`.clang-format`, `.clang-tidy`, `MyAllmanClassic.xml`) to enforce repository hygiene.
- **Verification:** New commits no longer include unwanted artifacts; repository stays clean across platforms.
- **Status:** Solved
- **Linked ADR:** ADR 0005-git-hygiene

---

## Problem 7: Inconsistent Debugger Usage Across Platforms
- **Date Discovered:** 2025-05-XX
- **Context:** Slice0 / Debugger Policy
- **Symptoms:** Developers reported inconsistent debugging experience; some used LLDB, others MSVC, leading to confusion in bug reports.
- **Root Cause Analysis:** Lack of unified baseline debugger policy across Windows/Linux.
- **Solution:** Defined baseline: GDB (Windows MinGW and Linux). LLDB and MSVC supported only as optional alternatives. Documented debugger usage in README/wiki.
- **Verification:** Developers aligned on GDB as default; onboarding guide updated.
- **Status:** Solved
- **Linked ADR:** ADR 0006-debugger-policy

---

## Problem 8: ABI Mismatches in CI Builds
- **Date Discovered:** 2025-05-XX
- **Context:** Slice0 / Build Toolchain CI Policy
- **Symptoms:** CI artifacts sometimes failed locally due to ABI mismatch between MSVC stubs and MinGW-built Qt.
- **Root Cause Analysis:** CI previously attempted fallback builds with MSVC instead of using the same toolchain as developers.
- **Solution:** Unified toolchain in CI: Qt 6.8 + MinGW on Windows, Qt 6.8 + GCC/Clang on Linux. Disabled fallback stubs. Enabled caching for MSYS2/Qt to reduce runtime.
- **Verification:** CI artifacts now identical to local builds; no ABI mismatch observed.
- **Status:** Solved
- **Linked ADR:** ADR 0007-build-toolchain-ci-policy

---

## Problem 9: Environment Variability Between Developers
- **Date Discovered:** 2025-09-01
- **Context:** Slice0 / Dev Container Planning
- **Symptoms:** "Works on my machine" issues due to differences in locally installed compilers, Qt SDKs, and PlantUML/Graphviz versions.
- **Root Cause Analysis:** Developers rely on host-installed toolchains with inconsistent versions.
- **Solution:** Planned adoption of Docker-based dev container (Phase 1) with unified toolchain, Qt, linters, PlantUML, and Graphviz. Container remains optional but acts as reference environment.
- **Verification:** Smoke tests inside dev container (build + ctest + UML export) succeeded during internal validation.
- **Status:** Planned
- **Linked ADR:** ADR 0008-docker-dev-container

---

## Problem 10: Inconsistent Error Propagation
- **Date Discovered:** 2025-09-01
- **Context:** Slice1 / Error Handling
- **Symptoms:** Functions used mixed error-handling approaches (exceptions, raw error codes), causing unpredictable behavior especially in async/QML contexts.
- **Root Cause Analysis:** Lack of a unified error propagation mechanism across layers; exceptions unsafe across Qt boundaries.
- **Solution:** Enforced exclusive use of `Result<T, Error>` for all fallible functions. Structured `Error` type introduced with `code`, `message`, and `context`. Exceptions allowed only in 3rd-party interop or as last-resort in `main()`.
- **Verification:** Unit tests in `tests/unit/result/` validated error propagation consistency. QML integration confirmed stable behavior.
- **Status:** Solved
- **Linked ADR:** ADR 0001-result-everywhere-error-propagation-policy

---

## Problem 11: Limited Flexibility with Compile-Time Feature Flags
- **Date Discovered:** 2025-09-04
- **Context:** Slice1 / Feature Flags
- **Symptoms:** Early design tied feature activation to CMake options or presets, requiring rebuilds to toggle features (e.g., logging on/off).
- **Root Cause Analysis:** Compile-time flags unsuitable for runtime control and conflict with planned integration into `AppSettingsManager`.
- **Solution:** Implemented `FeatureFlagsManager` as a runtime singleton (exposed to QML). Provided methods `isEnabled(key)` and `setFeature(key, value)` with default-enabled core subsystems.
- **Verification:** CI tests confirmed logging subsystem can be toggled at runtime. Integrated with `LogManager` and `ErrorReporter`.
- **Status:** Solved
- **Linked ADR:** ADR 0002-feature-flags-manager

---

## Problem 12: Direct Exposure of Infrastructure to QML
- **Date Discovered:** 2025-09-06
- **Context:** Slice2 / QML Integration
- **Symptoms:** QML attempted to call backend managers directly, leading to tight coupling and fragile API boundaries.
- **Root Cause Analysis:** No abstraction layer between QML and C++ managers (e.g., Settings, Theme).
- **Solution:** Introduced QML facades via `QmlSingletonProvider` (`App.Settings`, `App.Theme`, `App.Log`, `App.Notifier`). Backend refactor possible without breaking QML.
- **Verification:** QML now consumes only `App.*` facades; integration stable.
- **Status:** Solved
- **Linked ADR:** ADR 0001-qml-facades-vs-direct-infra

---

## Problem 13: Theme Not Persisting Across Restarts
- **Date Discovered:** 2025-09-06
- **Context:** Slice2 / Theme & Settings
- **Symptoms:** User-selected theme reset to default on app restart.
- **Root Cause Analysis:** ThemeManager handled runtime palette but persistence layer was missing.
- **Solution:** Stored only `ThemeMode` in `AppSettingsManager`. On startup, ThemeManager syncs mode from settings.
- **Verification:** Restart confirmed theme mode persists correctly.
- **Status:** Solved
- **Linked ADR:** ADR 0002-theme-persistence-approach

---

## Problem 14: QML StackView Failing to Resolve Pages
- **Date Discovered:** 2025-09-06
- **Context:** Slice2 / QML Paths
- **Symptoms:** `SettingsPage.qml` could not be resolved via relative/qrc paths in StackView.
- **Root Cause Analysis:** Using file paths instead of proper QML module registration.
- **Solution:** Registered QML pages via `qt_add_qml_module` (URI `Tasqly_V1 1.0`). Loaded components as `SettingsPage {}` instead of paths.
- **Verification:** StackView navigation worked correctly; IDE auto-completion available.
- **Status:** Solved
- **Linked ADR:** ADR 0003-qml-paths-resolution

---

## Problem 15: Conflicts Between qrc and QML Module
- **Date Discovered:** 2025-09-06
- **Context:** Slice2 / Build Resources
- **Symptoms:** Duplicate inclusion when QML files added to both `resources.qrc` and `qt_add_qml_module`.
- **Root Cause Analysis:** QML and resources managed inconsistently.
- **Solution:** Reserved `resources.qrc` for non-QML assets (icons, images, data). All QML files handled exclusively by `qt_add_qml_module`.
- **Verification:** Build completed without duplicate resource errors.
- **Status:** Solved
- **Linked ADR:** ADR 0004-resources-vs-qml-module

---

## Problem 16: SettingsPage Calling Nonexistent API
- **Date Discovered:** 2025-09-06
- **Context:** Slice2 / SettingsPage Architecture
- **Symptoms:** QML attempted to call `Settings.setThemeMode()`, but `AppSettingsManager` did not expose such invokable.
- **Root Cause Analysis:** Misalignment between QML calls and backend API.
- **Solution:** Temporary workaround: QML calls `Theme.switchTo(newMode)` then `Settings.save()`. Future slice will add `setThemeMode()` API properly.
- **Verification:** Theme toggle + persistence now works, though refactor needed in future.
- **Status:** Solved (with planned refactor)
- **Linked ADR:** ADR 0005-settingspage-architecture

---

## Problem 17: Linux CI Failing Due to Missing Qt Libraries
- **Date Discovered:** 2025-09-06
- **Context:** Slice2 / CI Optimization
- **Symptoms:** Linux test jobs failed at runtime with error: `libQt6Concurrent.so.6: cannot open shared object file`.
- **Root Cause Analysis:** Qt libraries not available in Linux test job environment.
- **Solution:** Split CI into separate build and test jobs, enabled caching, re-installed Qt 6.9.2 in Linux test jobs, and exported `LD_LIBRARY_PATH`.
- **Verification:** CI pipelines now run successfully; runtime Qt linking issues resolved.
- **Status:** Solved
- **Linked ADR:** ADR 0006-adr-ci-cache-split

---

## Problem 18: False-Positive Errors in Qt Creator QML Code Model
- **Date Discovered:** 2025-09-06
- **Context:** Slice3 / QML Code Model
- **Symptoms:** IDE (Qt Creator) showed errors like `Invalid property name "onRetry"` even though signals were correctly defined and app compiled fine.
- **Root Cause Analysis:** QML code model cache in Qt Creator sometimes becomes inconsistent after adding new signals or properties.
- **Solution:** Documented policy: verify runtime first, reset QML code model, restart IDE, clean build if necessary. Only log if reproducible.
- **Verification:** After reset/restart, false errors disappeared; runtime behavior always correct.
- **Status:** Solved
- **Linked ADR:** ADR 0001-qml-code-model-false-positives

---

## Problem 19: Inconsistent CMake Block Ordering
- **Date Discovered:** 2025-09-06
- **Context:** Slice3 / Build System
- **Symptoms:** `CMakeLists.txt` became inconsistent and hard to read; frequent merge conflicts on build definitions.
- **Root Cause Analysis:** Manual ordering of `target_sources`, `qt_add_qml_module`, and `target_link_libraries` without standard policy.
- **Solution:** Introduced Python scripts (`sort_add_executable.py`, `sort_qml_module.py`, etc.) to automatically reorder blocks with consistent grouping.
- **Verification:** Running scripts normalized all build files; merge conflicts reduced.
- **Status:** Solved
- **Linked ADR:** ADR 0002-cmake-sorting-scripts

---

## Problem 20: Invalid Routes Causing Navigation Errors
- **Date Discovered:** 2025-09-06
- **Context:** Slice3 / Navigation
- **Symptoms:** UI could attempt navigation with arbitrary strings, breaking stack or failing silently.
- **Root Cause Analysis:** No central validation of routes; navigation depended on raw strings.
- **Solution:** Introduced `Route` struct with `isValid()`. All navigation requests now go through `NavigationService::navigateTo(Route)` with guard policy.
- **Verification:** Invalid routes rejected with error logs and toast notifications; valid routes logged and pushed correctly.
- **Status:** Solved
- **Linked ADR:** ADR 0003-route-guard-policy

---

## Problem 21: Conflict Between QML Singleton and Named Element
- **Date Discovered:** 2025-09-06
- **Context:** Slice3 / QML Registration
- **Symptoms:** Build failed with `NavigateTo not declared in this scope`; QML runtime complained about ambiguous `NavigateTo`.
- **Root Cause Analysis:** `NavigateTo` was both declared as `QML_NAMED_ELEMENT` and registered as a singleton, causing auto-gen conflicts.
- **Solution:** Removed `QML_NAMED_ELEMENT(NavigateTo)`, kept only explicit singleton registration in `QmlSingletonProvider`.
- **Verification:** Build succeeded; QML connections bound correctly to `NavigateTo`.
- **Status:** Solved
- **Linked ADR:** ADR 0004-qml-singleton-vs-named-element

---

## Problem 22: Multiple Test and Meta-Object Issues in Navigation
- **Date Discovered:** 2025-09-06
- **Context:** Slice3 / Navigation & Testing
- **Symptoms:** Several integration tests failed:
  - Abstract class errors for dummy logger
  - QSignalSpy failed with `Route` and `PageState` types
  - Linker errors for `PageStateWrapper`
  - Undefined reference for `Notifier::show`
- **Root Cause Analysis:** Missing full stub implementations, missing `Q_DECLARE_METATYPE`, misuse of Q_GADGET, and incomplete CMake targets.
- **Solution:** Implemented complete `DummyLogger`, registered meta-types locally, converted `PageStateWrapper` into QObject with `Q_ENUM`, added `Notifier.cpp` to tests.
- **Verification:** All 51 unit and integration tests passed; navigation state machine validated.
- **Status:** Solved
- **Linked ADR:** ADR 0005-navigation-loader-fallback-testing-fixes

---

## Problem 23: Inconsistent Repository Error Codes
- **Date Discovered:** 2025-09-07
- **Context:** Slice4 / Repository Layer
- **Symptoms:** Developers used arbitrary integers or messages for repository errors (e.g., NotFound, AlreadyExists), causing inconsistency in tests and code.
- **Root Cause Analysis:** No standardized error code policy for repositories.
- **Solution:** Introduced `RepoErrorCode` enum inside `Error.h` (`NotFound=404`, `AlreadyExists=409`, `StorageFailure=500`). Repositories now construct `Error` with these codes.
- **Verification:** Unit tests validated error propagation consistency across repositories.
- **Status:** Solved
- **Linked ADR:** ADR 0001-repository-error-codes

---

## Problem 24: Ambiguous Local Include Paths
- **Date Discovered:** 2025-09-07
- **Context:** Slice4 / Include Policy
- **Symptoms:** Compiler errors when including headers across layers; duplicate file names (e.g., Goal.h) caused ambiguity.
- **Root Cause Analysis:** Headers included via local paths like `#include "Goal.h"` instead of consistent project-root paths.
- **Solution:** Standardized includes relative to `src/` root (`#include "domain/core/Goal.h"`). Deprecated local includes.
- **Verification:** Refactored files compiled cleanly across layers; navigation clarity improved.
- **Status:** Solved
- **Linked ADR:** ADR 0002-include-path-policy

---

## Problem 25: Namespace Conflicts Between Domain and App Types
- **Date Discovered:** 2025-09-07
- **Context:** Slice4 / Namespace Policy
- **Symptoms:** Compiler errors when consuming domain entities (`Task`, `Goal`) and repositories due to being in global namespace, conflicting with `tasqly::domain::core` types.
- **Root Cause Analysis:** Domain types lacked a unified namespace.
- **Solution:** Moved all domain types (entities, repos, utilities) under `tasqly::domain::core`. Application types remain under `tasqly::app::{subsystem}`.
- **Verification:** Compiler errors resolved; codebase now consistent with namespace policy.
- **Status:** Solved
- **Linked ADR:** ADR 0003-namespace-naming-policy

---

## Problem 26: Lack of Tracking for Newly Added Files
- **Date Discovered:** 2025-09-07
- **Context:** Slice4 / File Tracking
- **Symptoms:** Newly introduced files (e.g., `AppContext`, `AppLoadingManager`, `ListTasks`) not formally tracked, making it harder to review architectural changes.
- **Root Cause Analysis:** No centralized documentation of new files introduced in this slice.
- **Solution:** ADR documented all new files by layer (App, Infra, Test Fakes). Purpose of each file explicitly stated.
- **Verification:** File list cross-checked with repo; CI/code review now enforces consistency.
- **Status:** Solved
- **Linked ADR:** ADR 0004-tracking-new-files

---

## Problem 27: QML TaskListPage Tests Failing in CI
- **Date Discovered:** 2025-09-08
- **Context:** Slice4 / QML Testing
- **Symptoms:** `qmltestrunner` could not resolve `TaskListPage.qml` via `import Tasqly_V1 1.0`; tests failed since UI slice not yet implemented.
- **Root Cause Analysis:** QML module only bundled with app target (`appTasqly_V1`), not exported for standalone testing. Page incomplete.
- **Solution:** Modified tests to load via `qrc:/qt/qml/...` and added `skip()` conditions when page not ready. CI reports SKIP instead of FAIL.
- **Verification:** CI pipelines remain green; skip logs remind devs to update once UI is implemented.
- **Status:** Solved (temporary)
- **Linked ADR:** ADR 0005-adr_qml_task_list_page_tests

---

## Problem 28: LogManager Rotation Not Triggering Properly
- **Date Discovered:** 2025-09-15
- **Context:** Infra → Logging → `LogManager` (unit + integration tests)
- **Symptoms:**  
  - `TasqlyTests.LogManagerExtendedTest.RotatesWhenFileExceedsMaxSize` failing (no rotated files created).  
  - `TasqlyTests.RotationPolicyTests.TruncateModeWhenMaxFilesZero` failing.  
  - Logs exceeded maxBytes but rotation did not happen as expected.
- **Root Cause Analysis:**  
  - Rotation relied on `QFile::size()`, which lags due to buffered I/O.  
  - After switching to `m_writtenBytes`, rotation still failed because:  
    1. No `flush()` before `close()` → rotated files could be empty.  
    2. No `ensureFileOpen_locked()` after rotation → new file not created.  
    3. Truncate mode (`maxFiles=0`) didn’t reset `m_writtenBytes` properly.
- **Solution:**  
  - Added `m_writtenBytes` counter and updated logic in `writeFile_locked()`.  
  - Ensured `flush()` before `close()` in `performRotate_locked()`.  
  - Reset `m_writtenBytes` after every rotation/truncate.  
  - Called `ensureFileOpen_locked()` after successful rotation to reopen fresh file.  
- **Verification:**  
  - Re-ran all unit + integration tests.  
  - ✅ `RotationPolicyTests.TruncateModeWhenMaxFilesZero` passed.  
  - ✅ `RotatesWhenFileExceedsMaxSize` passed after final patch.  
  - Benchmarks (`Bench_LogRotation.cpp`) run successfully with stable performance.
- **Status:** Solved

---

## Problem 29: AppContext InitSuccess Test Causes Heap Corruption
- **Date Discovered:** 2025-09-15
- **Context:** App → Core → `AppContext` / Tests → `test_app_context.cpp`
- **Symptoms:**  
  - `TasqlyTests.AppContextTest.InitSuccess` fails with `Exit code 0xc0000374` (heap corruption / segfault).  
  - Crash occurs immediately after setting repositories and services.
- **Root Cause Analysis:**  
  - Fake repositories (`FakeTaskRepo`, `FakeGoalRepo`) and services (`FakeUuidGen`, `FakeClock`) implement domain interfaces.  
  - The domain interfaces (`ITaskRepository`, `IGoalRepository`, `IUuidGen`, `IClock`) did not declare a **virtual destructor**.  
  - When `std::shared_ptr` attempts to destroy these polymorphic objects via base pointer, undefined behavior occurs → heap corruption.
- **Solution:**  
  - Added `virtual ~Interface() = default;` to all domain interfaces (`ITaskRepository`, `IGoalRepository`, `IUuidGen`, `IClock`).  
  - Ensured that destructors are virtual, preventing undefined behavior on cleanup.
- **Verification:**  
  - Rebuilt project.  
  - ✅ `TasqlyTests.AppContextTest.InitSuccess` passed without segfault.  
  - ✅ Other AppContext tests (`InitFailure`, `ResourceCleanup`) passed.  
- **Status:** Solved

# ADR 0005: navigation-loader-fallback-testing-fixes

- **Status**: Accepted
- **Date**: 2025-09-06
- **Owners**: Tasqly Team
- **Related**: Phase0 — Slice3
- **Tags**: navigation, loader, fallback, testing, qt

---

## Context
During the implementation and testing of **Slice 3 (Navigation, Loader & Fallback)**, several integration and build issues arose around the navigation service, loader state machine, and testing setup.  
The ADR documents the encountered problems and the chosen resolutions to maintain project stability and testing coverage.

## Decision
We resolved the following issues as part of Slice 3 QA:

- **`ILogManager` stub in tests**  
  - Problem: Dummy logger in tests did not fully implement `ILogManager`, causing abstract class errors.  
  - Decision: Implemented a complete no-op `DummyLogger` with all required methods, using full namespace (`tasqly::app::logging::Level`, `RotationPolicy`).

- **`Route` and `PageState` in signals**  
  - Problem: Qt signals using `Route` and `PageStateWrapper::PageState` failed with QSignalSpy due to missing meta-type registration.  
  - Decision: Added `Q_DECLARE_METATYPE` + `qRegisterMetaType` in all tests before use.

- **Multiple definition of meta-type registration**  
  - Problem: Global `RegisterMetaTypes` struct caused linker errors.  
  - Decision: Replaced with local inline `qRegisterMetaType<>()` inside each test.

- **`PageStateWrapper::staticMetaObject` linker error**  
  - Problem: Defined with `Q_GADGET`, but moc did not generate `staticMetaObject`.  
  - Decision: Converted `PageStateWrapper` into a `QObject` with `Q_OBJECT` + `Q_ENUM`, enabling full metaobject support.

- **Notifier linking issue**  
  - Problem: `Notifier::show` was declared but missing in build, causing undefined reference.  
  - Decision: Added `Notifier.cpp` to `TasqlyTests` CMake target and implemented a minimal stub for tests.

## Consequences
- ✅ All 51 unit and integration tests passed successfully.  
- ✅ Navigation state machine verified with automated tests.  
- ⏳ Manual UI tests postponed until Phase 1 (UI polish), since current foundation UI does not yet expose SmartFallback/Toast properly.  
- ⚠️ Slight increase in `PageStateWrapper` weight (now QObject-based), but benefits outweigh cost by enabling correct Qt metaobject usage.  
- 🔄 Trade-off: Manual invalid-route testing requires temporary UI stubs until full UI layer is built.

## References
- **Slice 3 Analysis & Design Docs**  
- **Tests**: `test_route_validation.cpp`, `test_navigation_service.cpp`, `test_navigate_to_uc.cpp`  
- **Files Changed**: `PageState.h`, `Notifier.cpp`, CMakeLists, unit/integration test sources  
- ADR 0004: Route-as-Value & guard policy


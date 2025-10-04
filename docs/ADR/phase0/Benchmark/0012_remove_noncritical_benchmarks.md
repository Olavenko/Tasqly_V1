# ADR 0012: Remove Non-Critical Benchmarks

- **Status**: Draft
- **Date**: 2025-10-03
- **Owners**: Tasqly Team
- **Related**: Phase0 — Benchmark Cleanup
- **Tags**: benchmarks, cleanup, performance, quality

---

## Context
During Phase0, we created a wide range of benchmark files under `benchmarks/unit/` to measure different subsystems. While this was useful for early validation, some benchmarks do not provide meaningful value because they target simple data types or trivial classes with negligible performance impact. Keeping unnecessary benchmarks:
- Increases CI runtime without real value.
- Adds noise to reports, making it harder to track critical performance regressions.
- Consumes maintenance effort without benefit.

According to `Subsystems_Unified.md` and `Project_Quality_Policy.md`, only **performance-critical subsystems with explicit KPIs** require continuous benchmarking.

---

## Decision
We will **remove non-critical benchmarks** that do not align with subsystem performance bars. These include:

### To Be Removed
- `benchmarks/unit/core/Bench_Error.cpp`
- `benchmarks/unit/core/Bench_PageState.cpp`
- `benchmarks/unit/core/Bench_Result.cpp`
- `benchmarks/unit/core/Bench_Route.cpp`
- `benchmarks/unit/core/Bench_ThemeMode.cpp`
- `benchmarks/unit/errors/Bench_AppErrors.cpp`
- `benchmarks/unit/notifier/Bench_Notifier.cpp`
- `benchmarks/unit/persistence/Bench_InMemoryRepo.cpp`
- `benchmarks/unit/loading/Bench_AppLoadingManager.cpp` *(Optional → removed to reduce noise; performance validated in startup tests instead)*

### To Be Kept (Critical)
- **Logging**: Benchmarks for ErrorReporter, FirstLogWrite, Continuous, Facade, Flush, Masking, Rotation.
- **Navigation**: NavigateTo, NavigationService.
- **Persistence CRUD**: InMemoryGoalRepo_CRUD, InMemoryTaskRepo_CRUD.
- **Settings**: AppSettingsManager, FeatureFlagsManager.
- **Theme**: ThemeManager.
- **Integration**: CreateTaskFlow.

---

## Consequences
- ✅ CI reports will be cleaner and focused only on meaningful subsystems.
- ✅ Faster CI runtime by removing trivial benchmarks.
- ✅ Easier to spot regressions in performance-critical paths.
- ⚠️ Slight loss of visibility on trivial components (but no impact on user-facing performance).

---

## References
- `Subsystems_Unified.md` (Performance Bars definition)
- `Project_Quality_Policy.md` (Benchmarking requirements)
- `phase0x_quality_policies_ordered.md` (Phase0 DoR/DoD for benchmarks)
- Coverage + Benchmark Reports (Phase0_2025-10-02)

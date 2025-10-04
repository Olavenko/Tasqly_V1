# ADR 0013: Defer I/O-Bound Benchmarks

- **Status**: Draft
- **Date**: 2025-10-03
- **Owners**: Tasqly Team
- **Related**: Phase0 — Benchmark Policy, ADR-0012 (Non-Critical Benchmarks Cleanup)
- **Tags**: benchmarks, io-bound, defer, performance, phase1

---

## Context

During Phase0, we created a set of benchmarks to measure performance of subsystems. Coverage reports (Phase0_2025-10-03) show that a number of benchmarks are marked as **I/O-bound** (disk/file operations, persistence side-effects). 

Unlike CPU-bound benchmarks (deterministic, stable, and CI-friendly), I/O-bound benchmarks are:
- Highly **variable** (latency depends on OS load, disk state, etc.).
- **Not meaningful** until real subsystems (PostgreSQL, FileSink) are integrated in Phase1.
- Adding **noise** to CI/CD by creating unstable performance gates.

According to `Subsystems_Unified.md` and our **Phase0 Quality Policies**, Phase0 benchmarks must focus on **CPU-bound logic only**.

---

## Decision

We will **defer all I/O-bound benchmarks** identified in Phase0 reports to **Phase1**, when real persistence (PostgreSQL) and real logging sinks (FileSink rotation) are introduced.

### Benchmarks Deferred
- AppSettingsManager:
  - BM_AppSettingsManager_Save
  - BM_AppSettingsManager_DebouncedSave
  - BM_AppSettingsManager_CorruptFileRecovery
- GoalRepository (I/O variants):
  - BM_GoalRepo_Add_Medium/...
  - BM_GoalRepo_* (where flagged I/O-bound)
- Logging:
  - BM_FirstLogWrite (I/O variants)
  - BM_LogFlush_Enabled (I/O variants)
  - BM_LogManager_RotationTrigger (disk rotation)

### Benchmarks Kept
- All **CPU-bound** benchmarks (Navigation, CRUD InMemory, Result/Error handling, Theme, FeatureFlags, etc.) remain active.

---

## Consequences

- ✅ CI/CD becomes stable and focused on CPU-bound regressions only.
- ✅ Reports are cleaner and avoid false alarms.
- ✅ Documentation is clear that I/O-bound performance is important, but postponed.
- ⚠️ We temporarily lose continuous measurement of I/O-bound paths, but those are not meaningful without real I/O subsystems.

---

## Follow-Up Actions
- Remove I/O-bound benchmarks from `Benchmarks.cmake` in Phase0.
- Create new **Phase1 benchmarks** for PostgreSQL persistence, FileSink logging, and Settings I/O once real infra is integrated.
- Track this as a roadmap item under Phase1 Performance Work.

---

## References
- `Subsystems_Unified.md` (performance bars: log write, navigation, persistence)
- `Project_Quality_Policy.md` (Phase0 benchmark scope)
- `Phase0_2025-10-03_report.md` (coverage & benchmark results)
- ADR-0012 (removal of non-critical benchmarks)

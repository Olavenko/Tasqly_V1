# ADR 0014: Add New CPU-Bound Benchmarks

- **Status**: Draft
- **Date**: 2025-10-03
- **Owners**: Tasqly Team
- **Related**: Phase0 — Benchmark Policy, ADR-0012 (Non-Critical Benchmarks Cleanup), ADR-0013 (Defer I/O-Bound Benchmarks)
- **Tags**: benchmarks, cpu-bound, performance, phase0

---

## Context

After cleaning up non-critical benchmarks (ADR-0012) and deferring I/O-bound benchmarks (ADR-0013), the benchmark suite is now focused and stable. However, analysis of the project tree and coverage reports (Phase0_2025-10-02 & 2025-10-03) shows several **CPU-bound logic paths** without benchmark coverage.

These gaps exist in **use cases** and **core managers** where performance directly affects user experience, especially in daily workflows (task management, theme switching, settings load). Benchmarks for these are critical even in Phase0, because they:
- Are CPU-bound (stable, deterministic).
- Represent hot paths in the application.
- Contribute to KPIs defined in `Subsystems_Unified.md` (navigation <120ms, theme switch <100ms, repo CRUD <5ms).

---

## Decision

We will add new **CPU-bound benchmarks** immediately in Phase0 for the following files:

### Use Cases (src/app/usecases)
- `Bench_ListTasks.cpp` → Benchmark task listing performance (iteration, filtering).
- `Bench_LoadUserSettings.cpp` → Benchmark pure CPU load of deserializing user settings from memory.
- `Bench_SaveUserSettings.cpp` → Benchmark serialization cost of saving settings (CPU part only, without file I/O).
- `Bench_SwitchTheme.cpp` → Benchmark switching theme logic (color application, signals) excluding disk I/O.

### Infra Logging
- `Bench_LogManager_Throughput.cpp` → Benchmark throughput of logging many entries in-memory (e.g., 10k log writes), excluding file sinks.

---

## Consequences

- ✅ Coverage of CPU-bound logic is improved, focusing on hot paths.
- ✅ Benchmark suite remains stable in CI (no I/O noise).
- ✅ Ensures Tasqly meets its Phase0 performance bars on logic-level subsystems.
- ⚠️ Will require 5 new benchmark files under `benchmarks/unit/usecases/` and `benchmarks/unit/logging/`.

---

## Follow-Up Actions

1. Create new benchmark files based on `Development Standards/Templates/BenchmarkTemplate.md`:
   - `benchmarks/unit/usecases/Bench_ListTasks.cpp`
   - `benchmarks/unit/usecases/Bench_LoadUserSettings.cpp`
   - `benchmarks/unit/usecases/Bench_SaveUserSettings.cpp`
   - `benchmarks/unit/usecases/Bench_SwitchTheme.cpp`
   - `benchmarks/unit/logging/Bench_LogManager_Throughput.cpp`

2. Update `cmake/Benchmarks.cmake` to include new files.
3. Add CI/CD reports for the new benchmarks.
4. Document benchmark methodology and sample results in `docs/adr/phase0/Benchmark/`.

---

## References
- `Subsystems_Unified.md` (performance bars: navigation, theme, repos, logging)
- `Project_Quality_Policy.md` (benchmarking scope)
- `phase0x_quality_policies_ordered.md` (DoR/DoD for benchmarks)
- `Development Standards/Templates/BenchmarkTemplate.md`
- Phase0 Coverage & Benchmark Reports (2025-10-02, 2025-10-03)

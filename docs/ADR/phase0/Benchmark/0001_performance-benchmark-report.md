# ADR 0001: performance-benchmark-report

- **Status**: Draft
- **Date**: 2025-09-10
- **Owners**: Tasqly Team
- **Related**: PhaseN — SliceM (Performance Validation)
- **Tags**: performance, benchmark, logging, repository, qml

---

## Context
Tasqly requires clear validation against defined **Performance Bars** to ensure the system meets latency and efficiency targets.  
Benchmarks have been integrated into the build using **Google Benchmark**, and initial measurements were conducted on logging and repository subsystems.

## Decision
Adopt a dedicated `benchmarks/` folder with Google Benchmark integration, and measure key performance bars.  
The following results have been recorded:

| Metric                                    | Target       | Actual          | Status |
|-------------------------------------------|--------------|---------------  |--------|
| Singleton init (QML facades)              | ≤ 50 ms      | ❌ Not Measured |   🔴   |
| First log write avg                       | ≤ 2 ms       | **0.0325 ms**   |   ✅   |
| Theme switch                              | ≤ 100 ms     | ❌ Not Measured |   🟠   |
| Nav change (≤5 components)                | ≤ 120 ms     | ❌ Not Measured |   🟠   |
| File logging rotation (5–10MB, last 3)    | OK (≤ few ms)| **3.08 ms**     |   ✅   |
| In-memory repo queries (≤1k items)        | ≤ 5 ms       | **0.05–0.9 ms** |   ✅   |
| Repo hygiene (.gitignore, clang-format…)  | OK           | OK              |   🟢   |

## Consequences
- ✅ Benchmarks validate critical paths (repo queries, log writes, log rotation) as **within targets**.  
- ⚠️ UI-related metrics (QML singleton init, navigation, theme switch) are deferred until the UI layer is implemented.  
- 🔄 Trade-off: benchmarks tied to Google Benchmark require manual or CI runs, not integrated in unit tests.

## References
- `benchmarks/logging/Bench_FirstLogWrite.cpp`  
- `benchmarks/logging/Bench_LogRotation.cpp`  
- `benchmarks/persistence/Bench_InMemoryRepo.cpp`  
- `TESTING.md` (Performance Bars definition)  

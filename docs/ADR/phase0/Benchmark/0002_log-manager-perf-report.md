# ADR 0002: LogManager Performance Optimization (Preallocation)

- **Status**: Accepted
- **Date**: 2025-09-16
- **Owners**: Tasqly Team
- **Related**: Phase0 — Logging Subsystem (Performance Slice)
- **Tags**: performance, benchmark, logging, preallocation

---

## Context
The `LogManager` subsystem experienced performance spikes when handling large log messages (≥16KB). Benchmarks (`Bench_FirstLogWrite`, `Bench_LogRotation`) showed latency increasing to multiple milliseconds for 16KB messages, caused by frequent memory reallocations during UTF-8 conversions and buffer writes.

This degraded performance could cause logging stalls during intensive operations, violating stability KPIs defined in **Project_Quality_Policy.md**.

---

## Decision
Implement **Preallocated Buffering** inside `LogManager`:
- Added a reusable `QByteArray m_buffer` preallocated to **16KB**.  
- Modified `writeFile_locked()` to reuse this buffer across log writes, avoiding dynamic reallocation on every large log message.  
- Preserved all public API signatures and locking semantics.

---

## Consequences
- ✅ **Unit & Integration Tests**: All **116 tests passed** (0 failed).  
- ✅ **Benchmarks**:
  
| Message Size | Before (Baseline) | After (Preallocation) | Status |
|--------------|-------------------|-----------------------|--------|
| 128B         | ~35 µs           | ~34 µs                | ✅ Stable |
| 1KB          | ~37 µs           | ~36 µs                | ✅ Stable |
| 4KB          | ~70–120 µs (spiky) | ~52 µs (low variance) | ✅ Improved |
| 16KB         | ms-range spikes   | ~88 µs (stable)       | ✅ Major Fix |

- ✅ Rotation: ~4.47 ms (unchanged).  
- ✅ No regressions in masking functionality.  
- ✅ Improved stability at 16KB logs, meeting performance KPIs.  

---

## Alternatives Considered
- **Async Logging Queue (Producer/Consumer)**:
  - Pros: Further reduces `log()` latency under heavy load.
  - Cons: Adds threading complexity, harder test determinism.
  - Decision: Deferred to later slice (if benchmarks or production profiling show throughput bottlenecks).

---

## References
- Benchmark dashboard: `performance/reports/phase0_perf_reports/html/phase0_2025-09-16_dashboard.html`  
- Raw results: `performance/reports/phase0_perf_reports/raw/phase0_2025-09-16_bench_results.json`  
- Source files: `src/infra/logging/LogManager.cpp`, `src/infra/logging/LogManager.h`
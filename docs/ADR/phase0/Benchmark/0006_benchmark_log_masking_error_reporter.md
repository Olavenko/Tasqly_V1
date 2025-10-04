# ADR 0006: Benchmark Coverage for LogMasking & ErrorReporter

- **Status**: Accepted  
- **Date**: 2025-09-25  
- **Owners**: Mohamed Ali, Tasqly AI Assistant  
- **Related**: Phase0 — Benchmarks & Logging  
- **Tags**: logging, benchmarking, performance  

---

## Context
During the development of Tasqly’s logging infrastructure, two measurement gaps were identified:

1. **Lack of context masking benchmarks**  
   - LogManager benchmarks did not cover the overhead introduced by masked key-value contexts.  
   - This made it difficult to evaluate the cost of logging structured data, especially with large contexts (e.g., 100 entries).

2. **ErrorReporter performance unverified**  
   - ErrorReporter is a central API for structured error reporting, but no benchmarks existed to validate its runtime overhead.  
   - Without measurement, the performance impact of delegation to ILogManager (via NullLogger in tests) was uncertain.

---

## Decision
- Introduced **Bench_LogMasking.cpp** to measure the overhead of masked contexts:
  - Scenarios: no context, small context (10 keys), large context (100 keys).
  - Results: Overhead scaled from ~0.002ms (no context) → ~0.033ms (100 keys), which is acceptable.

- Introduced **Bench_ErrorReporter.cpp** using a NullLogger stub:
  - Covers `report()` with structured Error and `reportRaw()` API.
  - Results: Both measured ~0.000ms per call across 1M iterations, confirming negligible overhead.

---

## Consequences
- Benchmark suite now provides full coverage of **logging overheads**:
  - Context size scaling in LogManager.
  - Delegation cost in ErrorReporter.
- Confirms that both features are safe to use extensively in production, with predictable performance.
- Establishes baselines for future regression detection in CI.

---

## References
- `Bench_LogMasking.cpp` (2025-09-23)
- `Bench_ErrorReporter.cpp` (2025-09-22)
- Benchmark Report — Phase0 (2025-09-25)
- ADR Template — `ADR-template.md`
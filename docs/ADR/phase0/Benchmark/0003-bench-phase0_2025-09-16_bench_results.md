# ADR 0003: Benchmark Report (phase0_2025-09-16_bench_results)

- Date: 2025-09-16
- Status: Accepted
- Context: Phase0 benchmark report converted from JSON artifact.

## Decision
Benchmark results have been captured and compared against KPI thresholds.

## Consequences
- Report stored under `reports/benchmarks/phase0_perf_reports` with suffix `_report.*`
- KPI evaluation logged for CRUD ×10k benchmark.
- Issues with `cpu_time = 0` will be tracked as DeferredTask.

## Metadata
- Host: DYANKO
- CPUs: 8
- Executable: C:\Users\Olavenko\Desktop\TasqlyQt\Tasqly_V1\build\mingw-release\TasqlyBenchmarksRunner.exe
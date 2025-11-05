# Benchmark Report (MSVC)
_Phase: Phase1 — 2025-11-05_

**Commit**: local  
**Job**: manual  
**Host**: DYANKO  
**CPU**: 8 cores @ 3600 MHz  
**Runner**: C:\Users\Olavenko\Desktop\TasqlyQt\Tasqly_V1\build\msvc-benchmarks-release\TasqlyBenchmarksRunner.exe  
**Execution Time**: 0 min 10 sec  
**Generated On**: 11:24 AM — 2025-11-05

---

**Best Benchmark**: BM_Task_Construct (0.000 ms)

**Worst Benchmark**: BM_TaskMapper_TimeHelpers_10000 (40.837 ms)

---

## Slow Benchmarks

No slow benchmarks found.

## Medium Variance Benchmarks

| Benchmark                       | Iterations | Real Time (ms) | CPU Time (ms) | Variance (ms) | Threads | Ops/sec | Relative  | Type      | Baseline (ms) | Speedup (x) | Time (ms) | Change (%) | Trend |
|---------------------------------|------------|----------------|---------------|---------------|---------|---------|-----------|-----------|---------------|-------------|-----------|------------|-------|
| BM_TaskMapper_TimeHelpers_10000 |         17 |         40.837 |        41.360 |   -0.523      |       1 |     416 | 643015.9x | CPU-bound | -             | -           | -         | NEW        | New   |
| BM_TaskMapper_ToDto_1000        |         90 |          8.222 |         8.160 |    0.063      |       1 | 10.95K  | 129467.5x | CPU-bound | -             | -           | -         | NEW        | New   |

## Stable Benchmarks

| Benchmark                       | Iterations | Real Time (ms) | CPU Time (ms) | Variance (ms) | Threads | Ops/sec    | Relative | Type      | Baseline (ms) | Speedup (x) | Time (ms) | Change (%) | Trend |
|---------------------------------|------------|----------------|---------------|---------------|---------|------------|----------|-----------|---------------|-------------|-----------|------------|-------|
| BM_TaskMapper_FromDto_1000      |       1000 |          0.534 |         0.531 |    0.003      |       1 | 1.87M      | 8405.1x  | CPU-bound | -             | -           | -         | NEW        | New   |
| BM_GenerateUuid_1000            |       2133 |          0.344 |         0.322 |    0.021      |       1 | 6.21M      | 5409.2x  | CPU-bound | -             | -           | -         | NEW        | New   |
| BM_ValidateDeadline_Past_1000   |       8960 |          0.075 |         0.073 |    0.002      |       1 | 119.59M    | 1179.7x  | CPU-bound | -             | -           | -         | NEW        | New   |
| BM_ValidateTitle_Empty_1000     |      10000 |          0.052 |         0.052 |    0.000      |       1 | 193.37M    | 814.3x   | CPU-bound | -             | -           | -         | NEW        | New   |
| BM_ValidateTitle_Long_1000      |      11200 |          0.052 |         0.052 |    0.000      |       1 | 216.64M    | 814.0x   | CPU-bound | -             | -           | -         | NEW        | New   |
| BM_ValidateDeadline_Future_1000 |      28000 |          0.024 |         0.024 |    0.000      |       1 | 1.15B      | 383.6x   | CPU-bound | -             | -           | -         | NEW        | New   |
| BM_ValidateTitle_Valid_1000     |     100000 |          0.005 |         0.005 |   -0.000      |       1 | 18.92B     | 83.2x    | CPU-bound | -             | -           | -         | NEW        | New   |
| BM_Task_Construct               |    1947826 |          0.000 |         0.000 |   -0.000      |       1 | 5267.24B   | 5.8x     | CPU-bound | -             | -           | -         | NEW        | New   |
| BM_Task_Copy                    |   11200000 |          0.000 |         0.000 |   -0.000      |       1 | 176353.43B | 1.0x     | CPU-bound | -             | -           | -         | NEW        | New   |
| BM_Task_Move                    |    1866667 |          0.000 |         0.000 |    0.000      |       1 | 4998.26B   | 5.9x     | CPU-bound | -             | -           | -         | NEW        | New   |

---

## Comparison Summary

- **Faster:** 0
- **Slower:** 0
- **Same:** 0
- **New:** 12
- **Overall Improvement:** +0.00%


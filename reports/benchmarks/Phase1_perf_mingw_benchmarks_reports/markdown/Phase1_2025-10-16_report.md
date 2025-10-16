# Benchmark Report (MINGW)
_Phase: Phase1 — 2025-10-16_

**Commit**: local  
**Job**: manual  
**Host**: DYANKO  
**CPU**: 8 cores @ 3600 MHz  
**Runner**: C:\Users\Olavenko\Desktop\TasqlyQt\Tasqly_V1\build\mingw-benchmarks-release\TasqlyBenchmarksRunner.exe  
**Execution Time**: 0 min 10 sec  
**Generated On**: 12:15 PM — 2025-10-16

---

**Best Benchmark**: BM_Task_Construct (0.000 ms)

**Worst Benchmark**: BM_TaskMapper_TimeHelpers_10000 (3.400 ms)

---

## Slow Benchmarks

No slow benchmarks found.

## Medium Variance Benchmarks

| Benchmark                       | Iterations | Real Time (ms) | CPU Time (ms) | Variance (ms) | Threads | Ops/sec | Relative | Type      | Baseline (ms) | Speedup (x) | Time (ms) | Change (%) | Trend  |
|---------------------------------|------------|----------------|---------------|---------------|---------|---------|----------|-----------|---------------|-------------|-----------|------------|--------|
| BM_TaskMapper_TimeHelpers_10000 |        204 |          3.400 |         3.370 |    0.030      |       1 | 60.00K  | 56062.5x | CPU-bound |        10.119 | 2.98x       | -6.719    | 66.40%     | Faster |

## Stable Benchmarks

| Benchmark                       | Iterations | Real Time (ms) | CPU Time (ms) | Variance (ms) | Threads | Ops/sec    | Relative | Type      | Baseline (ms) | Speedup (x) | Time (ms) | Change (%) | Trend  |
|---------------------------------|------------|----------------|---------------|---------------|---------|------------|----------|-----------|---------------|-------------|-----------|------------|--------|
| BM_TaskMapper_ToDto_1000        |       1120 |          0.628 |         0.600 |    0.028      |       1 | 1.78M      | 10355.5x | CPU-bound |         0.423 | 0.67x       | +0.205    | 48.32%     | Slower |
| BM_TaskMapper_FromDto_1000      |       1867 |          0.379 |         0.377 |    0.003      |       1 | 4.92M      | 6251.5x  | CPU-bound |         2.774 | 7.32x       | -2.395    | 86.33%     | Faster |
| BM_GenerateUuid_1000            |       2635 |          0.257 |         0.255 |    0.002      |       1 | 10.25M     | 4239.8x  | CPU-bound |         1.085 | 4.22x       | -0.828    | 76.29%     | Faster |
| BM_ValidateDeadline_Past_1000   |       8960 |          0.080 |         0.080 |    0.000      |       1 | 111.65M    | 1323.3x  | CPU-bound |         0.081 | 1.02x       | -0.001    | 1.53%      | Faster |
| BM_ValidateTitle_Empty_1000     |      11200 |          0.051 |         0.049 |    0.002      |       1 | 218.79M    | 844.1x   | CPU-bound |         0.051 | 1.00x       | -0.000    | 0.15%      | Same   |
| BM_ValidateTitle_Long_1000      |      11200 |          0.051 |         0.047 |    0.004      |       1 | 219.35M    | 842.0x   | CPU-bound |         0.051 | 1.00x       | +0.000    | 0.23%      | Same   |
| BM_ValidateDeadline_Future_1000 |      21333 |          0.032 |         0.029 |    0.003      |       1 | 661.44M    | 531.8x   | CPU-bound |         0.032 | 1.01x       | -0.000    | 0.50%      | Faster |
| BM_ValidateTitle_Valid_1000     |     280000 |          0.003 |         0.002 |    0.000      |       1 | 110.22B    | 41.9x    | CPU-bound |         0.003 | 1.00x       | -0.000    | 0.28%      | Same   |
| BM_Task_Construct               |    2357895 |          0.000 |         0.000 |    0.000      |       1 | 8069.63B   | 4.8x     | CPU-bound |         0.001 | 3.83x       | -0.001    | 73.86%     | Faster |
| BM_Task_Copy                    |   10000000 |          0.000 |         0.000 |    0.000      |       1 | 164891.74B | 1.0x     | CPU-bound |         0.000 | 1.01x       | -0.000    | 0.71%      | Faster |
| BM_Task_Move                    |    2635294 |          0.000 |         0.000 |   -0.000      |       1 | 9142.38B   | 4.8x     | CPU-bound |         0.001 | 3.89x       | -0.001    | 74.32%     | Faster |

---

## Comparison Summary

- **Faster:** 8
- **Slower:** 1
- **Same:** 3
- **New:** 0
- **Overall Improvement:** +27.65%


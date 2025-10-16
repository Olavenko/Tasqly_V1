# Benchmark Report (MINGW)
_Phase: Phase1 — 2025-10-15_

**Commit**: local  
**Job**: manual  
**Host**: DYANKO  
**CPU**: 8 cores @ 3600 MHz  
**Runner**: C:\Users\Olavenko\Desktop\TasqlyQt\Tasqly_V1\build\mingw-benchmarks-release\TasqlyBenchmarksRunner.exe  
**Execution Time**: 0 min 10 sec  
**Generated On**: 04:37 PM — 2025-10-15

---

**Best Benchmark**: BM_Task_Copy (0.000 ms)

**Worst Benchmark**: BM_TaskMapper_TimeHelpers_10000 (10.119 ms)

---

## Slow Benchmarks

No slow benchmarks found. Performance is stable for this run.

## Medium Variance Benchmarks

| Benchmark                       | Iterations | Real Time (ms) | CPU Time (ms) | Variance (ms) | Threads | Ops/sec | Relative  | Type      |
|---------------------------------|------------|----------------|---------------|---------------|---------|---------|-----------|-----------|
| BM_TaskMapper_TimeHelpers_10000 |         64 |         10.119 |         9.766 |         0.353 |       1 |   6.32K | 165668.7x | CPU-bound |
| BM_TaskMapper_FromDto_1000      |        264 |          2.774 |         2.663 |         0.110 |       1 |  95.18K | 45410.3x  | CPU-bound |
| BM_GenerateUuid_1000            |        640 |          1.085 |         1.074 |         0.010 |       1 | 590.05K | 17757.8x  | CPU-bound |

## Stable Benchmarks

| Benchmark                       | Iterations | Real Time (ms) | CPU Time (ms) | Variance (ms) | Threads | Ops/sec    | Relative | Type      |
|---------------------------------|------------|----------------|---------------|---------------|---------|------------|----------|-----------|
| BM_TaskMapper_ToDto_1000        |       1600 |          0.423 |         0.420 |         0.003 |       1 |      3.78M | 6932.0x  | CPU-bound |
| BM_ValidateDeadline_Past_1000   |       8960 |          0.081 |         0.078 |         0.003 |       1 |    109.94M | 1334.3x  | CPU-bound |
| BM_ValidateTitle_Empty_1000     |      10000 |          0.051 |         0.052 |        -0.000 |       1 |    195.05M | 839.4x   | CPU-bound |
| BM_ValidateTitle_Long_1000      |      10000 |          0.051 |         0.052 |        -0.001 |       1 |    196.30M | 834.0x   | CPU-bound |
| BM_ValidateDeadline_Future_1000 |      21333 |          0.032 |         0.031 |         0.001 |       1 |    658.13M | 530.7x   | CPU-bound |
| BM_ValidateTitle_Valid_1000     |     263529 |          0.003 |         0.003 |        -0.000 |       1 |    103.45B | 41.7x    | CPU-bound |
| BM_Task_Construct               |     640000 |          0.001 |         0.001 |         0.000 |       1 |    572.59B | 18.3x    | CPU-bound |
| BM_Task_Move                    |     640000 |          0.001 |         0.001 |         0.000 |       1 |    570.26B | 18.4x    | CPU-bound |
| BM_Task_Copy                    |   11200000 |          0.000 |         0.000 |         0.000 |       1 | 183364.84B | 1.0x     | CPU-bound |


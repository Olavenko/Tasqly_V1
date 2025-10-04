# Benchmark Report (MSVC)
_Phase: Phase0 — 2025-10-04_

**Commit**: local  
**Job**: manual  
**Host**: DYANKO  
**CPU**: 8 cores @ 3600 MHz  
**Runner**: C:\Users\Olavenko\Desktop\TasqlyQt\Tasqly_V1\build\msvc-benchmarks-release\TasqlyBenchmarksRunner.exe  
**Execution Time**: 5 min 31 sec  
**Generated On**: 11:43 AM — 2025-10-04

---

**Best Benchmark**: BM_ErrorReporter_Report/iterations:1000000/repeats:5 (0.000 ms)

**Worst Benchmark**: BM_LogManager_Throughput/10000 (1.508 ms)

---

## Slow Benchmarks

No slow benchmarks found. Performance is stable for this run.

## Medium Variance Benchmarks

| Benchmark                      | Iterations | Real Time (ms) | CPU Time (ms) | Variance (ms) | Threads | Ops/sec | Relative  | Type      |
|--------------------------------|------------|----------------|---------------|---------------|---------|---------|-----------|-----------|
| BM_LogManager_Throughput/10000 |        640 |          1.508 |         1.123 |         0.385 |       1 | 424.40K | 644560.9x | CPU-bound |

## Stable Benchmarks

| Benchmark                                               | Iterations | Real Time (ms) | CPU Time (ms) | Variance (ms) | Threads | Ops/sec       | Relative  | Type      |
|---------------------------------------------------------|------------|----------------|---------------|---------------|---------|---------------|-----------|-----------|
| BM_FeatureFlagsManager_BulkInsert                       |       1867 |          0.447 |         0.427 |         0.021 |       1 |         4.17M | 191216.4x | CPU-bound |
| BM_AppSettingsManager_Load                              |       4480 |          0.259 |         0.174 |         0.084 |       1 |        17.33M | 110524.5x | CPU-bound |
| BM_LogManager_Throughput/1000                           |       5600 |          0.154 |         0.120 |         0.034 |       1 |        36.30M | 65932.4x  | CPU-bound |
| BM_TaskRepo_AddBulk                                     |       5973 |          0.143 |         0.118 |         0.025 |       1 |        41.72M | 61193.9x  | CPU-bound |
| BM_AppSettingsManager_SetThemeAndSave                   |      18667 |          0.103 |         0.051 |         0.052 |       1 |       181.14M | 44046.6x  | I/O-bound |
| BM_LogContinuous_FileOnly/16384/iterations:1000000      |    1000000 |          0.062 |         0.033 |         0.028 |       1 |        16.19B | 26403.7x  | CPU-bound |
| BM_LogContinuous_FileOnly/4096/iterations:1000000       |    1000000 |          0.016 |         0.011 |         0.005 |       1 |        63.30B | 6752.1x   | CPU-bound |
| BM_LogManager_Throughput/100                            |      49778 |          0.016 |         0.013 |         0.003 |       1 |         3.09B | 6891.6x   | CPU-bound |
| BM_SwitchTheme_Transition                               |      74667 |          0.012 |         0.008 |         0.003 |       1 |         6.45B | 4951.1x   | CPU-bound |
| BM_NavigateTo_RetryValid                                |      74667 |          0.011 |         0.009 |         0.002 |       1 |         6.70B | 4764.7x   | CPU-bound |
| BM_NavigateTo_ExecuteValid                              |     112000 |          0.007 |         0.005 |         0.001 |       1 |        16.28B | 2941.1x   | CPU-bound |
| BM_LogContinuous_FileOnly/1024/iterations:1000000       |    1000000 |          0.005 |         0.005 |         0.001 |       1 |       185.17B | 2308.3x   | CPU-bound |
| BM_LogContinuous_FileOnly/128/iterations:1000000        |    1000000 |          0.003 |         0.003 |         0.000 |       1 |       313.31B | 1364.2x   | CPU-bound |
| BM_NavigationService_NavigateValid                      |     224000 |          0.003 |         0.003 |         0.001 |       1 |        73.87B | 1296.1x   | CPU-bound |
| BM_NavigationService_GoBack                             |     224000 |          0.003 |         0.003 |         0.000 |       1 |        80.26B | 1192.9x   | CPU-bound |
| BM_NavigationService_RetryLast                          |     280000 |          0.003 |         0.003 |         0.000 |       1 |        91.55B | 1307.3x   | CPU-bound |
| BM_TaskRepo_Add_CopyMove                                |     263529 |          0.003 |         0.003 |         0.000 |       1 |        85.37B | 1319.4x   | CPU-bound |
| BM_CreateTaskFlow/iterations:10000                      |      10000 |          0.002 |         0.002 |         0.000 |       1 |         5.31B | 804.9x    | CPU-bound |
| BM_NavigateTo_ExecuteInvalid                            |     560000 |          0.002 |         0.001 |         0.000 |       1 |       341.51B | 700.9x    | CPU-bound |
| BM_TaskRepo_Remove                                      |     448000 |          0.002 |         0.001 |         0.000 |       1 |       274.07B | 698.7x    | CPU-bound |
| BM_NavigationService_NavigateInvalid                    |     896000 |          0.001 |         0.001 |         0.000 |       1 |      1079.04B | 354.9x    | CPU-bound |
| BM_TaskRepo_Update_CopyMove                             |     560000 |          0.001 |         0.001 |         0.000 |       1 |       485.25B | 493.3x    | CPU-bound |
| BM_TaskRepo_List                                        |     896000 |          0.001 |         0.001 |         0.000 |       1 |      1078.96B | 354.9x    | CPU-bound |
| BM_FeatureFlagsManager_Signal                           |    1000000 |          0.001 |         0.001 |         0.000 |       1 |      1273.44B | 335.6x    | CPU-bound |
| BM_ErrorReporter_Report/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |     20066.90B | 21.3x     | CPU-bound |
| BM_ErrorReporter_Report/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |     19261.62B | 22.2x     | CPU-bound |
| BM_ErrorReporter_Report/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |     19112.05B | 22.4x     | CPU-bound |
| BM_ErrorReporter_Report/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |     19580.81B | 21.8x     | CPU-bound |
| BM_ErrorReporter_Report/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |     19845.01B | 21.5x     | CPU-bound |
| BM_ErrorReporter_ReportRaw/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      9958.27B | 42.9x     | CPU-bound |
| BM_ErrorReporter_ReportRaw/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |     12038.72B | 35.5x     | CPU-bound |
| BM_ErrorReporter_ReportRaw/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |     14094.43B | 30.3x     | CPU-bound |
| BM_ErrorReporter_ReportRaw/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |     13245.58B | 32.3x     | CPU-bound |
| BM_ErrorReporter_ReportRaw/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |     13843.67B | 30.9x     | CPU-bound |
| BM_LogContinuous_NullSink/128/iterations:1000000        |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      4672.48B | 91.5x     | CPU-bound |
| BM_LogContinuous_NullSink/1024/iterations:1000000       |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      4667.42B | 91.6x     | CPU-bound |
| BM_LogContinuous_NullSink/4096/iterations:1000000       |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      4822.81B | 88.6x     | CPU-bound |
| BM_LogContinuous_NullSink/16384/iterations:1000000      |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      4540.07B | 94.1x     | CPU-bound |
| BM_LogFacade_Info                                       |    7466667 |          0.000 |         0.000 |         0.000 |       1 |     74304.18B | 43.0x     | CPU-bound |
| BM_LogFacade_Warn                                       |    8960000 |          0.000 |         0.000 |         0.000 |       1 |     85852.44B | 44.6x     | CPU-bound |
| BM_LogFacade_Error                                      |    7466667 |          0.000 |         0.000 |         0.000 |       1 |     69067.45B | 46.2x     | CPU-bound |
| BM_LogMasking_NoContext/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      2243.28B | 190.5x    | CPU-bound |
| BM_LogMasking_NoContext/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      2948.68B | 145.0x    | CPU-bound |
| BM_LogMasking_NoContext/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      2854.68B | 149.7x    | CPU-bound |
| BM_LogMasking_NoContext/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      2603.77B | 164.2x    | CPU-bound |
| BM_LogMasking_NoContext/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      2033.57B | 210.2x    | CPU-bound |
| BM_LogMasking_SmallContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      2489.58B | 171.7x    | CPU-bound |
| BM_LogMasking_SmallContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      2982.58B | 143.3x    | CPU-bound |
| BM_LogMasking_SmallContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      2790.07B | 153.2x    | CPU-bound |
| BM_LogMasking_SmallContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      2340.49B | 182.6x    | CPU-bound |
| BM_LogMasking_SmallContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      2948.94B | 144.9x    | CPU-bound |
| BM_LogMasking_LargeContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      3016.86B | 141.7x    | CPU-bound |
| BM_LogMasking_LargeContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      2716.42B | 157.3x    | CPU-bound |
| BM_LogMasking_LargeContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      3092.07B | 138.2x    | CPU-bound |
| BM_LogMasking_LargeContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      2726.01B | 156.8x    | CPU-bound |
| BM_LogMasking_LargeContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      2867.54B | 149.1x    | CPU-bound |
| BM_NavigateTo_RetryInvalid                              |    3446154 |          0.000 |         0.000 |         0.000 |       1 |     14805.36B | 99.5x     | CPU-bound |
| BM_NavigationService_CurrentRoute                       |   20363636 |          0.000 |         0.000 |         0.000 |       1 |    558486.11B | 15.6x     | CPU-bound |
| BM_TaskRepo_GetById                                     |    1947826 |          0.000 |         0.000 |         0.000 |       1 |      4866.91B | 171.1x    | CPU-bound |
| BM_FeatureFlagsManager_Get                              |    7466667 |          0.000 |         0.000 |         0.000 |       1 |     63793.02B | 50.0x     | CPU-bound |
| BM_FeatureFlagsManager_SetToggle                        |    2036364 |          0.000 |         0.000 |         0.000 |       1 |      5991.16B | 145.3x    | CPU-bound |
| BM_ThemeManager_Mode                                    |  344615385 |          0.000 |         0.000 |         0.000 |       1 | 147298292.13B | 1.0x      | CPU-bound |
| BM_ThemeManager_SwitchMode                              |    4977778 |          0.000 |         0.000 |         0.000 |       1 |     33987.77B | 62.6x     | CPU-bound |
| BM_ThemeManager_Palette                                 |   44800000 |          0.000 |         0.000 |         0.000 |       1 |   2954810.99B | 6.5x      | CPU-bound |
| BM_ThemeManager_Fonts                                   |   56000000 |          0.000 |         0.000 |         0.000 |       1 |   3646883.95B | 6.6x      | CPU-bound |
| BM_ListTasks/10                                         |    2357895 |          0.000 |         0.000 |         0.000 |       1 |      7129.54B | 141.4x    | CPU-bound |
| BM_ListTasks/100                                        |    2800000 |          0.000 |         0.000 |         0.000 |       1 |      8530.20B | 140.3x    | CPU-bound |
| BM_ListTasks/1000                                       |    3200000 |          0.000 |         0.000 |         0.000 |       1 |      9697.11B | 141.0x    | CPU-bound |
| BM_LoadUserSettings                                     |    2986667 |          0.000 |         0.000 |         0.000 |       1 |      9796.32B | 130.3x    | CPU-bound |
| BM_SaveUserSettings                                     |    2240000 |          0.000 |         0.000 |         0.000 |       1 |      7171.23B | 133.5x    | CPU-bound |
| BM_SaveUserSettings_ForcedFlush                         |    2635294 |          0.000 |         0.000 |         0.000 |       1 |      8632.55B | 130.5x    | CPU-bound |
| BM_SwitchTheme_NoOp                                     |  100000000 |          0.000 |         0.000 |         0.000 |       1 |  16036311.34B | 2.7x      | CPU-bound |


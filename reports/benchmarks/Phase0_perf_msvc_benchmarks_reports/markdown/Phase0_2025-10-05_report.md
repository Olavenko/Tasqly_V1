# Benchmark Report (MSVC)
_Phase: Phase0 — 2025-10-05_

**Commit**: local  
**Job**: manual  
**Host**: DYANKO  
**CPU**: 8 cores @ 3600 MHz  
**Runner**: C:\Users\Olavenko\Desktop\TasqlyQt\Tasqly_V1\build\msvc-benchmarks-release\TasqlyBenchmarksRunner.exe  
**Execution Time**: 2 min 34 sec  
**Generated On**: 04:44 PM — 2025-10-05

---

**Best Benchmark**: BM_ErrorReporter_Report/iterations:1000000/repeats:5 (0.000 ms)

**Worst Benchmark**: BM_LogManager_Throughput/10000 (1.406 ms)

---

## Slow Benchmarks

No slow benchmarks found. Performance is stable for this run.

## Medium Variance Benchmarks

| Benchmark                      | Iterations | Real Time (ms) | CPU Time (ms) | Variance (ms) | Threads | Ops/sec | Relative  | Type      |
|--------------------------------|------------|----------------|---------------|---------------|---------|---------|-----------|-----------|
| BM_LogManager_Throughput/10000 |        498 |          1.406 |         1.381 |         0.025 |       1 | 354.29K | 702995.9x | CPU-bound |

## Stable Benchmarks

| Benchmark                                               | Iterations | Real Time (ms) | CPU Time (ms) | Variance (ms) | Threads | Ops/sec       | Relative  | Type      |
|---------------------------------------------------------|------------|----------------|---------------|---------------|---------|---------------|-----------|-----------|
| BM_FeatureFlagsManager_BulkInsert                       |       1659 |          0.386 |         0.367 |         0.019 |       1 |         4.30M | 192980.2x | CPU-bound |
| BM_AppSettingsManager_Load                              |       2635 |          0.222 |         0.285 |        -0.063 |       1 |        11.87M | 111012.1x | CPU-bound |
| BM_LogManager_Throughput/1000                           |       5600 |          0.129 |         0.128 |         0.000 |       1 |        43.55M | 64310.7x  | CPU-bound |
| BM_TaskRepo_AddBulk                                     |       6292 |          0.103 |         0.102 |         0.001 |       1 |        60.95M | 51633.5x  | CPU-bound |
| BM_AppSettingsManager_SetThemeAndSave                   |       2007 |          0.056 |         0.304 |        -0.247 |       1 |        35.58M | 28208.4x  | CPU-bound |
| BM_LogContinuous_FileOnly/16384/iterations:1000000      |    1000000 |          0.050 |         0.038 |         0.012 |       1 |        19.99B | 25023.9x  | CPU-bound |
| BM_LogContinuous_FileOnly/4096/iterations:1000000       |    1000000 |          0.015 |         0.011 |         0.004 |       1 |        68.15B | 7338.3x   | CPU-bound |
| BM_LogManager_Throughput/100                            |      40727 |          0.014 |         0.015 |        -0.000 |       1 |         2.85B | 7141.2x   | CPU-bound |
| BM_NavigateTo_RetryValid                                |      89600 |          0.009 |         0.009 |         0.000 |       1 |        10.46B | 4285.2x   | CPU-bound |
| BM_NavigateTo_ExecuteValid                              |     112000 |          0.006 |         0.006 |         0.000 |       1 |        19.86B | 2820.8x   | CPU-bound |
| BM_SwitchTheme_Transition                               |     224000 |          0.006 |         0.003 |         0.003 |       1 |        39.31B | 2850.0x   | CPU-bound |
| BM_LogContinuous_FileOnly/1024/iterations:1000000       |    1000000 |          0.005 |         0.005 |         0.000 |       1 |       197.83B | 2528.1x   | CPU-bound |
| BM_LogContinuous_FileOnly/128/iterations:1000000        |    1000000 |          0.003 |         0.003 |         0.000 |       1 |       354.47B | 1410.9x   | CPU-bound |
| BM_NavigationService_NavigateValid                      |     280000 |          0.003 |         0.003 |        -0.000 |       1 |       110.17B | 1271.1x   | CPU-bound |
| BM_NavigationService_GoBack                             |     280000 |          0.003 |         0.002 |         0.000 |       1 |       110.23B | 1270.4x   | CPU-bound |
| BM_NavigationService_RetryLast                          |     298667 |          0.003 |         0.003 |        -0.000 |       1 |       116.77B | 1279.2x   | CPU-bound |
| BM_TaskRepo_Add_CopyMove                                |     248889 |          0.003 |         0.003 |         0.000 |       1 |        89.52B | 1390.4x   | CPU-bound |
| BM_CreateTaskFlow/iterations:10000                      |      10000 |          0.002 |         0.002 |         0.000 |       1 |         6.28B | 796.0x    | CPU-bound |
| BM_NavigateTo_ExecuteInvalid                            |     560000 |          0.001 |         0.001 |         0.000 |       1 |       455.02B | 615.5x    | CPU-bound |
| BM_NavigationService_NavigateInvalid                    |     896000 |          0.001 |         0.001 |         0.000 |       1 |      1385.66B | 323.4x    | CPU-bound |
| BM_TaskRepo_Update_CopyMove                             |     640000 |          0.001 |         0.001 |         0.000 |       1 |       610.33B | 524.4x    | CPU-bound |
| BM_TaskRepo_Remove                                      |     497778 |          0.001 |         0.001 |         0.000 |       1 |       356.19B | 698.9x    | CPU-bound |
| BM_TaskRepo_List                                        |     896000 |          0.001 |         0.001 |         0.000 |       1 |      1282.72B | 349.4x    | CPU-bound |
| BM_FeatureFlagsManager_Signal                           |    1120000 |          0.001 |         0.001 |         0.000 |       1 |      1869.68B | 299.6x    | CPU-bound |
| BM_ErrorReporter_Report/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |     20101.43B | 24.9x     | CPU-bound |
| BM_ErrorReporter_Report/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |     20174.35B | 24.8x     | CPU-bound |
| BM_ErrorReporter_Report/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |     18229.58B | 27.4x     | CPU-bound |
| BM_ErrorReporter_Report/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |     18234.03B | 27.4x     | CPU-bound |
| BM_ErrorReporter_Report/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |     20242.01B | 24.7x     | CPU-bound |
| BM_ErrorReporter_ReportRaw/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |     13956.46B | 35.8x     | CPU-bound |
| BM_ErrorReporter_ReportRaw/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |     13969.72B | 35.8x     | CPU-bound |
| BM_ErrorReporter_ReportRaw/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |     13950.29B | 35.9x     | CPU-bound |
| BM_ErrorReporter_ReportRaw/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |     13952.35B | 35.8x     | CPU-bound |
| BM_ErrorReporter_ReportRaw/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |     13947.76B | 35.9x     | CPU-bound |
| BM_LogContinuous_NullSink/128/iterations:1000000        |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |      4952.57B | 101.0x    | CPU-bound |
| BM_LogContinuous_NullSink/1024/iterations:1000000       |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      4921.89B | 101.6x    | CPU-bound |
| BM_LogContinuous_NullSink/4096/iterations:1000000       |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |      4926.27B | 101.5x    | CPU-bound |
| BM_LogContinuous_NullSink/16384/iterations:1000000      |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      4939.51B | 101.3x    | CPU-bound |
| BM_LogFacade_Info                                       |    5600000 |          0.000 |         0.000 |        -0.000 |       1 |     61957.23B | 45.2x     | CPU-bound |
| BM_LogFacade_Warn                                       |    8960000 |          0.000 |         0.000 |         0.000 |       1 |     96742.42B | 46.3x     | CPU-bound |
| BM_LogFacade_Error                                      |    5600000 |          0.000 |         0.000 |         0.000 |       1 |     60604.26B | 46.2x     | CPU-bound |
| BM_LogMasking_NoContext/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      3115.14B | 160.5x    | CPU-bound |
| BM_LogMasking_NoContext/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |      3282.60B | 152.4x    | CPU-bound |
| BM_LogMasking_NoContext/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      3539.70B | 141.3x    | CPU-bound |
| BM_LogMasking_NoContext/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      3354.18B | 149.1x    | CPU-bound |
| BM_LogMasking_NoContext/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      3428.93B | 145.9x    | CPU-bound |
| BM_LogMasking_SmallContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      3490.47B | 143.3x    | CPU-bound |
| BM_LogMasking_SmallContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      3367.76B | 148.5x    | CPU-bound |
| BM_LogMasking_SmallContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |      3474.59B | 143.9x    | CPU-bound |
| BM_LogMasking_SmallContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      3535.74B | 141.5x    | CPU-bound |
| BM_LogMasking_SmallContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      3536.02B | 141.4x    | CPU-bound |
| BM_LogMasking_LargeContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |      3558.50B | 140.5x    | CPU-bound |
| BM_LogMasking_LargeContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |      3596.60B | 139.1x    | CPU-bound |
| BM_LogMasking_LargeContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |      3592.34B | 139.2x    | CPU-bound |
| BM_LogMasking_LargeContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |      3576.94B | 139.8x    | CPU-bound |
| BM_LogMasking_LargeContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |      3587.11B | 139.4x    | CPU-bound |
| BM_NavigateTo_RetryInvalid                              |    3446154 |          0.000 |         0.000 |         0.000 |       1 |     17246.67B | 99.9x     | CPU-bound |
| BM_NavigationService_CurrentRoute                       |   22400000 |          0.000 |         0.000 |        -0.000 |       1 |    723673.87B | 15.5x     | CPU-bound |
| BM_TaskRepo_GetById                                     |    1947826 |          0.000 |         0.000 |         0.000 |       1 |      5488.20B | 177.5x    | CPU-bound |
| BM_FeatureFlagsManager_Get                              |    6400000 |          0.000 |         0.000 |        -0.000 |       1 |     59586.92B | 53.7x     | CPU-bound |
| BM_FeatureFlagsManager_SetToggle                        |    2133333 |          0.000 |         0.000 |         0.000 |       1 |      6252.74B | 170.6x    | CPU-bound |
| BM_ThemeManager_Mode                                    |  344615385 |          0.000 |         0.000 |         0.000 |       1 | 172353722.29B | 1.0x      | CPU-bound |
| BM_ThemeManager_SwitchMode                              |    4977778 |          0.000 |         0.000 |         0.000 |       1 |     37184.92B | 67.0x     | CPU-bound |
| BM_ThemeManager_Palette                                 |   44800000 |          0.000 |         0.000 |         0.000 |       1 |   3067488.60B | 7.3x      | CPU-bound |
| BM_ThemeManager_Fonts                                   |   44800000 |          0.000 |         0.000 |         0.000 |       1 |   2971526.47B | 7.5x      | CPU-bound |
| BM_ListTasks/10                                         |    2357895 |          0.000 |         0.000 |         0.000 |       1 |      8132.16B | 145.0x    | CPU-bound |
| BM_ListTasks/100                                        |    2488889 |          0.000 |         0.000 |         0.000 |       1 |      8568.88B | 145.3x    | CPU-bound |
| BM_ListTasks/1000                                       |    2036364 |          0.000 |         0.000 |        -0.000 |       1 |      6686.07B | 152.3x    | CPU-bound |
| BM_LoadUserSettings                                     |    2488889 |          0.000 |         0.000 |        -0.000 |       1 |      9086.89B | 137.0x    | CPU-bound |
| BM_SaveUserSettings                                     |    2635294 |          0.000 |         0.000 |         0.000 |       1 |      9470.67B | 139.2x    | CPU-bound |
| BM_SaveUserSettings_ForcedFlush                         |    2240000 |          0.000 |         0.000 |         0.000 |       1 |      8038.61B | 139.4x    | CPU-bound |
| BM_SwitchTheme_NoOp                                     |  112000000 |          0.000 |         0.000 |         0.000 |       1 |  23379884.35B | 2.4x      | CPU-bound |


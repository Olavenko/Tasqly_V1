# Benchmark Report (MINGW)
_Phase: Phase0 — 2025-10-05_

**Commit**: local  
**Job**: manual  
**Host**: DYANKO  
**CPU**: 8 cores @ 3600 MHz  
**Runner**: C:\Users\Olavenko\Desktop\TasqlyQt\Tasqly_V1\build\mingw-benchmarks-release\TasqlyBenchmarksRunner.exe  
**Execution Time**: 2 min 3 sec  
**Generated On**: 04:43 PM — 2025-10-05

---

**Best Benchmark**: BM_SwitchTheme_NoOp (0.000 ms)

**Worst Benchmark**: BM_LogManager_Throughput/10000 (0.921 ms)

---

## Slow Benchmarks

No slow benchmarks found. Performance is stable for this run.

## Medium Variance Benchmarks

No medium-variance benchmarks found.

## Stable Benchmarks

| Benchmark                                               | Iterations | Real Time (ms) | CPU Time (ms) | Variance (ms) | Threads | Ops/sec       | Relative  | Type      |
|---------------------------------------------------------|------------|----------------|---------------|---------------|---------|---------------|-----------|-----------|
| BM_LogManager_Throughput/10000                          |        747 |          0.921 |         0.920 |         0.001 |       1 |       810.99K | 775011.6x | CPU-bound |
| BM_FeatureFlagsManager_BulkInsert                       |       2240 |          0.324 |         0.328 |        -0.004 |       1 |         6.91M | 272793.9x | CPU-bound |
| BM_AppSettingsManager_Load                              |       3446 |          0.238 |         0.240 |        -0.002 |       1 |        14.48M | 200214.6x | CPU-bound |
| BM_AppSettingsManager_SetThemeAndSave                   |        896 |          0.103 |         0.575 |        -0.472 |       1 |         8.67M | 86946.2x  | CPU-bound |
| BM_TaskRepo_AddBulk                                     |       7168 |          0.092 |         0.087 |         0.005 |       1 |        77.72M | 77603.0x  | CPU-bound |
| BM_LogManager_Throughput/1000                           |       7467 |          0.092 |         0.090 |         0.002 |       1 |        81.05M | 77513.0x  | CPU-bound |
| BM_LogManager_Throughput/100                            |      89600 |          0.009 |         0.009 |        -0.000 |       1 |         9.68B | 7791.0x   | CPU-bound |
| BM_NavigateTo_RetryValid                                |      89600 |          0.008 |         0.007 |         0.000 |       1 |        11.86B | 6356.3x   | CPU-bound |
| BM_SwitchTheme_Transition                               |     224000 |          0.006 |         0.003 |         0.003 |       1 |        35.98B | 5238.8x   | CPU-bound |
| BM_NavigateTo_ExecuteValid                              |     112000 |          0.005 |         0.005 |         0.000 |       1 |        23.99B | 3927.5x   | CPU-bound |
| BM_TaskRepo_Add_CopyMove                                |     224000 |          0.003 |         0.003 |        -0.000 |       1 |        72.15B | 2612.1x   | CPU-bound |
| BM_TaskRepo_Update_CopyMove                             |     448000 |          0.002 |         0.002 |         0.000 |       1 |       281.62B | 1338.5x   | CPU-bound |
| BM_TaskRepo_Remove                                      |     344615 |          0.002 |         0.002 |         0.000 |       1 |       170.60B | 1699.6x   | CPU-bound |
| BM_NavigationService_NavigateValid                      |     320000 |          0.002 |         0.002 |         0.000 |       1 |       144.42B | 1864.4x   | CPU-bound |
| BM_NavigationService_GoBack                             |     320000 |          0.002 |         0.002 |        -0.000 |       1 |       146.33B | 1840.0x   | CPU-bound |
| BM_NavigationService_RetryLast                          |     344615 |          0.002 |         0.002 |         0.000 |       1 |       157.37B | 1842.5x   | CPU-bound |
| BM_CreateTaskFlow/iterations:10000                      |      10000 |          0.002 |         0.002 |         0.000 |       1 |         5.30B | 1587.1x   | CPU-bound |
| BM_FeatureFlagsManager_Signal                           |    1000000 |          0.001 |         0.001 |         0.000 |       1 |      1937.10B | 434.4x    | CPU-bound |
| BM_TaskRepo_GetById                                     |    1120000 |          0.001 |         0.001 |         0.000 |       1 |      1736.60B | 542.7x    | CPU-bound |
| BM_TaskRepo_List                                        |     746667 |          0.001 |         0.001 |         0.000 |       1 |       766.50B | 819.6x    | CPU-bound |
| BM_NavigationService_NavigateInvalid                    |    1120000 |          0.001 |         0.001 |         0.000 |       1 |      1955.95B | 481.8x    | CPU-bound |
| BM_NavigateTo_ExecuteInvalid                            |     640000 |          0.001 |         0.001 |         0.000 |       1 |       590.26B | 912.3x    | CPU-bound |
| BM_SwitchTheme_NoOp                                     |  186666667 |          0.000 |         0.000 |         0.000 |       1 |  47025452.14B | 3.3x      | CPU-bound |
| BM_SaveUserSettings                                     |    2800000 |          0.000 |         0.000 |        -0.000 |       1 |     10705.20B | 220.1x    | CPU-bound |
| BM_SaveUserSettings_ForcedFlush                         |    2800000 |          0.000 |         0.000 |        -0.000 |       1 |     10726.39B | 219.6x    | CPU-bound |
| BM_LoadUserSettings                                     |    2800000 |          0.000 |         0.000 |         0.000 |       1 |     11029.01B | 213.6x    | CPU-bound |
| BM_ListTasks/10                                         |    3200000 |          0.000 |         0.000 |         0.000 |       1 |     14313.71B | 188.1x    | CPU-bound |
| BM_ListTasks/100                                        |    2635294 |          0.000 |         0.000 |         0.000 |       1 |     11821.46B | 187.6x    | CPU-bound |
| BM_ListTasks/1000                                       |    3200000 |          0.000 |         0.000 |        -0.000 |       1 |     12468.19B | 215.9x    | CPU-bound |
| BM_ThemeManager_Mode                                    |  560000000 |          0.000 |         0.000 |         0.000 |       1 | 471182162.39B | 1.0x      | CPU-bound |
| BM_ThemeManager_SwitchMode                              |    4977778 |          0.000 |         0.000 |         0.000 |       1 |     35892.79B | 116.7x    | CPU-bound |
| BM_ThemeManager_Palette                                 |   56000000 |          0.000 |         0.000 |        -0.000 |       1 |   3796667.63B | 12.4x     | CPU-bound |
| BM_ThemeManager_Fonts                                   |   64000000 |          0.000 |         0.000 |        -0.000 |       1 |   5214380.28B | 10.3x     | CPU-bound |
| BM_FeatureFlagsManager_Get                              |    8960000 |          0.000 |         0.000 |         0.000 |       1 |    108617.46B | 69.4x     | CPU-bound |
| BM_FeatureFlagsManager_SetToggle                        |    3200000 |          0.000 |         0.000 |         0.000 |       1 |     13239.60B | 203.4x    | CPU-bound |
| BM_NavigationService_CurrentRoute                       |   49777778 |          0.000 |         0.000 |         0.000 |       1 |   3505929.91B | 11.9x     | CPU-bound |
| BM_NavigateTo_RetryInvalid                              |    4072727 |          0.000 |         0.000 |         0.000 |       1 |     23545.42B | 145.5x    | CPU-bound |
| BM_LogMasking_NoContext/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |      4372.31B | 192.4x    | CPU-bound |
| BM_LogMasking_NoContext/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |      4389.52B | 191.7x    | CPU-bound |
| BM_LogMasking_NoContext/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      4382.00B | 192.0x    | CPU-bound |
| BM_LogMasking_NoContext/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |      4387.85B | 191.8x    | CPU-bound |
| BM_LogMasking_NoContext/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      4386.37B | 191.8x    | CPU-bound |
| BM_LogMasking_SmallContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |      4396.61B | 191.4x    | CPU-bound |
| BM_LogMasking_SmallContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      4393.37B | 191.5x    | CPU-bound |
| BM_LogMasking_SmallContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |      4380.12B | 192.1x    | CPU-bound |
| BM_LogMasking_SmallContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      4373.65B | 192.4x    | CPU-bound |
| BM_LogMasking_SmallContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |      4313.46B | 195.1x    | CPU-bound |
| BM_LogMasking_LargeContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |      4459.96B | 188.7x    | CPU-bound |
| BM_LogMasking_LargeContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      4462.22B | 188.6x    | CPU-bound |
| BM_LogMasking_LargeContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |      4420.30B | 190.3x    | CPU-bound |
| BM_LogMasking_LargeContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      4415.82B | 190.5x    | CPU-bound |
| BM_LogMasking_LargeContext/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |      4454.08B | 188.9x    | CPU-bound |
| BM_LogFacade_Info                                       |    8960000 |          0.000 |         0.000 |         0.000 |       1 |    122893.47B | 61.3x     | CPU-bound |
| BM_LogFacade_Warn                                       |    8960000 |          0.000 |         0.000 |         0.000 |       1 |    123262.59B | 61.2x     | CPU-bound |
| BM_LogFacade_Error                                      |    8960000 |          0.000 |         0.000 |         0.000 |       1 |    123376.66B | 61.1x     | CPU-bound |
| BM_LogContinuous_NullSink/128/iterations:1000000        |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      6303.68B | 133.5x    | CPU-bound |
| BM_LogContinuous_NullSink/1024/iterations:1000000       |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      6329.13B | 132.9x    | CPU-bound |
| BM_LogContinuous_NullSink/4096/iterations:1000000       |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      6288.22B | 133.8x    | CPU-bound |
| BM_LogContinuous_NullSink/16384/iterations:1000000      |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      6303.28B | 133.5x    | CPU-bound |
| BM_LogContinuous_FileOnly/128/iterations:1000000        |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      6360.86B | 132.3x    | CPU-bound |
| BM_LogContinuous_FileOnly/1024/iterations:1000000       |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      6364.72B | 132.2x    | CPU-bound |
| BM_LogContinuous_FileOnly/4096/iterations:1000000       |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      6264.20B | 134.3x    | CPU-bound |
| BM_LogContinuous_FileOnly/16384/iterations:1000000      |    1000000 |          0.000 |         0.000 |         0.000 |       1 |      6374.06B | 132.0x    | CPU-bound |
| BM_ErrorReporter_Report/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |     64900.51B | 13.0x     | CPU-bound |
| BM_ErrorReporter_Report/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |     66377.26B | 12.7x     | CPU-bound |
| BM_ErrorReporter_Report/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |     65541.54B | 12.8x     | CPU-bound |
| BM_ErrorReporter_Report/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |     65261.37B | 12.9x     | CPU-bound |
| BM_ErrorReporter_Report/iterations:1000000/repeats:5    |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |     65617.66B | 12.8x     | CPU-bound |
| BM_ErrorReporter_ReportRaw/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |     31258.60B | 26.9x     | CPU-bound |
| BM_ErrorReporter_ReportRaw/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |     31241.90B | 26.9x     | CPU-bound |
| BM_ErrorReporter_ReportRaw/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |     31400.23B | 26.8x     | CPU-bound |
| BM_ErrorReporter_ReportRaw/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |         0.000 |       1 |     31148.77B | 27.0x     | CPU-bound |
| BM_ErrorReporter_ReportRaw/iterations:1000000/repeats:5 |    1000000 |          0.000 |         0.000 |        -0.000 |       1 |     31373.43B | 26.8x     | CPU-bound |

All benchmarks are stable and fast.


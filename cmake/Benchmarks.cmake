# ---------------------------------------------------------------
# ⚡ Google Benchmark Integration (TasqlyBenchmarksRunner)
# ---------------------------------------------------------------
# 📌 Purpose : Download and integrate Google Benchmark
# 🧱 Layer   : Build System (Benchmarking Infrastructure)
# 👤 Author  : Mohamed Ali
# 🗓️ Updated : 2025-09-16
#
# 🧠 Description:
# - Downloads and configures Google Benchmark
# - Defines the benchmark runner target `TasqlyBenchmarksRunner`
# - Adds reporting rules for exporting benchmark results (JSON)
# ---------------------------------------------------------------

include(FetchContent)

# ---------------------------------------------------------------
# 📥 Google Benchmark FetchContent
# ---------------------------------------------------------------
FetchContent_Declare(
    googlebenchmark
    URL https://github.com/google/benchmark/archive/refs/tags/v1.8.3.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# Disable unused options to speed up build
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_TESTING     OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_WERROR      OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googlebenchmark)

# ---------------------------------------------------------------
# ⚡ Benchmark Runner Target
# ---------------------------------------------------------------
add_executable(TasqlyBenchmarksRunner

    # 📊 Integration Benchmarks (Integration)
    benchmarks/integration/Bench_CreateTaskFlow.cpp

    # 📊 Logging Benchmarks (Unit)
    benchmarks/unit/logging/Bench_ErrorReporter.cpp
    benchmarks/unit/logging/Bench_LogContinuous.cpp
    benchmarks/unit/logging/Bench_LogFacade.cpp
    benchmarks/unit/logging/Bench_LogManager_Throughput.cpp
    benchmarks/unit/logging/Bench_LogMasking.cpp

    # 📊 Navigation Benchmarks (Unit)
    benchmarks/unit/navigation/Bench_NavigateTo.cpp
    benchmarks/unit/navigation/Bench_NavigationService.cpp

    # 📊 Persistence Benchmarks (Unit)
    benchmarks/unit/persistence/Bench_InMemoryTaskRepo_CRUD.cpp

    # 📊 Settings Benchmarks (Unit)
    benchmarks/unit/settings/Bench_AppSettingsManager.cpp
    benchmarks/unit/settings/Bench_FeatureFlagsManager.cpp

    # 📊 Theme Benchmarks (Unit)
    benchmarks/unit/theme/Bench_ThemeManager.cpp

    # 📊 Usecases Benchmarks (Unit)
    benchmarks/unit/usecases/Bench_ListTasks.cpp
    benchmarks/unit/usecases/Bench_LoadUserSettings.cpp
    benchmarks/unit/usecases/Bench_SaveUserSettings.cpp
    benchmarks/unit/usecases/Bench_SwitchTheme.cpp

    # 📊 Main entry
    benchmarks/bench_main.cpp
)

set_target_properties(TasqlyBenchmarksRunner PROPERTIES AUTOMOC ON)

target_include_directories(TasqlyBenchmarksRunner PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(TasqlyBenchmarksRunner
    PRIVATE
        tasqly_core
        benchmark::benchmark
)

# ---------------------------------------------------------------
# 🛡️ Quality Gates
# ---------------------------------------------------------------
if (COMMAND tasqly_apply_warnings)
    tasqly_apply_warnings(TasqlyBenchmarksRunner)
endif()
if (COMMAND tasqly_apply_debug_sanitizers)
    tasqly_apply_debug_sanitizers(TasqlyBenchmarksRunner)
endif()

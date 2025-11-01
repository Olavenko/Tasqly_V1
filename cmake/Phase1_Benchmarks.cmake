# ---------------------------------------------------------------
# ⚡ Google Benchmark Integration (TasqlyBenchmarksRunner)
# ---------------------------------------------------------------
# 📌 Purpose : Download and integrate Google Benchmark
# 🧱 Layer   : Build System (Benchmarking Infrastructure)
# 👤 Author  : Mohamed Ali
# 🗓️ Created : 2025-08-28
# 🗓️ Updated : 2025-09-10
#
# 🧠 Description:
# - Downloads and configures Google Benchmark
# - Defines the benchmark runner target `TasqlyBenchmarksRunner`
# - Adds reporting rules for exporting benchmark results (JSON)
# ---------------------------------------------------------------
# ---------------------------------------------------------------
# ⚡ Benchmark Runner Target
# ---------------------------------------------------------------
add_executable(TasqlyBenchmarksRunner

    #========================================================
    # [Phase1]
    #========================================================
    # 📊 Domain Benchmarks (Unit)
    benchmarks/unit/domain/P1_S1_Bench_DomainValidation.cpp
    benchmarks/unit/domain/P1_S1_Bench_TaskEntity.cpp
    benchmarks/unit/domain/P1_S1_Bench_TaskMapper.cpp

    # 📊 Main entry
    benchmarks/bench_main.cpp
)

#=====================================================================

set_target_properties(TasqlyBenchmarksRunner PROPERTIES AUTOMOC ON)

target_include_directories(TasqlyBenchmarksRunner PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(TasqlyBenchmarksRunner
    PRIVATE
        tasqly_core
        tasqly_fakes
        benchmark
        Qt6::Core
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

message(STATUS "[Benchmarks] TasqlyBenchmarksRunner linked successfully with Google Benchmark + Qt")

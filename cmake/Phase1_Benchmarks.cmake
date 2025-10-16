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
# 🧩 Tasqly Diagnostic Build Info (Cross-Platform + Timing)
#=====================================================================
message(STATUS "====================================================")
message(STATUS " Tasqly Diagnostic Build Info")
message(STATUS "====================================================")

# 🧱 General Environment Info
message(STATUS "CMake Version       : ${CMAKE_VERSION}")
message(STATUS "Project Name        : ${PROJECT_NAME}")
message(STATUS "Project Version     : ${PROJECT_VERSION}")
message(STATUS "Source Directory    : ${CMAKE_SOURCE_DIR}")
message(STATUS "Build Directory     : ${CMAKE_BINARY_DIR}")
message(STATUS "Generator           : ${CMAKE_GENERATOR}")
message(STATUS "Host OS             : ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "System Name         : ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processor           : ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message(STATUS "Build Type          : ${CMAKE_BUILD_TYPE}")

# ⚙️ Compiler Configuration
message(STATUS "C++ Compiler ID     : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Compiler Path   : ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Flags (common)  : ${CMAKE_CXX_FLAGS}")
message(STATUS "C++ Flags (debug)   : ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "C++ Flags (release) : ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "Linker Flags        : ${CMAKE_EXE_LINKER_FLAGS}")

# 🎨 Qt Runtime Info (if available)
if (TARGET Qt6::Core)
    get_target_property(QtCore_location Qt6::Core LOCATION)
    if (QtCore_location)
        message(STATUS "Qt Version          : ${Qt6_VERSION}")
        message(STATUS "Qt Core Library     : ${QtCore_location}")
    else()
        message(STATUS "Qt Detected         : Yes (location unknown at configure-time)")
    endif()
else()
    message(STATUS "Qt Detected         : Not linked to Qt (non-Qt target)")
endif()

# 🧩 Current Tasqly Phase Configuration
message(STATUS "Current Phase (Tests)     : ${TASQLY_TEST_PHASE}")
message(STATUS "Current Phase (Benchmarks): ${TASQLY_BENCHMARK_PHASE}")

# 🧰 Enabled Features / Flags
message(STATUS "Feature Flags Enabled:")
message(STATUS " - Build Tests      : ${TASQLY_BUILD_TESTS}")
message(STATUS " - Build Benchmarks : ${TASQLY_BUILD_BENCHMARKS}")
message(STATUS " - Build Reports    : ${TASQLY_BUILD_REPORTS}")

# 🔍 Runtime STL / ABI (for GCC/Clang awareness)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|MinGW|AppleClang")
    include(CheckCXXSourceCompiles)
    check_cxx_source_compiles("
    #include <iostream>
    int main() {
    #ifdef _GLIBCXX_USE_CXX11_ABI
    # if _GLIBCXX_USE_CXX11_ABI == 1
    #error ABI_IS_1
    # else
    #error ABI_IS_0
    # endif
    #else
    #error ABI_UNDEFINED
    #endif
    return 0;
    }
    " TASQLY_ABI_DETECTED)

    if (TASQLY_ABI_DETECTED)
        message(STATUS "Runtime STL         : libstdc++ (ABI=1)")
    else()
        message(STATUS "Runtime STL         : libstdc++ (ABI unknown)")
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Runtime STL         : MSVC STL (ABI not applicable)")
else()
    message(STATUS "Runtime STL         : Unknown compiler/runtime")
endif()

# 📦 External Dependencies (if fetched via FetchContent)
if (DEFINED googlebenchmark_SOURCE_DIR)
    message(STATUS "Dependency: Google Benchmark → ${googlebenchmark_SOURCE_DIR}")
endif()
if (DEFINED googletest_SOURCE_DIR)
    message(STATUS "Dependency: Google Test      → ${googletest_SOURCE_DIR}")
endif()

# 🕒 Timing (start date + elapsed)
string(TIMESTAMP TASQLY_CONFIGURE_START_DATE "%m/%d/%Y")
string(TIMESTAMP TASQLY_CONFIGURE_START_TIME "%H:%M")
message(STATUS "Configure Start Date: ${TASQLY_CONFIGURE_START_DATE} - Time: ${TASQLY_CONFIGURE_START_TIME}")

string(TIMESTAMP TASQLY_START_EPOCH "%s")
string(TIMESTAMP TASQLY_END_EPOCH "%s")
math(EXPR TASQLY_ELAPSED "${TASQLY_END_EPOCH} - ${TASQLY_START_EPOCH}")
message(STATUS "Elapsed Time (approx): ${TASQLY_ELAPSED} seconds since configure start")

message(STATUS "====================================================")

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

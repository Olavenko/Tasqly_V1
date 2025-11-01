# ---------------------------------------------------------------
# 🧩 External Dependencies (GoogleTest + Google Benchmark)
# ---------------------------------------------------------------
# 📌 Purpose : Fetch and cache GoogleTest / Google Benchmark
# 📦 Cache Location : <project_root>/externals/_deps
# ---------------------------------------------------------------

include(FetchContent)

# ---------------------------------------------------------------
# 🌐 Use persistent local folder instead of build/_deps
# ---------------------------------------------------------------
# This ensures dependencies are reused even if build/ is deleted.
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/externals/_deps")
file(MAKE_DIRECTORY "${FETCHCONTENT_BASE_DIR}")

message(STATUS "====================================================")
message(STATUS " Tasqly External Dependencies Loader (Persistent Cache)")
message(STATUS " Base Path : ${FETCHCONTENT_BASE_DIR}")
message(STATUS "====================================================")

# ---------------------------------------------------------------
# 🧪 GoogleTest
# ---------------------------------------------------------------
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.0.zip
)

FetchContent_MakeAvailable(googletest)

if(TARGET gtest)
    message(STATUS "[GoogleTest] ✅ Ready at ${googletest_SOURCE_DIR}")
else()
    message(WARNING "[GoogleTest] ⚠️ Target 'gtest' not found after fetch!")
endif()

# ---------------------------------------------------------------
# 📊 Google Benchmark
# ---------------------------------------------------------------
FetchContent_Declare(
    googlebenchmark
    URL https://github.com/google/benchmark/archive/refs/tags/v1.9.0.zip
)

# Disable optional features to reduce size
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)
set(BENCHMARK_USE_BUNDLED_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googlebenchmark)

if(TARGET benchmark)
    message(STATUS "[Google Benchmark] ✅ Ready at ${googlebenchmark_SOURCE_DIR}")
else()
    message(WARNING "[Google Benchmark] ⚠️ Target 'benchmark' not found after fetch!")
endif()

# ---------------------------------------------------------------
# ✅ Summary
# ---------------------------------------------------------------
message(STATUS "----------------------------------------------------")
message(STATUS " External Dependencies Summary")
message(STATUS "----------------------------------------------------")
message(STATUS " - GoogleTest   : ${googletest_SOURCE_DIR}")
message(STATUS " - Benchmark    : ${googlebenchmark_SOURCE_DIR}")
message(STATUS "----------------------------------------------------")

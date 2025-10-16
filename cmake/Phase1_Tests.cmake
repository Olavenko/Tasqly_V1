# ---------------------------------------------------------------
# 🧪 GoogleTest Integration — TasqlyTestsRunner
# ---------------------------------------------------------------
# 📌 Purpose : Integrate GoogleTest and define the Tasqly test target
# 🧱 Layer   : Build System (Testing Infrastructure)
# 👤 Author  : Mohamed Ali
# 🗓️ Updated : 2025-09-18
#
# 🧠 Description:
# - Downloads and configures GoogleTest (pinned version)
# - Defines the test runner target `TasqlyTestsRunner`
# - Links Qt + tasqly_core
# - Auto-discovers and registers tests with CTest
# ---------------------------------------------------------------

include(CTest)
enable_testing()

# ---------------------------------------------------------------
# 📥 GoogleTest FetchContent
# ---------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE) # Prevent GTest from being installed
FetchContent_MakeAvailable(googletest)

# ---------------------------------------------------------------
# 🎨 Required Qt Components for Tests
# ---------------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    Concurrent
    Test
)

# ---------------------------------------------------------------
# 🏗️ Test Runner Target
# ---------------------------------------------------------------
add_executable(TasqlyTestsRunner

    #=========================================================
    # Phase[1]
    # =========================================================
    # 🛠️ integration Tests Domain — mappers [Phase1][Slice1]
    tests/integration/domain/mappers/test_P1_TaskRepositoryIntegration.cpp
    tests/integration/domain/mappers/test_P1_TaskMapperRepository_RoundTrip.cpp

    # 🛠️ Unit Tests Domain — entities [Phase1][Slice1]
    tests/unit/domain/entities/test_P1_DomainValidation.cpp
    tests/unit/domain/entities/test_P1_TaskPriority.cpp
    tests/unit/domain/entities/test_P1_TaskStatus.cpp

    # 🛠️ Unit Tests Domain — errors [Phase1][Slice1]
    tests/unit/domain/errors/test_P1_DomainError.cpp
    tests/unit/domain/errors/test_P1_DomainResult.cpp

    # 🛠️ Unit Tests Domain — mappers [Phase1][Slice1]
    tests/unit/domain/mappers/test_P1_TaskMapper.cpp

    # 🧪 Main Test Files
    tests/common/RuntimeDiagnostic.h
    tests/test_main.cpp
    tests/test_result.cpp
    tests/test_result_void.cpp
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

# ---------------------------------------------------------------
# 🔗 Test Runner Dependencies
# ---------------------------------------------------------------
target_link_libraries(TasqlyTestsRunner
    PRIVATE
        tasqly_core
        tasqly_fakes
        GTest::gtest
        GTest::gtest_main
        Qt6::Concurrent
        Qt6::Test
)

# ---------------------------------------------------------------
# 📂 Include Directories
# ---------------------------------------------------------------
target_include_directories(TasqlyTestsRunner PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# ---------------------------------------------------------------
# ⚙️ Target Properties
# ---------------------------------------------------------------
set_target_properties(TasqlyTestsRunner PROPERTIES AUTOMOC ON)
target_compile_definitions(TasqlyTestsRunner PRIVATE
    PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

# ---------------------------------------------------------------
# 🛡️ Quality Gates
# ---------------------------------------------------------------
if (COMMAND tasqly_apply_warnings)
    tasqly_apply_warnings(TasqlyTestsRunner)
endif()
if (COMMAND tasqly_apply_debug_sanitizers)
    tasqly_apply_debug_sanitizers(TasqlyTestsRunner)
endif()

# ---------------------------------------------------------------
# 🔍 GoogleTest Integration with CTest
# ---------------------------------------------------------------
include(GoogleTest)
gtest_discover_tests(TasqlyTestsRunner
    TEST_PREFIX TasqlyTests.
    WORKING_DIRECTORY $<TARGET_FILE_DIR:TasqlyTestsRunner>
    DISCOVERY_MODE PRE_TEST
    PROPERTIES ENVIRONMENT
        "QT_QPA_PLATFORM=offscreen;QSG_RHI_BACKEND=software"
)

message(STATUS "[Tests] TasqlyTestsRunner enabled")

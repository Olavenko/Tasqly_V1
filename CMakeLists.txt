#
# 🧱 File: CMakeLists.txt
# -----------------------
# 📌 Purpose   : Root CMake configuration for Tasqly application.
# 🧱 Layer     : Build System (CMake)
# 👤 Author    : Mohamed Ali
# 🗓️ Created   : 2025-09-01
# 🗓️ Updated   : 2025-09-16
#
# 🧠 Description:
# Defines the minimum required CMake version, project metadata, build
# type policies, compiler settings, and integrates Qt Quick for building
# the main Tasqly application with QML module support. Also integrates
# GoogleTest and Google Benchmark. Warnings & sanitizers are applied
# per-target and per-compiler.
#

# ---------------------------------------------------------------
# 🏗️ CMake Minimum Version
# ---------------------------------------------------------------
# Use a version range to ensure compatibility across newer releases.
# This allows building on newer CMake versions while still enforcing a baseline.
cmake_minimum_required(VERSION 3.21...3.29)

# ---------------------------------------------------------------
# 🏷️ Project Metadata
# ---------------------------------------------------------------
# Define project name, version, and supported languages.
# C++ is enforced as the only language since C is not required.
project(Tasqly_V1 VERSION 0.1 LANGUAGES CXX)

# ---------------------------------------------------------------
# 🧪 Build Diagnostics
# ---------------------------------------------------------------
# Print useful environment details for debugging CMake configuration.
# These messages appear in the CMake configure step.
message(STATUS "CMAKE_C_COMPILER   = ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_PREFIX_PATH  = ${CMAKE_PREFIX_PATH}")

# ---------------------------------------------------------------
# ➕ C++ Standard Configuration
# ---------------------------------------------------------------
# Enforce modern C++ standard across the entire project.
# MSVC fix ensures __cplusplus macro is correctly defined for feature checks.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Avoid compiler-specific extensions (e.g., GNU/C99)

if (MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

# ---------------------------------------------------------------
# 🏗️ Build Types Policy
# ---------------------------------------------------------------
# Define available build types:
#   - Debug         : With debug symbols, no optimizations.
#   - Release       : Optimized build without debug symbols.
#   - RelWithDebInfo: Optimized build with debug symbols.
#
# For single-config generators (e.g., Makefiles, Ninja),
# default to Debug if no build type is explicitly set.
# For multi-config generators (e.g., MSVC, Ninja Multi-Config),
# CMAKE_CONFIGURATION_TYPES is respected automatically.
# ---------------------------------------------------------------
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

# ---------------------------------------------------------------
# 🛡️ Target-Scoped Warnings
# ---------------------------------------------------------------
# Apply strict compiler warnings to a given target.
# MSVC uses /W4 and standard compliance flags.
# GCC/Clang use -Wall -Wextra -Wpedantic.
# ---------------------------------------------------------------
function(tasqly_apply_warnings tgt)
    if (MSVC)
        target_compile_options(${tgt} PRIVATE
            /W4 /permissive- /EHsc /utf-8
        )
    else()
        target_compile_options(${tgt} PRIVATE
            -Wall -Wextra -Wpedantic
        )
    endif()
endfunction()

# ---------------------------------------------------------------
# 🧪 Debug Sanitizers
# ---------------------------------------------------------------
# Apply AddressSanitizer (ASan) and UndefinedBehaviorSanitizer (UBSan)
# for Unix-like systems (GCC/Clang) when in Debug mode.
# This ensures memory safety issues are caught early during development.
# ---------------------------------------------------------------
function(tasqly_apply_debug_sanitizers tgt)
    if (UNIX AND (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang") AND CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Enabling ASan/UBSan for target: ${tgt}")
        target_compile_options(${tgt} PRIVATE -fsanitize=address,undefined)
        target_link_options(${tgt} PRIVATE -fsanitize=address,undefined)
    endif()
endfunction()

# In your top-level CMakeLists.txt, BEFORE any find_package(Qt6 ...) call
set(QT_FORCE_CMP0156_TO_VALUE NEW CACHE STRING "Force Qt to use NEW behavior for CMP0156")

# ---------------------------------------------------------------
# 🎨 Qt Quick Integration
# ---------------------------------------------------------------
# Find required Qt6 components. "Test" is included here to support
# QML/C++ unit testing targets. Version 6.9 is enforced for compatibility.
find_package(Qt6 COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    Concurrent
    Test
    REQUIRED
)

# Apply standard Qt project settings:
# - Enables AUTOMOC, AUTORCC, AUTOUIC
# - Sets default Qt compile/link flags
# - Ensures Qt6 minimum version (6.9 here)
qt_standard_project_setup(REQUIRES 6.9)

# ---------------------------------------------------------------
# ⚙️ Tasqly Build Options
# ---------------------------------------------------------------
option(TASQLY_BUILD_TESTS "Build GoogleTest unit tests" ON)
option(TASQLY_BUILD_BENCHMARKS "Build Google Benchmark targets" OFF)
option(TASQLY_BUILD_REPORTS "Build reports (coverage, XML, benchmarks)" OFF)

# ---------------------------------------------------------------
# ⚙️ Phase Control (for selective Tests / Benchmarks builds)
# ---------------------------------------------------------------
set(TASQLY_TEST_PHASE "Phase1" CACHE STRING "Select which test phase to build")
set(TASQLY_BENCHMARK_PHASE "Phase1" CACHE STRING "Select which benchmark phase to build")

# ---------------------------------------------------------------
# 📦 External Dependencies
# ---------------------------------------------------------------
include(cmake/ExternalDeps.cmake)

# ---------------------------------------------------------------
# 📦 Core Library Target
# ---------------------------------------------------------------
include(cmake/Core.cmake)

# ---------------------------------------------------------------
# 🏗️ Application Target
# ---------------------------------------------------------------
include(cmake/App.cmake)

# ---------------------------------------------------------------
# 🧪 Tests Target (GoogleTest Integration)
# ---------------------------------------------------------------
if(TASQLY_BUILD_TESTS)
    if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/${TASQLY_TEST_PHASE}_Tests.cmake")
        message(STATUS "[App] Loading tests for phase: ${TASQLY_TEST_PHASE}")
        include(cmake/${TASQLY_TEST_PHASE}_Tests.cmake)
else()
        message(WARNING "Phase file not found for: ${TASQLY_TEST_PHASE}, falling back to default Tests.cmake")
        include(cmake/Tests.cmake)
    endif()
endif()

# ---------------------------------------------------------------
# ⚡ Benchmarks Target (Google Benchmark Integration)
# ---------------------------------------------------------------
if(TASQLY_BUILD_BENCHMARKS)
    if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/${TASQLY_BENCHMARK_PHASE}_Benchmarks.cmake")
        message(STATUS "Loading benchmarks for phase: ${TASQLY_BENCHMARK_PHASE}")
        include(cmake/${TASQLY_BENCHMARK_PHASE}_Benchmarks.cmake)
    else()
        message(WARNING "Phase file not found for: ${TASQLY_BENCHMARK_PHASE}, falling back to default Benchmarks.cmake")
        include(cmake/Benchmarks.cmake)
    endif()
endif()

# ---------------------------------------------------------------
# 📊 Reports & Options (Coverage, Test Reports, Benchmarks Reports)
# ---------------------------------------------------------------
if(TASQLY_BUILD_REPORTS)
    include(cmake/Reports.cmake)
endif()

# ---------------------------------------------------------------
# 🧩 PostgreSQL Integration (Cross-Platform Smart Detection)
# ---------------------------------------------------------------
# Purpose:
#   - Detect PostgreSQL client library and headers (libpq).
#   - Define TASQLY_PG_AVAILABLE for tests and other modules.
#   - Avoid breaking build on CI Windows when PostgreSQL is missing.
# ---------------------------------------------------------------

# Try to find PostgreSQL quietly (doesn't break build if missing)
find_package(PostgreSQL QUIET)

if (PostgreSQL_FOUND)
    set(TASQLY_PG_AVAILABLE TRUE)
    include_directories(${PostgreSQL_INCLUDE_DIRS})
    link_directories(${PostgreSQL_LIBRARY_DIRS})
    message(STATUS "[PostgreSQL] Found PostgreSQL version ${PostgreSQL_VERSION_STRING}")
    message(STATUS "[PostgreSQL] Include dir: ${PostgreSQL_INCLUDE_DIRS}")
    message(STATUS "[PostgreSQL] Library dir: ${PostgreSQL_LIBRARY_DIRS}")

elseif (WIN32)
    # Manual fallback for local Windows dev environment
    set(_pg_root "C:/Program Files/PostgreSQL/17")
    if (EXISTS "${_pg_root}/include/libpq-fe.h")
        set(TASQLY_PG_AVAILABLE TRUE)
        include_directories("${_pg_root}/include")
        link_directories("${_pg_root}/lib")
        message(STATUS "[PostgreSQL] Fallback applied: using ${_pg_root}")
    else()
        set(TASQLY_PG_AVAILABLE FALSE)
        message(WARNING "[PostgreSQL] Not found — skipping DB integration on Windows.")
    endif()

else()
    set(TASQLY_PG_AVAILABLE FALSE)
    message(WARNING "[PostgreSQL] Not found on this system — DB integration disabled.")
endif()

#=====================================================================
# 🧩 Unified Diagnostics Summary
#=====================================================================
include(cmake/DiagnosticsSummary.cmake)

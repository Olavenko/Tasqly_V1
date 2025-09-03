#
# 🧱 File: CMakeLists.txt
# -----------------------
# 📌 Purpose   : Root CMake configuration for Tasqly application.
# 🧱 Layer     : Build System (CMake)
# 👤 Author    : Mohamed Ali
# 🗓️ Updated   : 2025-09-01
#
# 🧠 Description:
# Defines the minimum required CMake version, project metadata, build
# type policies, compiler settings, and integrates Qt Quick for building
# the main Tasqly application with QML module support. Also integrates
# GoogleTest for unit testing. Warnings & sanitizers are applied per-target
# and per-compiler to avoid leaking GCC/Clang flags into MSVC or third-party deps.
#

cmake_minimum_required(VERSION 3.21)

# 🏷️ Project metadata
project(Tasqly_V1 VERSION 0.1 LANGUAGES CXX)

# 🧪 Diagnostics (build environment)
message(STATUS "CMAKE_C_COMPILER   = ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_PREFIX_PATH  = ${CMAKE_PREFIX_PATH}")

# ➕ Enforce C++20 as the project standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Avoid compiler-specific extensions

# ---------------------------------------------------------------
# 🏗️ Build Types Policy
# ---------------------------------------------------------------
# Define and enforce standard build configurations:
# - Debug        : For local development (extra warnings, sanitizers).
# - Release      : For optimized production builds.
# - RelWithDebInfo : For CI/CD builds with debug symbols.
# ---------------------------------------------------------------

# Restrict to the allowed build types (for multi-config generators this is informational)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)

# Default to Debug if not explicitly set (single-config generators)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

# ---------------------------------------------------------------
# 🛡️ Target-scoped warnings & sanitizers (avoid leaking to external deps)
# ---------------------------------------------------------------
# Apply warnings per compiler and only to our targets.
function(tasqly_apply_warnings tgt)
    if (MSVC)
        target_compile_options(${tgt} PRIVATE
            /W4           # high warning level
            /permissive-  # stricter standard conformance
            /EHsc         # C++ exceptions semantics
            /utf-8        # source encoding
        )
    else()
        target_compile_options(${tgt} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

# Apply sanitizers only on Linux with GCC/Clang, and only for Debug
function(tasqly_apply_debug_sanitizers tgt)
    if (UNIX AND (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang") AND CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Enabling ASan/UBSan for target: ${tgt}")
        target_compile_options(${tgt} PRIVATE -fsanitize=address,undefined)
        target_link_options(${tgt} PRIVATE -fsanitize=address,undefined)
    endif()
endfunction()

# ---------------------------------------------------------------
# 🎨 Qt Quick Integration
# ---------------------------------------------------------------
# Locate Qt6 with required Quick module and initialize defaults
find_package(Qt6 COMPONENTS Core Gui Qml Quick Concurrent Test REQUIRED)

# Apply standard Qt project setup (requires Qt 6.9+)
qt_standard_project_setup(REQUIRES 6.9)

# ---------------------------------------------------------------
# 🏗️ Application Target
# ---------------------------------------------------------------
# Main executable for Tasqly application
qt_add_executable(appTasqly_V1
    main.cpp
)

# Define QML module for the application (Tasqly_V1 namespace)
qt_add_qml_module(appTasqly_V1
    URI Tasqly_V1
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES src/domain/core/Error.h
        SOURCES src/domain/core/Result.h
        SOURCES src/app/logging/ILogManager.h
        SOURCES src/infra/logging/LogManager.cpp src/infra/logging/LogManager.h
        SOURCES src/app/logging/ErrorReporter.cpp src/app/logging/ErrorReporter.h
        SOURCES src/app/errors/AppErrors.cpp src/app/errors/AppErrors.h
        QML_FILES src/ui/qml/components/SmartFallback.qml src/ui/qml/components/Toast.qml
)

# ---------------------------------------------------------------
# ⚙️ Target Properties
# ---------------------------------------------------------------
# Bundle identifiers and executable flags for cross-platform support
set_target_properties(appTasqly_V1 PROPERTIES
#   MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appTasqly_V1
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Link application with Qt Quick
target_link_libraries(appTasqly_V1
    PRIVATE Qt6::Quick
)

# Apply warning/sanitizer policies to our app target only
tasqly_apply_warnings(appTasqly_V1)
tasqly_apply_debug_sanitizers(appTasqly_V1)

# ---------------------------------------------------------------
# 📦 Installation Rules
# ---------------------------------------------------------------
# Define how the application is packaged and installed
include(GNUInstallDirs)
install(TARGETS appTasqly_V1
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ---------------------------------------------------------------
# 🧪 GoogleTest Integration (TasqlyTests)
# ---------------------------------------------------------------
# 📌 Purpose : Integrate GoogleTest and define the Tasqly test target
# 🧱 Layer   : Build System (Testing Infrastructure)
# 👤 Author  : Mohamed Ali
# 🗓️ Updated : 2025-09-03
#
# 🧠 Description:
# Downloads and configures GoogleTest for unit/integration tests.
# Defines a dedicated target `TasqlyTests` and wires Qt + project sources.
# ---------------------------------------------------------------

include(CTest)
enable_testing()

include(FetchContent)

# Fetch GoogleTest (official release)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# Disable GTest installation (CI convenience)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

# Make GoogleTest available
FetchContent_MakeAvailable(googletest)

# Ensure required Qt components are available for the tests
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

# Define test target and list test sources
add_executable(TasqlyTests
    tests/test_result.cpp
    tests/integration/logging/test_qml_error_to_toast_and_log.cpp
    tests/unit/logging/test_log_level_parsing.cpp
    tests/unit/logging/test_masking.cpp
    tests/unit/logging/test_rotation_policy.cpp
    tests/unit/logging/test_concurrency.cpp
)

# Bring in implementation files needed by tests (to resolve symbols)
target_sources(TasqlyTests PRIVATE
    src/infra/logging/LogManager.cpp
    src/app/logging/ErrorReporter.cpp
    # Header-only with Q_GADGET (needs AUTOMOC)
    src/domain/core/Error.h
)

# Make sure AUTOMOC processes Error.h (Q_GADGET → staticMetaObject)
set_source_files_properties(src/domain/core/Error.h PROPERTIES SKIP_AUTOMOC OFF)

# Allow tests to include from the project source tree
target_include_directories(TasqlyTests PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# Enable AUTOMOC for signals/slots and Q_GADGET meta-objects
set_target_properties(TasqlyTests PROPERTIES AUTOMOC ON)

# Provide project root path to tests (used for QML imports, etc.)
target_compile_definitions(TasqlyTests PRIVATE PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

# Link GoogleTest and Qt libraries required by the tests
target_link_libraries(TasqlyTests
    PRIVATE
        GTest::gtest_main
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::Concurrent
		Qt6::Test
)

# Apply our compiler warnings/sanitizers to tests as well (if helpers exist)
if (COMMAND tasqly_apply_warnings)
    tasqly_apply_warnings(TasqlyTests)
endif()
if (COMMAND tasqly_apply_debug_sanitizers)
    tasqly_apply_debug_sanitizers(TasqlyTests)
endif()

# ---- GoogleTest discovery (no timeouts at all)
include(GoogleTest)
gtest_discover_tests(TasqlyTests
    TEST_PREFIX TasqlyTests.
    WORKING_DIRECTORY $<TARGET_FILE_DIR:TasqlyTests>
    DISCOVERY_MODE PRE_TEST
    # No DISCOVERY_TIMEOUT and no PROPERTIES TIMEOUT → unlimited
    PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QSG_RHI_BACKEND=software"
)

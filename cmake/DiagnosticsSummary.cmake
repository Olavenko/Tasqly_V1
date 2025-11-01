# =====================================================================
# 🧠 Tasqly Diagnostics Summary
# =====================================================================
# 📌 Purpose : Centralized diagnostic output for Tasqly builds
# 👤 Author  : Mohamed Ali
# 🗓️ Created : 2025-10-21
# 🔖 Version : 2.0 (Dynamic ABI Detection + Cross-Compiler Health Checks)
# =====================================================================

message(STATUS "====================================================")
message(STATUS " Tasqly Diagnostic Summary")
message(STATUS "----------------------------------------------------")

# =====================================================================
# 🧩 Dependencies Summary
# =====================================================================
message(STATUS "||[Dependencies]||")
message(STATUS "----------------------------------------------------")
if (DEFINED TASQLY_DEP_SUMMARY)
    message(STATUS "${TASQLY_DEP_SUMMARY}")
else()
    message(STATUS " - No external dependencies recorded.")
endif()

# =====================================================================
# ⚙️ Build Environment
# =====================================================================
message(STATUS "----------------------------------------------------")
message(STATUS "||[Build Environment]||")
message(STATUS "----------------------------------------------------")
message(STATUS " CMake Version       : ${CMAKE_VERSION}")
message(STATUS " Project Name        : ${PROJECT_NAME}")
message(STATUS " Project Version     : ${PROJECT_VERSION}")
message(STATUS " Build Type          : ${CMAKE_BUILD_TYPE}")
message(STATUS " Generator           : ${CMAKE_GENERATOR}")
message(STATUS " Source Directory    : ${CMAKE_SOURCE_DIR}")
message(STATUS " Build Directory     : ${CMAKE_BINARY_DIR}")

# =====================================================================
# 🧰 Compiler Info
# =====================================================================
message(STATUS "----------------------------------------------------")
message(STATUS "||[Compiler]||")
message(STATUS "----------------------------------------------------")
message(STATUS " C++ Compiler ID     : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " C++ Compiler Path   : ${CMAKE_CXX_COMPILER}")
message(STATUS " C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " Flags (Debug)       : ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS " Flags (Release)     : ${CMAKE_CXX_FLAGS_RELEASE}")

# =====================================================================
# 🧬 Dynamic ABI / API Mode Detection
# =====================================================================
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    include(CheckCXXSymbolExists)
    check_cxx_symbol_exists(_GLIBCXX_USE_CXX11_ABI "string" HAS_GLIBCXX11_ABI)
    if (HAS_GLIBCXX11_ABI)
        set(TASQLY_ABI_MODE "_GLIBCXX_USE_CXX11_ABI = 1 (C++11 string ABI active)")
    else()
        set(TASQLY_ABI_MODE "_GLIBCXX_USE_CXX11_ABI = 0 (Legacy string ABI)")
    endif()
    message(STATUS " ABI / API Mode      : ${TASQLY_ABI_MODE}")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(TASQLY_ABI_MODE "MSVC STL (Single unified ABI)")
    message(STATUS " ABI / API Mode      : ${TASQLY_ABI_MODE}")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(TASQLY_ABI_MODE "libc++ (LLVM / Apple ABI model)")
    message(STATUS " ABI / API Mode      : ${TASQLY_ABI_MODE}")
elseif (UNIX)
    set(TASQLY_ABI_MODE "GCC/Linux default ABI")
    message(STATUS " ABI / API Mode      : ${TASQLY_ABI_MODE}")
else()
    set(TASQLY_ABI_MODE "Unknown / default toolchain behavior")
    message(STATUS " ABI / API Mode      : ${TASQLY_ABI_MODE}")
endif()

# =====================================================================
# 🎨 Qt Integration
# =====================================================================
if (TARGET Qt6::Core)
message(STATUS "----------------------------------------------------")
    message(STATUS "||[Qt Integration]||")
message(STATUS "----------------------------------------------------")
    message(STATUS " Qt Version          : ${Qt6_VERSION}")
    get_target_property(QtCore_location Qt6::Core LOCATION)
    if (QtCore_location)
        message(STATUS " Qt Core Library     : ${QtCore_location}")
    endif()
endif()

# =====================================================================
# 🧩 Cross-Compiler Health Checks
# =====================================================================
message(STATUS "----------------------------------------------------")
message(STATUS "||[Cross-Compiler Health Checks]||")
message(STATUS "----------------------------------------------------")

# 🔍 GoogleTest Toolchain Check
if (EXISTS "${FETCHCONTENT_BASE_DIR}/googletest-src/CMakeCache.txt")
    file(STRINGS "${FETCHCONTENT_BASE_DIR}/googletest-src/CMakeCache.txt"
         GTEST_COMPILER_LINE REGEX "CMAKE_CXX_COMPILER_ID:STRING=.*")
    string(REGEX REPLACE ".*=" "" GTEST_COMPILER_ID "${GTEST_COMPILER_LINE}")
    if (NOT GTEST_COMPILER_ID STREQUAL CMAKE_CXX_COMPILER_ID)
        message(WARNING " GoogleTest built with ${GTEST_COMPILER_ID} while project uses ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()

# 🔍 ABI Verification
if (DEFINED TASQLY_ABI_MODE)
    message(STATUS " ABI Mode Verified   : ${TASQLY_ABI_MODE}")
else()
    message(WARNING " ABI mode not explicitly detected — compiler default in use.")
endif()

# =====================================================================
# 🧰 Features & Flags
# =====================================================================
message(STATUS "----------------------------------------------------")
message(STATUS "||[Features]||")
message(STATUS "----------------------------------------------------")
message(STATUS " Build Tests         : ${TASQLY_BUILD_TESTS}")
message(STATUS " Build Benchmarks    : ${TASQLY_BUILD_BENCHMARKS}")
message(STATUS " Build Reports       : ${TASQLY_BUILD_REPORTS}")

# =====================================================================
# 🧩 PostgreSQL Integration Status
# =====================================================================
if (DEFINED TASQLY_PG_AVAILABLE)
    message(STATUS "----------------------------------------------------")
    message(STATUS "||[PostgreSQL Integration]||")
    message(STATUS "----------------------------------------------------")

    if (TASQLY_PG_AVAILABLE)
        message(STATUS " PostgreSQL Status     : Found")
        if (DEFINED TASQLY_PG_INCLUDE_DIRS)
            message(STATUS " Include Directory     : ${TASQLY_PG_INCLUDE_DIRS}")
        endif()
        if (DEFINED TASQLY_PG_LIBRARIES)
            message(STATUS " Linked Library        : ${TASQLY_PG_LIBRARIES}")
        endif()
        message(STATUS " Active Mode           : TASQLY_WITH_POSTGRESQL = 1")
    else()
        message(STATUS " PostgreSQL Status     : Not Found (InMemory mode active)")
        message(STATUS " Active Mode           : TASQLY_WITH_POSTGRESQL = 0")
    endif()
endif()

# =====================================================================
# 🕒 Build Timing
# =====================================================================
string(TIMESTAMP TASQLY_CONFIGURE_DATE "%Y-%m-%d %H:%M:%S")
message(STATUS "----------------------------------------------------")
message(STATUS "||[Timing]||")
message(STATUS "----------------------------------------------------")
message(STATUS " Configure Timestamp : ${TASQLY_CONFIGURE_DATE}")

message(STATUS "----------------------------------------------------")
message(STATUS " ✅ Configuration completed successfully.")
message(STATUS "====================================================")

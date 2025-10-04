#
# 🧱 File: cmake/toolchains/msvc.cmake
# -------------------------------------
# 📌 Purpose   : Toolchain definition for MSVC (Microsoft Visual C++) with Ninja on Windows.
# 🧱 Layer     : Build System (CMake Toolchain)
# 👤 Author    : Mohamed Ali
# 🗓️ Updated   : 2025-09-18
#
# 🧠 Description:
# - Uses MSVC compiler (cl.exe) from Visual Studio / Build Tools if available.
# - Enforces C++20, disables extensions, and enables strict mode.
# - Qt paths are **not** set here → must be passed via CMAKE_PREFIX_PATH (QT_PREFIX env).
#

# ---------------------------------------------------------------
# 🏗️ System Root
# ---------------------------------------------------------------
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# ---------------------------------------------------------------
# 🎯 Compilers (auto-detect or fallback to cl.exe)
# ---------------------------------------------------------------
if (CMAKE_C_COMPILER STREQUAL "" OR CMAKE_CXX_COMPILER STREQUAL "")
    message(VERBOSE "No compiler set → forcing MSVC cl.exe from PATH")
    set(CMAKE_C_COMPILER cl CACHE STRING "C compiler" FORCE)
    set(CMAKE_CXX_COMPILER cl CACHE STRING "C++ compiler" FORCE)
endif()

# ---------------------------------------------------------------
# ⚙️ Language Standards
# ---------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "" FORCE)
set(CMAKE_CXX_EXTENSIONS OFF CACHE BOOL "" FORCE)

# ---------------------------------------------------------------
# ⚙️ MSVC-Specific Flags
# ---------------------------------------------------------------
if (MSVC)
    add_compile_options(
        /Zc:__cplusplus   # Report correct __cplusplus value
        /permissive-      # Strict standard compliance
        /W4               # High warning level
        /EHsc             # Standard C++ exception handling
        /utf-8            # Source and execution charset = UTF-8
    )
endif()

# ---------------------------------------------------------------
# 🔎 Find Behavior
# ---------------------------------------------------------------
set(CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH ON)
set(CMAKE_FIND_USE_CMAKE_ENVIRONMENT_PATH ON)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

# ---------------------------------------------------------------
# 📦 Packages
# ---------------------------------------------------------------
# Do NOT hardcode Qt here.
# Use CMAKE_PREFIX_PATH via QT_PREFIX environment or presets.

#
# 🧱 File: cmake/toolchains/mingw.cmake
# -------------------------------------
# 📌 Purpose   : Toolchain definition for MinGW-w64 with Ninja on Windows.
# 🧱 Layer     : Build System (CMake Toolchain)
# 👤 Author    : Mohamed Ali
# 🗓️ Updated   : 2025-09-18
#
# 🧠 Description:
# Uses GCC/G++ from MSYS2 MinGW64 via PATH to avoid mixing with other toolchains.
# Do not hardcode Qt here; presets/env should provide CMAKE_PREFIX_PATH (e.g., QT_PREFIX).
#

# ---------------------------------------------------------------
# 🎯 Compilers (only set if not already defined)
# ---------------------------------------------------------------
if (NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER)
    message(VERBOSE "No compiler set → falling back to gcc/g++ from PATH")
    set(CMAKE_C_COMPILER gcc CACHE STRING "C compiler" FORCE)
    set(CMAKE_CXX_COMPILER g++ CACHE STRING "C++ compiler" FORCE)
endif()

# ---------------------------------------------------------------
# 🏗️ System Root
# ---------------------------------------------------------------
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# ---------------------------------------------------------------
# ⚙️ Build Options
# ---------------------------------------------------------------
set(CMAKE_CXX_EXTENSIONS OFF CACHE BOOL "Disable compiler-specific extensions" FORCE)

# ---------------------------------------------------------------
# 🔎 Find Behavior (keep defaults; rely on PATH/env)
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
# Do NOT set Qt paths here. Provide Qt via CMAKE_PREFIX_PATH (e.g., QT_PREFIX)
# from presets or environment. This keeps the toolchain portable.

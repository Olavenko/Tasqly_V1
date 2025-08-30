#
# 🧱 File: cmake/toolchains/mingw.cmake
# -------------------------------------
# 📌 Purpose   : Toolchain definition for MinGW-w64 with Ninja on Windows.
# 🧱 Layer     : Build System (CMake Toolchain)
# 👤 Author    : Mohamed Ali
# 🗓️ Updated   : 2025-08-30
#
# 🧠 Description:
# This file forces CMake to use the GCC/G++ compilers from MSYS2 MinGW64.
# It avoids conflicts with other toolchains (e.g., FPC) by explicitly
# setting compilers and related build flags.
#

# ---------------------------------------------------------------
# 🎯 Compilers
# ---------------------------------------------------------------
# Force C and C++ compilers to point to MSYS2 MinGW-w64 toolchain.
# This prevents accidental detection of other gcc.exe stubs
# (e.g., Free Pascal’s fake gcc).
# ---------------------------------------------------------------
set(CMAKE_C_COMPILER "C:/msys64/mingw64/bin/gcc.exe" CACHE FILEPATH "C compiler" FORCE)
set(CMAKE_CXX_COMPILER "C:/msys64/mingw64/bin/g++.exe" CACHE FILEPATH "C++ compiler" FORCE)

# ---------------------------------------------------------------
# 🏗️ System Root
# ---------------------------------------------------------------
# Explicitly mark the system name and processor for cross-compilation.
# (Even though MinGW64 is native, this helps CMake resolve paths.)
# ---------------------------------------------------------------
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# ---------------------------------------------------------------
# ⚙️ Build Options
# ---------------------------------------------------------------
# Ensure Ninja is used as generator (set in presets).
# Disable compiler-specific extensions for portability.
# ---------------------------------------------------------------
set(CMAKE_CXX_EXTENSIONS OFF CACHE BOOL "Disable compiler-specific extensions" FORCE)

# ---------------------------------------------------------------
# 📦 Package Path
# ---------------------------------------------------------------
# Ensure CMake finds Qt and other packages installed under MSYS2/Qt.
# (This path is also reinforced in CMakePresets.json).
# ---------------------------------------------------------------
# NOTE: Don’t hardcode Qt here, keep it in presets for flexibility.

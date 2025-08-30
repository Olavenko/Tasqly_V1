# 0007 — Build Toolchain CI Policy

## Context
Tasqly requires a consistent cross-platform build system to ensure reproducibility between developer machines and CI runners.  
Historically, CI attempted to use MSVC or fallback stubs, which caused ABI mismatches with the baseline developer setup (MinGW + Qt).  

## Decision
- The official build stack is unified: **Qt 6.8 + MinGW on Windows (MSYS2)**, and **Qt 6.8 + GCC/Clang on Linux (Ubuntu)**.  
- GitHub Actions is configured to install **MSYS2 MinGW + Qt (win64_mingw)** on Windows, and **Qt (desktop/linux)** on Ubuntu.  
- CMake configuration requires `Qt6 >= 6.8`, no fallback stubs.  
- Caching is applied for MSYS2 and Qt to keep CI runtime practical.  

## Consequences
- ✅ Developers and CI use the same ABI and toolchain (no surprises).  
- ✅ CI runs meaningful tests with full Qt build (not stubs).  
- ✅ Future releases can safely publish CI artifacts identical to local builds.  
- ⏱️ CI jobs may take ~6–7 minutes initially, ~3 minutes with caching.  
- 🔄 MinGW setup on GitHub runners adds some maintenance overhead.

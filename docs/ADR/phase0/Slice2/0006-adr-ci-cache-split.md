# ADR 0006: Split-CI-Jobs-and-Enable-Build-Cache

- **Status**: Accepted
- **Date**: 2025-09-06
- **Owners**: Tasqly Team
- **Related**: Phase0 — Slice0 (Project Initialization & Testing Foundation)
- **Tags**: ci, caching, linux, windows, pipelines

---

## Context
The CI pipeline for Tasqly previously ran build and tests in a single job.  
This caused long runtimes and made debugging harder.  
Additionally, Linux test jobs failed at runtime due to missing Qt libraries (`libQt6Concurrent.so.6`).

## Decision
- Split CI workflow into **separate jobs** for build and test across all platforms (Linux GCC/Clang, Windows MinGW, Windows MSVC).
- **Enabled caching** for build directories to significantly reduce repeated build times.
- Added **re-installation of Qt 6.9.2** inside Linux test jobs to ensure `$Qt6_DIR` is available and libraries are discoverable at runtime.
- Exported `LD_LIBRARY_PATH` in Linux test jobs to fix runtime linking issues.

## Consequences
**Positive:**
- CI runtime reduced after first cache warm-up.
- Failures are easier to isolate (build vs. test).
- Stable Linux runtime environment for tests.

**Negative:**
- Some steps (Qt setup) are duplicated between build and test jobs.
- Cache invalidation may occasionally trigger full rebuilds.

## Notes
- This ADR ensures Tasqly’s CI pipeline remains **fast, stable, and maintainable** during ongoing vertical slice development.
- Future ADR may cover artifact publishing for releases.

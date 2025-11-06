# ADR 0004: Conditional-Skip-For-PostgreSQL-Integration-Tests-On-Windows

- **Status**: Accepted  
- **Date**: 2025-11-05  
- **Owners**: Tasqly Build & CI Team  
- **Related**: Phase1 — Slice2 (Persistence Layer)  
- **Tags**: cmake, github-actions, ci, postgresql, testing, cross-platform

---

## Context

During the continuous integration (CI) process, the Windows builds (both MSVC and MinGW) failed due to missing PostgreSQL client headers and libraries (`libpq-fe.h`, `PQerrorMessage` linkage errors).  
This occurred because the Windows CI runners (GitHub-hosted) do not have a running PostgreSQL server instance, and integration tests were tightly coupled with database connectivity.  

However, locally on developer machines and on Linux CI environments, PostgreSQL is available and tests run successfully.  
We needed a consistent solution to:
1. Allow **full database integration tests** on Linux and local dev setups.
2. Automatically **skip** PostgreSQL-related tests on Windows CI without breaking the build.

---

## Decision

The solution was to implement a **conditional PostgreSQL availability check** at the build level (CMake) and test runtime (GoogleTest fixture):

1. **CMake-level PostgreSQL detection**
   - Centralized in `FindTasqlyPostgreSQL.cmake`.
   - Detects PostgreSQL via `find_package(PostgreSQL)` or fallback paths (`C:/PostgreSQL/17`).
   - Sets a unified variable:  
     ```cmake
     set(TASQLY_PG_AVAILABLE TRUE/FALSE)
     ```

2. **Conditional test compilation**
   - In `Phase1_Tests.cmake`, after defining the test runner target:
     ```cmake
     if (NOT TASQLY_PG_AVAILABLE)
         message(WARNING "[Tests] 🚫 PostgreSQL unavailable — defining TASQLY_SKIP_DB_TESTS for conditional compilation.")
         target_compile_definitions(TasqlyTestsRunner PRIVATE TASQLY_SKIP_DB_TESTS)
     endif()
     ```
   - This prevents database-related tests from being compiled on systems without PostgreSQL.

3. **Runtime skip in fixture**
   - In `DatabaseIntegrationFixture.h`, if `TASQLY_SKIP_DB_TESTS` is defined or running on Windows CI (`CI=true`), the test suite uses:
     ```cpp
     GTEST_SKIP() << "Skipping PostgreSQL integration tests on Windows CI.";
     ```
   - Locally, the same logic detects and uses available PostgreSQL server instances automatically.

4. **GitHub Actions separation**
   - **Windows Jobs (MSVC + MinGW)** → Unit tests only  
   - **Linux Jobs (GCC + Clang)** → Full integration (includes PostgreSQL)  
   - PostgreSQL client installation remains for headers and linking, but no active DB service is used on Windows.

---

## Consequences

### ✅ Benefits
- CI no longer fails on Windows runners due to missing PostgreSQL server.
- Developers can still run integration tests locally on Windows if PostgreSQL is installed.
- Linux CI continues to validate real database interactions.
- Simplified maintenance — single test suite, conditional execution.

### ⚠️ Drawbacks
- Windows CI won’t validate DB integration logic (limited to unit tests).
- Any PostgreSQL-specific regressions might only be caught in Linux jobs.

### 🔄 Trade-offs
- Accepted platform asymmetry in favor of consistent CI stability.
- Deferred potential Dockerized PostgreSQL service for future cross-platform testing.

---

## References
- `cmake/FindTasqlyPostgreSQL.cmake`
- `cmake/Phase1_Tests.cmake`
- `tests/integration/common/DatabaseIntegrationFixture.h`
- `.github/workflows/ci.yml`
- Related ADRs: _0003-build-system-refactor_, _0002-testing-infrastructure-setup_

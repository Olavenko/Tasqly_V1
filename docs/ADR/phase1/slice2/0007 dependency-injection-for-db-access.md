# ADR 0007: dependency-injection-for-db-access

- **Status**: Draft  
- **Date**: 2025-11-12  
- **Owners**: Tasqly Team  
- **Related**: Phase1 — Slice3  
- **Tags**: refactor, postgres, repository, dependency-injection, testing, architecture

---

## Context

During testing of `P1_S2_PostgresTaskRepository`, the team faced limited access to internal members (`m_conn`, `executeQuery()`) required for validating PostgreSQL connection recovery logic and retry mechanisms.

To enable testing, a temporary compile-time macro `TASQLY_TESTING_BUILD` was introduced (ADR 0006).  
While this solved short-term coverage issues, it introduced a long-term architectural problem — conditional compilation within production code.

### Root Cause
The repository directly depends on the PostgreSQL C API (`libpq`) and manages connection lifecycle internally, creating **tight coupling** between:
- Business persistence logic, and  
- Database connection mechanics.

This coupling violates **Dependency Inversion (SOLID)** and blocks isolated unit testing.

---

## Decision

To enable maintainable and testable persistence, we will **introduce a Dependency Injection (DI) layer** via an abstract database interface.

### ✅ Adopted Approach

1. **Create Interface `IDbConnection`**
   - Defines generic database operations:
     - `execute(query: std::string)`
     - `reset()`
     - `isValid()`
   - Independent of PostgreSQL or any database vendor.

2. **Implement `PostgresConnection`**
   - Concrete class that wraps `libpq` logic.
   - Responsible for actual connection, query execution, and reconnection.
   - Resides under `src/infra/db/`.

3. **Refactor `P1_S2_PostgresTaskRepository`**
   - Instead of owning a `PGconn*`, it now depends on an injected `IDbConnection*`.
   - Connection management and recovery logic delegated to the injected dependency.
   - This removes the need for internal testing access.

4. **Update Unit Tests**
   - Create `FakeDbConnection` inside `tests/fakes/db/`.
   - Fake implements `IDbConnection` and simulates connection states:
     - failure on first call → success after `reset()`.
   - Enables deterministic tests for retry paths without touching real PostgreSQL.

5. **Remove `TASQLY_TESTING_BUILD` Macro**
   - All conditional access modifiers in headers will be deleted.
   - The build configuration for testing (`Phase1_Tests.cmake`) will be simplified.

6. **Documentation**
   - Update `Subsystems_Unified.md` to reflect “Persistence → Database Interface Layer”.
   - Add diagram in `docs/uml/phase1/slice3/component.puml` showing DI boundaries.

---

## Consequences

### ✅ Benefits
- Full test coverage of persistence behavior without exposing internals.  
- No compile-time divergence between production and testing builds.  
- Cleaner separation of concerns (`repository` vs `connection`).  
- Enables future database backends (InMemory, SQLite).  
- Aligns with Tasqly’s Quality Policy and SOLID principles.

### ⚠️ Drawbacks
- Requires moderate refactor (multiple files).  
- Slight increase in indirection and interface complexity.

### 🔄 Trade-offs
We sacrifice minor simplicity for major long-term testability, maintainability, and architecture integrity.

---

## Files Affected

| Category | Path | Change Type |
|-----------|------|-------------|
| **New Interface** | `src/infra/db/IDbConnection.h` | ➕ New |
| **New Implementation** | `src/infra/db/PostgresConnection.cpp/h` | ➕ New |
| **Refactor** | `src/infra/db/P1_S2_PostgresTaskRepository.cpp/h` | ♻️ Replace `PGconn*` with `IDbConnection*` |
| **Tests** | `tests/fakes/db/FakeDbConnection.h` | ➕ New |
| **Tests** | `tests/unit/persistence/test_P1_S2_PostgresTaskRepository.cpp` | ♻️ Adjust to use FakeConnection |
| **Build Config** | `cmake/Phase1_Tests.cmake` | ♻️ Remove `TASQLY_TESTING_BUILD` flag |
| **Docs** | `docs/adr/0006-testing-access-limitation.md` | 🔗 Reference this ADR as successor |
| **Diagrams** | `docs/uml/phase1/slice3/component.puml` | ➕ Add new “Persistence DI Layer” diagram |

---

## References
- [ADR 0006 — Testing Access Limitation in PostgresTaskRepository](0006-testing-access-limitation-in-postgrestaskrepository.md)  
- [Project_Quality_Policy.md](../Project_Quality_Policy.md)  
- [Subsystems_Unified.md](../Subsystems_Unified.md)  
- [Testing_Standards_Combined.md](../Testing_Standards_Combined.md)  
- [BuildPolicy.md](../BuildPolicy.md)  

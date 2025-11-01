# ADR 0002: PostgreSQL Connection Policy and Fallback to InMemory

- **Status**: Accepted  
- **Date**: 2025-10-23  
- **Author**: Mohamed Ali  
- **Phase/Slice**: Phase 1 / Slice 2  
- **Related**: P1_S2_PostgresTaskRepository.cpp, P1_S2_TaskRepositoryFactory.cpp, P1_AppSettings.cpp  

---

## Context

During Phase 1 / Slice 2, the infrastructure layer introduced a persistent repository
backed by PostgreSQL (`P1_S2_PostgresTaskRepository`) and an in-memory fallback implementation.
Runtime stability required clear policies for:

- Connection management (single connection vs. pooling)
- Retry and timeout on startup
- Credential masking in logs
- Seamless fallback to the in-memory repository when DB init fails

---

## Decision

### 1. Connection Management
Start with **a single libpq connection** per repository instance.  
Pooling is deferred to Phase 2 once concurrent workloads are profiled.  
All connection parameters are read from `P1_AppSettings` (`DB_HOST`, `DB_PORT`, …).

### 2. Retry + Timeout Policy
- `connect()` retries **once** after 500 ms if `PQconnectdbParams` fails.  
- Default timeout: **5 seconds**, configurable via `DB_CONNECT_TIMEOUT`.  
- Policy ensures quick recovery on transient startup errors without hanging.

### 3. Fallback Behavior
- If connection ultimately fails:
  - A structured `P1_Error(DbInit)` is logged and emitted.
  - The `P1_S2_TaskRepositoryFactory` activates the **InMemoryTaskRepository** automatically.
  - User feedback is provided through `P1_Notifier::fallbackActivated()`.
- The behavior is gated by env flag  
  `FEATURES_DB_FALLBACK_INMEMORY` (default = true).

### 4. Security & Logging
- Sensitive values (`DB_PASS`, `token`, `secret`, …) are masked by `P1_Error::toString(true)`.  
- All DB events logged through `P1_Logger` with proper level (INFO / WARN / ERROR).

### 5. Seeding Policy
- The fallback repository may seed deterministic demo data if  
  `FEATURES_INMEMORY_SEED=true` (development / testing only).  
- Seeds are static for test reproducibility.

### 6. Future Work (Phase 2)
- Evaluate lightweight connection pool (e.g., 1–4 persistent sessions).  
- Add exponential backoff for retry.  
- Integrate runtime health checks for Postgres reconnect.

---

## Consequences

| Aspect | Impact |
|--------|---------|
| **Reliability** | App auto-recovers from DB failure during startup |
| **Security** | No credentials appear in logs |
| **Performance** | Slight delay (≤ 0.5 s) on transient retry |
| **Maintainability** | Centralized feature-flag control via `P1_AppSettings` |
| **Scalability** | Pooling deferred to a future slice |

---

## Verification

- ✅ Unit Tests: InMemoryRepo CRUD parity  
- ✅ Integration Tests: Postgres migration + rollback  
- ✅ Manual Tests: Simulated SSL lib missing → fallback triggered  
- ✅ CI Pipeline: env variables masked & verified  

---

## Status

The policy is **implemented and enforced** as of Phase 1 / Slice 2.  
Future ADRs will extend this one for connection pooling and advanced retry logic.

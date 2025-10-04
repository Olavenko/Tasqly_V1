# ADR 0001: Repository-Error-Codes

- **Status**: Draft
- **Date**: 2025-09-07
- **Owners**: Tasqly Team
- **Related**: Phase0 — Slice4
- **Tags**: persistence, error, repository

---

## Context
Repositories need to return errors via `Result<T, Error>`.  
Currently, developers may use arbitrary numeric codes or messages, leading to inconsistency across modules and tests.

## Decision
Define a standardized enum `RepoErrorCode` inside `Error.h` for common repository-level failures:

```cpp
namespace tasqly::domain::core {
enum class RepoErrorCode {
    NotFound        = 404,  // entity not found
    AlreadyExists   = 409,  // conflict on insert
    StorageFailure  = 500   // internal failure
};
}
```

Repositories will construct `Error` values using these codes, ensuring consistency in both code and testing.

## Consequences
- ✅ Consistent error handling across repositories.  
- ✅ Easier to test (compare against enum values).  
- ✅ Clear mapping between error semantics and codes.  
- ⚠️ Adds a small upfront design overhead.  
- 🔄 Future expansion may add codes (e.g., Unauthorized, Timeout).  

## References
- `src/domain/core/Error.h`  
- `src/domain/core/Result.h`

# ADR 0003: Namespace-And-Naming-Policy

- **Status**: Accepted
- **Date**: 2025-09-07
- **Owners**: Tasqly Team
- **Related**: Phase0 — Slice4
- **Tags**: namespace, naming, consistency

---

## Context
During Slice 4 implementation, entities (`Task`, `Goal`) and repository interfaces (`ITaskRepository`, `IGoalRepository`) were initially declared in the global namespace.  
This caused compiler errors when trying to consume them in use cases (e.g. `ListTasks`), since other domain types (`Error`, `Result`) were already under the namespace `tasqly::domain::core`.

## Decision
All domain types (entities, repositories, utilities) must be declared under:

```cpp
namespace tasqly::domain::core { ... }
```

This includes:  
- Entities: `Task`, `Goal`  
- Interfaces: `ITaskRepository`, `IGoalRepository`, `IClock`, `IUuidGen`  
- Value types: `Error`, `Result`  

Application layer types remain under:  
```cpp
namespace tasqly::app::{subsystem}
```

(e.g. `tasqly::app::usecases`, `tasqly::app::logging`).

## Consequences
- ✅ Clear separation of layers.  
- ✅ Domain symbols are grouped under one namespace.  
- ✅ Consistent with existing domain types.  
- ⚠️ Call sites must use fully-qualified names or `using namespace`.  
- 🔄 Existing code must be updated to reference fully-qualified names.  

## References
- `src/domain/core/Error.h`  
- `src/domain/core/Result.h`  
- `src/app/core/AppContext.h`

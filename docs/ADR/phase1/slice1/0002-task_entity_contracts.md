# ADR 0002: Task Entity & Contracts — No Qt/DB in Domain

- **Status:** Draft  
- **Date:** 2025-10-06  
- **Owners:** Tasqly Team  
- **Related:** Phase 1 — Slice 1  
- **Tags:** domain, entity, repository, validation, result

---

## Context
Tasqly’s domain layer must remain **pure, testable, and infrastructure-agnostic**. During early planning of Phase 1, we identified the risk of coupling domain entities directly to Qt or database-specific types. Such coupling would violate Clean Architecture principles and hinder testing, portability, and scalability.

To mitigate this, we must design a clean domain model for tasks, including:
- A `Task` entity with strongly typed fields.
- Enum-based status and priority for state clarity.
- Repository interfaces (`ITaskRepository`) for persistence abstraction.
- Pure C++ types (no QVariant, QString, QDateTime, etc.).

---

## Decision
We adopt a **clean, Qt-free, DB-free domain layer** for Tasqly. All domain logic will use standard C++ and custom type wrappers (`Uuid`, `TimePoint`) provided in Phase 0.

### ✅ Entities
**Task Entity (domain/core/Task.h)**
```cpp
#pragma once

#include <string>
#include <optional>
#include "domain/core/Result.h"
#include "domain/core/Error.h"
#include "domain/core/Uuid.h"
#include "domain/core/TimePoint.h"

namespace tasqly::domain::core {

enum class TaskStatus { Todo, Doing, Done };
enum class TaskPriority { Low, Normal, High };

struct Task {
    Uuid id;
    std::string title;
    std::string notes;
    TaskStatus status;
    TaskPriority priority;
    std::optional<TimePoint> deadline;
    TimePoint createdAt;
    TimePoint updatedAt;
};

/// 🧪 Validation helper
domain::core::Result<void, Error> validateTask(const Task& t);

} // namespace tasqly::domain::core
```

### ✅ Repository Interface
**ITaskRepository (domain/core/ITaskRepository.h)**
```cpp
#pragma once

#include <vector>
#include <optional>
#include "domain/core/Task.h"
#include "domain/core/Result.h"
#include "domain/core/Error.h"

namespace tasqly::domain::core {

class ITaskRepository {
public:
    virtual Result<Task, Error> create(const Task& task) = 0;
    virtual Result<Task, Error> update(const Task& task) = 0;
    virtual Result<void, Error> remove(const Uuid& id) = 0;
    virtual Result<std::vector<Task>, Error> list(
        std::optional<TaskStatus> status = std::nullopt,
        std::optional<TaskPriority> priority = std::nullopt,
        int limit = 100,
        int offset = 0) = 0;
    virtual ~ITaskRepository() = default;
};

} // namespace tasqly::domain::core
```

### ✅ Validation Contract
Rules enforced in `validateTask()`:
| Condition | Error Code | Description |
|------------|-------------|--------------|
| `title.empty()` | VALIDATION_EMPTY_TITLE | Task must have non-empty title |
| `deadline < now()` | VALIDATION_DEADLINE_PAST | Deadline cannot be in the past |

---

## Consequences
### ✅ Benefits
- Pure, dependency-free domain logic (fully testable).
- Consistent architecture across future slices.
- Easy integration with different infrastructures (InMemory, PostgreSQL, REST).

### ⚠️ Drawbacks
- Slightly more code due to manual mapping (to DTOs).
- Requires additional mappers and validation layers.

### 🔄 Trade-offs
- **Pro:** Predictable behavior, strong boundaries, reusable domain.
- **Con:** More boilerplate vs. direct data binding to Qt.

---

## References
- [ADR-template.md](../../../../docs/ADR-template.md)
- [Subsystems_Unified.md](../../../../Development%20Standards/Subsystems_Unified.md)
- [C++_Standards_Combined.md](../../../../Development%20Standards/Standards/C++_Standards_Combined.md)
- [FilePolicy.md](../../../../Development%20Standards/Policies/FilePolicy.md)
- [Project_Quality_Policy.md](../../../../Development%20Standards/Project_Quality_Policy.md)
- [Testing_Standards_Combined.md](../../../../Development%20Standards/Standards/Testing_Standards_Combined.md)

---

**Next Step:** Generate ADR 0002 — “DTO Mappers vs Direct Entity Exposure.”
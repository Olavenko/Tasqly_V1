# ADR 0001: Domain Entities, Contracts & Mappers

- **Status:** Draft  
- **Date:** 2025-10-06  
- **Owners:** Tasqly Team  
- **Related:** Phase 1 — Slice 1  
- **Tags:** domain, entity, repository, validation, result
---

## 🎯 Goal
Establish a clean, Qt-independent **domain layer foundation** for Tasqly, including:
- Core `Task` entity (pure domain object)
- Repository interfaces (`ITaskRepository`)
- Enums and validation rules
- DTO mappers (`TaskDto <-> Task`)
- Documentation of the mapping strategy (manual vs codegen)

This slice ensures **zero Qt/DB leakage** into the domain layer and prepares for later infrastructure slices (InMemory and Postgres repos).

---

## 🧩 Dependencies
- **Phase 0:**
  - `Result<T, Error>` — for all return values and error propagation
  - `Error` — unified error system integrated with LogManager/Notifier
  - `Id` and `TimePoint` — for identity and time abstraction
- **Subsystems:**
  - Error subsystem (for domain-level validation errors)
  - Logging subsystem (for debug/trace integration in later slices)
  - Persistence subsystem (future integration only)

---

## 🧱 Domain Model Scope
### 1. Entity: `Task`
Represents a single to-do item or goal.

| Field | Type | Notes |
|-------|------|-------|
| `id` | `Uuid` | Immutable identifier |
| `title` | `std::string` | Required, non-empty |
| `notes` | `std::string` | Optional details |
| `status` | `TaskStatus` | Enum: `Todo`, `Doing`, `Done` |
| `priority` | `TaskPriority` | Enum: `Low`, `Normal`, `High` |
| `deadline` | `std::optional<TimePoint>` | Must be >= now if set |
| `createdAt` | `TimePoint` | Auto-generated on creation |
| `updatedAt` | `TimePoint` | Auto-updated on modification |

---

### 2. Enums
```cpp
enum class TaskStatus { Todo, Doing, Done };
enum class TaskPriority { Low, Normal, High };
```

---

### 3. Interfaces
`ITaskRepository` defines CRUD and filtering logic:
```cpp
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
```

---

### 4. Mappers
Two-way translation between domain and infrastructure DTOs.
- **Domain → DTO:** Prepares for DB or API persistence.
- **DTO → Domain:** Converts persisted objects into core entities.

⚠️ **Decision required (ADR 0003):** Manual mapping vs. code generation.

---

## ⚖️ Validation Rules
| Rule | Condition | Error Code |
|------|------------|-------------|
| Empty title | `title.empty()` | `VALIDATION_EMPTY_TITLE` |
| Deadline in past | `deadline < now()` | `VALIDATION_DEADLINE_PAST` |

---

## ⚠️ Edge Cases
- Empty title → `Err(code: VALIDATION)`
- Deadline in past → `Err(code: VALIDATION)`
- Duplicated ID in repository → `Err(code: DUPLICATE_ID)`

---

## 🧠 Testing Matrix
| Type | Scope | Expected |
|------|--------|-----------|
| Unit | Entity validation | ✅ Title & deadline validation |
| Unit | Enum parsing | ✅ Correct mapping to string/int |
| Unit | Mapper roundtrip | ✅ Task → DTO → Task identical |
| Integration | Domain + FakeRepo | ✅ CRUD + filters functional |

---

## 🔄 ADRs to Produce
| ID | Title | Purpose |
|----|--------|----------|
| 0001 | Task entity & contracts — no Qt/DB in domain | Define pure domain structure |
| 0002 | DTO Mappers vs direct entity exposure | Evaluate pros/cons of DTO pattern |
| 0003 | Mapper strategy — manual vs codegen | Decide mapper implementation approach |

---

## 🧪 Quality Hooks
- Static analysis (clang-tidy, cppcheck)
- Coverage ≥ 60% (Phase 1 baseline)
- All tests pass under Address/UndefinedSanitizers
- Unit tests: `tests/domain/task/TaskTests.cpp`

---

## 📚 References
- [C++_Standards_Combined.md](../../../../Development%20Standards/Standards/C++_Standards_Combined.md)
- [FilePolicy.md](../../../../Development%20Standards/Policies/FilePolicy.md)
- [Testing_Standards_Combined.md](../../../../Development%20Standards/Standards/Testing_Standards_Combined.md)
- [Project_Quality_Policy.md](../../../../Development%20Standards/Project_Quality_Policy.md)
- [Subsystems_Unified.md](../../../../Development%20Standards/Subsystems_Unified.md)
- [ADR-template.md](../../../../docs/ADR-template.md)

---

**Next Step:** Create `0001-task-entity-contracts.md` ADR to finalize entity and repository design.


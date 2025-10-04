# ADR 0004: Pointer-based Storage for InMemory Repositories

## 📝 Context
Currently, `InMemoryGoalRepository` and `InMemoryTaskRepository` store **entity objects** (`Goal`, `Task`) directly inside their internal containers (`QHash` or `unordered_map`).

### Issues:
- **High Copy Overhead**: `listSync` returns full copies of vectors containing thousands of objects (seen in `BM_GoalRepo_List_Stress`).
- **Inefficient add/update**: `add` and `update` require additional copies/moves, even when only references are needed.
- **Benchmarks Impact**: Variance and CPU Time spikes observed in `BM_GoalRepo_Add_Stress`, `BM_GoalRepo_List_Stress`, and `BM_TaskRepo_Add_Stress`.

This directly violates performance expectations defined in `phase0x_quality_policies_ordered.md` and subsystem KPIs 【Subsystems_Unified.md】.

## 🎯 Decision
We will migrate repositories to use **pointer-based storage**:
- Store entities as `std::unique_ptr<Goal>` and `std::unique_ptr<Task>` internally.
- Update repository APIs (`add`, `update`, `list`) to:
  - Accept and manage entity ownership (`std::unique_ptr`).
  - Return lightweight views (`Goal*`, `Task*`) inside vectors.
- Benchmarks and application services will consume pointers instead of full objects.

### Example:
```cpp
// Before
QHash<QString, Goal> m_goals;

// After
QHash<QString, std::unique_ptr<Goal>> m_goals;

// Before API
std::future<Result<std::vector<Goal>>> list(int limit, int offset);

// After API
std::future<Result<std::vector<Goal*>>> list(int limit, int offset);
```

## ✅ Benefits
- **Performance**: Reduces copies during add/list/update.
- **Consistency**: Unified ownership semantics — repositories own entities.
- **Scalability**: Prepares for future iterator/view-based APIs.
- **Application Benefit**: Unlike span/view-only solution, this refactor benefits **both benchmarks and the running application** immediately.

## ⚠️ Consequences
- **Breaking Change**: Any code consuming `list()` or `add()` must be updated to handle pointers instead of objects.
- **Tests Will Break**: Unit tests comparing objects must be refactored to dereference pointers.
- **Refactor Required Across Layers**: Services, use cases, and benchmarks will all require migration.

## 🔗 Impacted Files
- `InMemoryGoalRepository.{h,cpp}`
- `InMemoryTaskRepository.{h,cpp}`
- `IGoalRepository.h`, `ITaskRepository.h`
- `Bench_InMemoryRepo.cpp`
- Services (`domain/services/…`)
- Use cases (`application/usecases/…`)
- Tests (`tests/unit/...`)

## 🛠️ Alternatives Considered
1. **Quick Fix (string → QString, reserve, bulk)**
   - ✅ Small improvement
   - ❌ Application doesn’t benefit fully

2. **Span/View API (Zero-copy view)**
   - ✅ Backward compatible
   - ❌ Application doesn’t benefit unless migrated to new API

3. **Pointer-based Storage (Chosen)**
   - ✅ Structural fix
   - ✅ Application + benchmarks benefit
   - ❌ Requires full refactor

## 🚀 Next Steps
1. Implement pointer-based storage in **InMemoryGoalRepository**.
2. Update **IGoalRepository** interface to return vectors of pointers.
3. Apply the same change to **InMemoryTaskRepository**.
4. Update benchmarks to consume pointer-based results.
5. Fix broken tests and services.
6. Rerun benchmarks and validate reduced variance and CPU time.

## 📅 Status
**Accepted — Phase0 Implementation Ongoing**
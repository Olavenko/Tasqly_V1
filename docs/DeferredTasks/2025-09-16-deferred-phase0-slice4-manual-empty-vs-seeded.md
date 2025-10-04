# Deferred Task — manual-test-empty-vs-seeded

- **Status**: Deferred  
- **Date Created**: 2025-09-16  
- **Owner**: Tasqly Team  
- **Phase/Slice**: Phase0 — Slice4  
- **Tags**: testing, persistence, manual

---

## Context
Manual test case: Verify TaskListPage shows correct state for empty vs seeded repositories.  
Defined in Slice4 testing matrix.

## Reason for Deferral
Manual test execution not yet completed.  
Deferred to later phase after automated seed toggle tests were validated.  
Required for user-facing verification.

## Next Steps
- Run manual test with features.inmemory enabled/disabled.  
- Confirm TaskListView renders empty when seed=off, and seeded items otherwise.  
- Document results in test report.  

## References
- TaskListPage.qml  
- InMemoryTaskRepository.cpp  
- tests/integration/persistence/test_seed_toggle.cpp

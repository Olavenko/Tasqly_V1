# Deferred Task — seed-off-empty-state

- **Status**: Deferred  
- **Date Created**: 2025-09-16  
- **Owner**: Tasqly Team  
- **Phase/Slice**: Phase0 — Slice4  
- **Tags**: persistence, repository, seeding

---

## Context
Edge case: Seed off → “Empty” state should render correctly in UI.  
Defined in Slice4 (Persistence Abstraction).

## Reason for Deferral
Validation of empty state rendering not yet performed.  
Requires QML integration test with SmartPageLoader and TaskListView.  
Deferred due to time-boxing of Slice4 implementation.

## Next Steps
- Implement test case ensuring TaskListPage shows empty state when seed is disabled.  
- Verify consistency between QML loader state and repository state.  
- Add regression test to prevent future regressions.  

## References
- TaskListPage.qml  
- InMemoryTaskRepository.cpp  
- tests/integration/persistence/test_seed_toggle.cpp

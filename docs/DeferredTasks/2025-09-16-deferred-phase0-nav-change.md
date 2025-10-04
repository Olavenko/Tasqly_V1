# Deferred Task — navigation-change-performance

- **Status**: Deferred  
- **Date Created**: 2025-09-16  
- **Owner**: Tasqly Team  
- **Phase/Slice**: Phase0 — Global  
- **Tags**: performance, navigation, benchmark

---

## Context
Target: Navigation change < 120ms (≤5 components).  
Defined in Phase0 performance bars.

## Reason for Deferral
No automated navigation benchmarks exist.  
SmartPageLoader integration benchmarks missing in CI.  
Need profiling with 5+ QML components to validate responsiveness.

## Next Steps
- Add benchmark for NavigationService route changes.  
- Integrate with Google Benchmark + QML Profiler.  
- CI/CD validation required before release freeze.  

## References
- Subsystems_Unified.md (Navigation System)  
- QML_Standards_Combined.md (page loading rules)

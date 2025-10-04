# Deferred Task — theme-switch-performance

- **Status**: Deferred  
- **Date Created**: 2025-09-16  
- **Owner**: Tasqly Team  
- **Phase/Slice**: Phase0 — Global  
- **Tags**: performance, theme, benchmark

---

## Context
Target: Theme switch < 100ms.  
Defined in Phase0 performance bars.

## Reason for Deferral
Theme switching benchmarks are not automated.  
Requires Google Benchmark integration + QML Profiler runs.  
CI pipeline does not yet validate frame timing.

## Next Steps
- Write benchmark for ThemeManager palette switch.  
- Integrate QML Profiler validation in CI.  
- Confirm latency ≤100ms on target devices.  

## References
- Subsystems_Unified.md (ThemeManager)  
- QML_Standards_Combined.md (styling rules)

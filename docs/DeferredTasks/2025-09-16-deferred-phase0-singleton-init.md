# Deferred Task — singleton-init-qml-facades

- **Status**: Deferred  
- **Date Created**: 2025-09-16  
- **Owner**: Tasqly Team  
- **Phase/Slice**: Phase0 — Global  
- **Tags**: performance, qml, init

---

## Context
Target: Ensure Singleton init (QML facades) < 50ms.  
Defined in Phase0 performance bars.

## Reason for Deferral
Measurement not automated in CI yet.  
Requires integration of Google Benchmark with application startup profiling.  
Currently no reliable dataset to confirm timing across Windows/Linux.

## Next Steps
- Add benchmark for QML Singleton initialization.  
- Automate benchmark execution in CI (Debug/Release).  
- Validate with Empty/Medium/Stress datasets.  

## References
- Project_Quality_Policy.md  
- performance/benchmarks/logging/Bench_FirstLogWrite.cpp

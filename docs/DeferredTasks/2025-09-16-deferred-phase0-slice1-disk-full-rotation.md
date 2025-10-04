# Deferred Task — disk-full-log-rotation

- **Status**: Deferred  
- **Date Created**: 2025-09-16  
- **Owner**: Tasqly Team  
- **Phase/Slice**: Phase0 — Slice1  
- **Tags**: logging, error, rotation

---

## Context
Edge case: Disk full during log rotation.  
Expected behavior: fallback to console and drop excess logs.  
Defined in Slice1 (Logging & Error Backbone).

## Reason for Deferral
Implementation of safe fallback for disk full condition not yet completed.  
Requires simulation of disk limits + test environment hooks.  
Not critical for initial MVP logging backbone.

## Next Steps
- Implement log rotation guard to detect disk full errors.  
- Ensure LogManager safely falls back to console output.  
- Add integration test simulating disk limit.  

## References
- Subsystems_Unified.md (Logging System)  
- tests/unit/logging/test_rotation_policy.cpp

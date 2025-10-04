# Deferred Task — empty-error-message-default

- **Status**: Deferred  
- **Date Created**: 2025-09-16  
- **Owner**: Tasqly Team  
- **Phase/Slice**: Phase0 — Slice1  
- **Tags**: error, defaults, reporting

---

## Context
Edge case: Empty error message should default to "Unknown error".  
Defined in Slice1 (Logging & Error Backbone).

## Reason for Deferral
Default mapping for empty error messages not yet implemented.  
Requires ErrorReporter update + QML Notifier adaptation.  
Deferred as non-blocking for core slice completion.

## Next Steps
- Update Error struct handling to replace empty message with default.  
- Verify QML Toast correctly displays "Unknown error".  
- Add regression test for empty message case.  

## References
- Subsystems_Unified.md (Error Handling System)  
- AppErrors.cpp / AppErrors.h

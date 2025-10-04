# Deferred Task — persist-last-navigated-route

- **Status**: Deferred  
- **Date Created**: 2025-09-16  
- **Owner**: Tasqly Team  
- **Phase/Slice**: Phase0 — Slice3  
- **Tags**: navigation, persistence, appsettings

---

## Context
Persist last navigated Route in AppSettingsManager.  
Goal: restore user context on application restart.  
ADR required: resume context on restart.

## Reason for Deferral
Persistence feature not yet implemented.  
Requires extension of AppSettingsManager schema and integration tests.  
Deferred due to time-boxing of Slice3.

## Next Steps
- Extend AppSettingsManager to store last Route.  
- Add integration test for restoring Route after restart.  
- Document in ADR once implemented.  

## References
- AppSettingsManager.cpp/h  
- Subsystems_Unified.md (Navigation System)

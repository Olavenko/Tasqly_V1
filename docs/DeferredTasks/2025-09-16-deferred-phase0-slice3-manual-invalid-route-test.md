# Deferred Task — manual-test-invalid-route

- **Status**: Deferred  
- **Date Created**: 2025-09-16  
- **Owner**: Tasqly Team  
- **Phase/Slice**: Phase0 — Slice3  
- **Tags**: testing, navigation, manual

---

## Context
Manual test case: Invalid route should trigger toast + maintain stable navigation stack.  
Defined in Slice3 testing matrix.

## Reason for Deferral
Manual test execution not yet performed.  
Deferred due to prioritization of automated tests.  
Must be validated before end of Phase0.

## Next Steps
- Execute manual test in debug builds.  
- Verify invalid route produces toast notification.  
- Confirm stack remains stable (no unintended changes).  
- Document result in test report.  

## References
- NavigationService.cpp  
- SmartPageLoader.qml  
- Toast.qml

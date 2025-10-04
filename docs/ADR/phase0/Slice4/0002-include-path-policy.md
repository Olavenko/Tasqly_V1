# ADR 0002: Include-Path-Policy

- **Status**: Draft
- **Date**: 2025-09-07
- **Owners**: Tasqly Team
- **Related**: Phase0 — Slice4
- **Tags**: include, cmake, consistency

---

## Context
Previously, header files in the project were often included using local paths (e.g., `#include "Goal.h"` from within `domain/core`).  
This caused inconsistencies and occasional compiler errors when including across layers (e.g., infra ↔ domain).

## Decision
All includes must be written relative to the `src/` directory root.  
For example:
```cpp
#include "domain/core/Goal.h"
#include "domain/core/Result.h"
#include "app/settings/AppSettingsManager.h"
#include "infra/persistence/InMemoryTaskRepository.h"
```

Local includes like `#include "Goal.h"` are deprecated and should be refactored incrementally.

## Consequences
- ✅ Consistent include style across the project.  
- ✅ Easier to navigate includes (clear layer/module context).  
- ✅ Avoids ambiguous resolution issues when files with the same name exist in multiple folders.  
- ⚠️ Requires refactoring of older headers over time.  
- 🔄 Transitional period: old includes will still compile, but must be replaced gradually.  

## References
- `CMakeLists.txt` (target_include_directories)  
- `src/domain/core/Goal.h`  
- `src/domain/core/Result.h`  
- ADR 0001: Repository Error Codes

# 🧱 Header File Template (`.h`)

```cpp
/*
 * 🧱 File: <FileName>.h
 * ---------------------
 * 📌 Purpose   : [Short sentence about what this file is for]
 * 🧱 Layer     : [Domain (Core) | Application (Use Cases) | Infrastructure | etc.]
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : <YYYY-MM-DD>
 * 🗓️ Updated   : <YYYY-MM-DD>
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable (update explicitly if Experimental/Deprecated)
 *
 * 🧠 Description:
 * - Briefly describe the responsibilities of this file.
 * - Mention how it fits into the overall architecture.
 *
 * 🔗 Depends On:
 *   - List all QML components, pages, or C++ classes that this file requires to function (write `None` if not applicable).
 *   - All dependencies must be written explicitly, not just as an example.
 * 🔗 Related   : [Cross-module related files]
 * ✅ Tested In : [Test file name if available]
 * ⚠️ Errors    : [Exceptions thrown / error handling contract]
 * ⚡ Performance: [Complexity notes / optimization hints]
 * 🔒 Security  : [Sanitization / sensitive handling if applicable, write `N/A` if not applicable]
 * 🕒 Last Major Change: <YYYY-MM-DD>
 */

#pragma once

// 👉 Always use relative paths from `src/`
#include "domain/core/Task.h"   // example include

namespace tasqly::<layer>::<subsystem> {

// 👉 Declare your class/struct/interface here
// - Keep public API first
// - Keep comments short and clear

} // namespace tasqly::<layer>::<subsystem>
```
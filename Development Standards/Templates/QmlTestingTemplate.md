# 🧪 QML Testing Template (`.qml`)

This template defines the baseline for writing **QML unit and integration tests**  
using **QtTest** in the Tasqly project.

---

```qml
/*
 * 🧱 File: <TestName>.qml
 * -----------------------
 * 📌 Purpose   : [Short sentence about what this test covers]
 * 🧱 Layer     : UI (Testing)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : <YYYY-MM-DD>
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable (update explicitly if Experimental/Deprecated)
 *
 * 🧠 Description:
 * - Define what this test verifies (component behavior, page state, etc.).
 * - Mention expected inputs/outputs and edge cases.
 *
 * 🔗 Depends On:
 *   - List all QML components, pages, or C++ classes that this file requires to function (write `None` if not applicable).
 *   - All dependencies must be written explicitly, not just as an example.
 * 🔗 Related   : [Cross-module related files]s
 * ✅ Tested In : [Test file name if available]
 * ⚠️ Errors    : [Exceptions thrown / error handling contract]
 * ⚡ Performance: [Complexity notes / optimization hints]
 * 🔒 Security  : [Sanitization / sensitive handling if applicable, write `N/A` if not applicable]
 * 🕒 Last Major Change: <YYYY-MM-DD>
 */

import QtQuick 2.15
import QtTest 1.2
import Tasqly.Ui.Components 1.0

TestCase {
    name: "SidebarTest"

    // 👉 Component under test
    Sidebar {
        id: sidebar
    }

    // 👉 Unit Test Example
    function test_defaultVisible() {
        compare(sidebar.visible, true)
    }

    // 👉 Signal Test Example
    function test_buttonClick() {
        var clicked = false
        sidebar.clicked.connect(() => clicked = true)
        sidebar.clicked()
        compare(clicked, true)
    }

    // 👉 Snapshot Test Example
    function test_snapshotState() {
        sidebar.state = "collapsed"
        verify(sidebar.state === "collapsed")
    }
}
```

---

## Guidelines
- **Unit tests**: for reusable components (properties, signals).  
- **Snapshot tests**: verify visual states (`loading`, `error`, `collapsed`).  
- **Integration tests**: for navigation and multi-component interaction.  
- **Naming**: test files should match component/page name, e.g. `TaskButtonTest.qml`.  

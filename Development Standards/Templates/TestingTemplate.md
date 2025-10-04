# 🧪 Testing Template (`.cpp` / `.qml`)

```cpp
/*
 * 🧱 File: <TestFileName>.cpp / <ComponentName>Test.qml
 * -------------------------
 * 📌 Purpose   : [Short sentence about what this test covers]
 * 🧱 Layer     : [Unit | Integration | Snapshot | Regression]
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : <YYYY-MM-DD>
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable (update explicitly if Experimental/Deprecated)
 *
 * 🧠 Description:
 * - Describe what this test validates (component behavior, page state, use case result).
 * - Mention important edge cases and expected results.
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
```

---

## Example (C++ with GoogleTest)
```cpp
#include <gtest/gtest.h>
#include "Task.h"

TEST(TaskTest, DefaultStatus) {
    Task t("Sample");
    EXPECT_EQ(t.status(), "pending");
}

TEST(TaskTest, MarkAsDone) {
    Task t("Sample");
    t.markDone();
    EXPECT_EQ(t.status(), "done");
}
```

---

## Example (QML with QtTest)
```qml
import QtQuick 2.15
import QtTest 1.2
import Tasqly.Ui.Components 1.0

TestCase {
    name: "TaskButtonTest"

    TaskButton {
        id: button
        text: "Add Task"
    }

    function test_defaultText() {
        compare(button.text, "Add Task")
    }

    function test_clickSignal() {
        var clicked = false
        button.clicked.connect(() => clicked = true)
        button.clicked()
        compare(clicked, true)
    }
}
```

---

## Guidelines
- **Naming**: Use `Test_<ClassName>.cpp` for C++ or `<ComponentName>Test.qml` for QML.  
- **Structure**: Follow Arrange → Act → Assert.  
- **Isolation**: Unit tests must be independent and fast.  
- **Snapshots**: Required for UI components with multiple states (loading, error, empty).  
- **Integration**: Required when multiple components/services interact.  

---

## TODO / Future
- Add regression tests for critical bug fixes.  
- Automate snapshot testing for QML Pages with loaders and navigation.  

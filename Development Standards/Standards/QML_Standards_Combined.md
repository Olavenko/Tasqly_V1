# 🎨 QML Standards — Tasqly

This document unifies all QML-related standards used in the Tasqly project.  
It covers components, pages, style guide, and testing practices.  
Always follow these standards to ensure **consistency, maintainability, and accessibility**.

---

## 🔹 Execution Order (High-Level Workflow)
1. Create QML Component/Page using the template.  
2. Apply Style Guide rules (naming, theming, layout).  
3. Write at least one QML Test case for every new component/page.  
4. Run UI Profiling with QML Profiler if performance issues are suspected.  

---

## 1. QML Component Template (`.qml`)

```qml
/*
 * 🧱 File: <ComponentName>.qml
 * ----------------------------
 * 📌 Purpose   : [Short sentence about what this component is for]
 * 🧱 Layer     : UI (Component)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : <YYYY-MM-DD>
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable (update explicitly if Experimental/Deprecated)
 *
 * 🧠 Description:
 * - Describe what this reusable component does.
 * - Mention key use cases and where it is intended to be used.
 *
 * 🔗 Depends On:
 *   - List all QML components, pages, or C++ classes that this file requires to function (write `None` if not applicable).
 * 🔗 Related   : [Cross-module related files]
 * 🎨 Style     : Uses ThemeManager for styling (no hardcoded colors).
 * ✅ Tested In : [Test QML file if available]
 * ♿ Accessibility: Focus [Yes/No], Keyboard [Yes/No], Screen Reader [Yes/No]
 * ⚡ Performance : [Notes if component is inside a repeater/list]
 * 🕒 Last Major Change: <YYYY-MM-DD>
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root
    width: 200
    height: 50

    // 👉 Custom Properties
    property alias text: label.text   // Public API (must have default value)

    // 👉 Signals
    signal clicked()                  // User interaction

    // 👉 Layout Section
    Rectangle {
        id: background
        anchors.fill: parent
        color: ThemeManager.palette.primary

        Label {
            id: label
            anchors.centerIn: parent
            text: "Default"
        }
    }

    // 👉 Behavior Section
    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.clicked()
            console.log("Component clicked")
        }
    }

    // 📝 Example Usage:
    // TaskButton {
    //     text: "Add Task"
    //     onClicked: console.log("Add pressed")
    // }
}
```

---

## 2. QML Page Template (`.qml`)

```qml
/*
 * 🧱 File: <PageName>.qml
 * -----------------------
 * 📌 Purpose   : [Short sentence about what this page is for]
 * 🧱 Layer     : UI (Page)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : <YYYY-MM-DD>
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable (update explicitly if Experimental/Deprecated)
 *
 * 🧠 Description:
 * - Describe the role of this Page (e.g., Task Overview, Goal Planner).
 * - Mention navigation role (entry, child page, modal).
 *
 * 🔗 Depends On:
 *   - List all QML components, pages, or C++ classes that this file requires to function (write `None` if not applicable).
 * 🔗 Related   : [Cross-module related files]
 * 🎨 Style     : Uses ThemeManager for colors, fonts, icons.
 * ✅ Tested In : [Test QML file if available]
 * ♿ Accessibility: Keyboard [Yes/No], Screen Reader [Yes/No]
 * ⚡ Performance : [Notes if page has heavy models/loaders]
 * 🕒 Last Major Change: <YYYY-MM-DD>
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: root
    title: qsTr("Page Title")

    // 👉 Custom Properties
    property string pageState: "Ready"   // Page state tracking

    // 👉 Layout Section
    ColumnLayout {
        anchors.fill: parent

        // Example component
        Label {
            text: qsTr("Hello Tasqly Page")
            Layout.alignment: Qt.AlignHCenter
        }
    }

    // 👉 Behavior / Navigation
    Component.onCompleted: {
        console.log("Page loaded:", root.title)
    }

    // 📝 Example Usage (must use NavigationService):
    // NavigationService.push("TaskPage.qml")
}
```

---

## 3. QML Style Guide

### Imports
- Always import in this order:
  1. QtQuick (base)  
  2. QtQuick.Controls  
  3. QtQuick.Layouts  
  4. Project-specific imports (`Tasqly.Ui.Components`)  

### IDs & Naming
- Root element must always have `id: root`.  
- Use descriptive IDs: `mainLayout`, `taskListView`, `addButton`.  
- Avoid generic IDs like `item1`, `rect2`.  

### Properties & Signals
- Define **public API** clearly at the top of the file.  
- Add short comments above each property/signal.  

### Layout & Anchors
- Prefer **Layout objects** (ColumnLayout, RowLayout).  
- Avoid absolute positioning unless necessary.  

### Styling & Theming
- ❌ Do NOT hardcode colors, fonts, or sizes.  
- ✅ Always use `ThemeManager.palette` and style constants.  

### Comments & Sections
- Divide file into clear sections with comments:  
  `// 👉 Custom Properties`, `// 👉 Layout Section`, `// 👉 Behavior Section`.  

### Accessibility
- All interactive elements must support keyboard navigation.  
- Provide `Accessible.name` for screen readers.  

### Performance
- Avoid complex bindings inside `Repeater` or `ListView`.  
- Use `Loader` for heavy components.  
- Profile if a page feels slow.  

### File Naming
- **Pages**: PascalCase + `Page` suffix (e.g., `TaskOverviewPage.qml`).  
- **Components**: PascalCase descriptive name (e.g., `TaskButton.qml`).  
- File length must not exceed **400 lines** → split into smaller files.  

### Best Practices
- No inline JavaScript longer than 5 LOC (move to `.js` helpers).  
- Avoid `console.log` in production code.  
- Components must follow SRP (single responsibility principle).  

---

## 4. QML Testing Template (`.qml`)

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
 * 🔗 Related   : [Cross-module related files]
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

### Guidelines
- Every **QML Component/Page must have at least one test case**.  
- Unit tests for reusable components (properties, signals).  
- Snapshot tests for verifying visual states.  
- Integration tests for multi-component interactions.  
- Test file naming: `<ComponentName>Test.qml`.  

---

## 🔗 References
- [PROJECT_QUALITY_POLICY.md](../PROJECT_QUALITY_POLICY.md)  

---

## ✅ Maintenance
- Review this document at every **Phase end**.  
- Update when new QML standards, style rules, or testing practices are introduced.  

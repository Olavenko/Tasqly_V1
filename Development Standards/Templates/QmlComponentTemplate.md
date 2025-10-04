# 🧩 QML Component Template (`.qml`)

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
 *   - All dependencies must be written explicitly, not just as an example.
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
    property alias text: label.text   // Public API

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
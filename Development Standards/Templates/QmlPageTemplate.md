# 📄 QML Page Template (`.qml`)

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
 *   - All dependencies must be written explicitly, not just as an example.
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
// 👉 Add project-specific imports (e.g., Tasqly.Ui.Components)

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

    // 📝 Example Usage:
    // NavigationService.push("TaskPage.qml")
}
```
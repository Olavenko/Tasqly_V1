/*
 * 🧱 File: Sidebar.qml
 * --------------------
 * 📌 Purpose   : Sidebar navigation menu integrated with NavigateTo usecase.
 * 🧱 Layer     : UI (QML Frontend)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-06
 * 🗓️ Updated   : 2025-09-06
 *
 * 🧠 Description:
 * Provides a minimal sidebar with navigation items.
 * Delegates navigation requests to NavigateTo usecase (C++).
 * Integrated with ThemeManager for consistent look & feel.
 */

import QtQuick 6.9
import QtQuick.Controls 6.9
import QtQuick.Layouts 6.9
import Tasqly_V1 1.0

Item {
    id: root
    Layout.fillHeight: true
    Layout.preferredWidth: sidebarWidth

    // 🎛️ Exposed: list of routes (stubbed for now)
    property var routes: [
        { id: "dashboard", title: "Dashboard", source: "qrc:/Tasqly_V1/pages/DashboardPage.qml" },
        { id: "settings",  title: "Settings",  source: "qrc:/Tasqly_V1/pages/SettingsPage.qml" },
        { id: "broken",    title: "BrokenRoute", source: "" } // 🧪 Invalid route for manual test
    ]

    // 🪝 required: enforced by the engine & visible to the code model
    required property NavigateTo navigateTo

    // 📦 Sidebar width
    property int sidebarWidth: 200

    // 📌 Sidebar menu layout
    Rectangle {
        anchors.fill: parent
        color: ThemeManager.currentTheme === "dark" ? "#333" : "#eee"
        border.color: "#aaa"

        ListView {
            id: navList
            anchors.fill: parent
            model: routes

            delegate: ItemDelegate {
                width: parent.width
                text: modelData.title
                onClicked: {
                    if (navigateTo) {
                        navigateTo.execute({ id: modelData.id, timestamp: new Date() })
                    }
                }
            }
        }
    }
}

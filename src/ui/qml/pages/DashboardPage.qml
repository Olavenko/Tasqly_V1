/*
 * 🧱 File: DashboardPage.qml
 * --------------------------
 * 📌 Purpose   : Stub page for Dashboard navigation target.
 * 🧱 Layer     : UI (QML Frontend)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-06
 * 🔖 Version   : 1.1 (Qt6 migration + unified module import)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Simple placeholder page representing the Dashboard.
 * Used to test Sidebar navigation and StackView integration.
 *
 * 🎨 Style     : Basic centered label, inherits Theme defaults
 * ✅ Tested In : Manual navigation demo via Sidebar
 * 🕒 Last Major Change: 2025-09-13 (Qt6 migration + module import cleanup)
 */


import QtQuick 6.9
import QtQuick.Controls 6.9
import Tasqly_V1 1.0

Page {
    id: dashboard
    anchors.fill: parent

    Label {
        anchors.centerIn: parent
        text: "📊 Dashboard Page (Stub)"
        font.pixelSize: 20
    }
}

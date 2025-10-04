/*
 * 🧱 File: Main.qml
 * -----------------
 * 📌 Purpose   : Root application window with Sidebar + SmartPageLoader navigation.
 * 🧱 Layer     : UI (QML Frontend)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-04
 * 🗓️ Updated   : 2025-09-13
 *
 * 🧠 Description:
 * Root window that manages navigation between app pages.
 * - Uses Sidebar for navigation menu.
 * - Delegates navigation to NavigateTo use case (C++ singleton).
 * - Displays pages via SmartPageLoader with state awareness.
 * - Supports retry on errors via preserved context.
 */

import QtQuick 6.9
import QtQuick.Controls 6.9
import QtQuick.Layouts 6.9
import Tasqly_V1 1.0

ApplicationWindow {
    id: window
    width: 800
    height: 600
    visible: true
    title: qsTr("Tasqly")

    RowLayout {
        anchors.fill: parent

        // 📌 Sidebar navigation menu
        Sidebar {
            id: sidebar
            Layout.preferredWidth: 200
            navigateTo: NavigateTo   // ✅ usecase singleton from C++
        }

        // 📌 Main content area with SmartPageLoader
        SmartPageLoader {
            id: loader
            Layout.fillWidth: true
            Layout.fillHeight: true
            pageState: PageState.Ready   // default state
            onRetryRequested: NavigateTo.retry()

            // ✅ react to NavigateTo signals
            Connections {
                target: NavigateTo
                function onPageStateChanged(state) {
                    loader.pageState = state
                }
            }
        }
    }
}

/*
 * 🧱 File: SmartFallback.qml
 * --------------------------
 * 📌 Purpose   : Fallback component for error states with retry support & notifier.
 * 🧱 Layer     : UI (QML Component)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-05
 * 🔖 Version   : 1.1 (Qt6 migration + unified imports)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Displays an error fallback UI with retry button.
 * Integrates with Notifier (toast) to show error messages.
 * Designed to be embedded inside SmartPageLoader or other loaders.
 *
 * 🔗 Depends:
 *   - Notifier (C++ facade via QmlSingletonProvider)
 *   - SmartPageLoader (parent component integration)
 *
 * 🎨 Style     : Label + Button with Theme defaults, toast for notifications
 * ✅ Tested In : Manual demo via SmartPageLoader error state
 * 🕒 Last Major Change: 2025-09-13 (Qt6 migration + module import update)
 */

import QtQuick 6.9
import QtQuick.Controls 6.9
import Tasqly_V1 1.0

Item {
    id: root
    anchors.fill: parent

    // 🎛️ Input: error message text
    property string errorMessage: "Something went wrong"

    // 🪝 Retry signal to be connected by parent (e.g., SmartPageLoader)
    signal retry()

    Column {
        anchors.centerIn: parent
        spacing: 12

        Label {
            id: messageLabel
            text: errorMessage
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 16
        }

        Button {
            text: "Retry"
            onClicked: {
                // 🔁 Notify parent for retry
                root.retry()
                // 🔔 Show toast via Notifier system
                Notifier.show("🔁 Retrying last action...", "info")
            }
        }
    }

    // 🔔 Auto-toast when error is displayed
    Component.onCompleted: {
        Notifier.show("❌ " + errorMessage, "error")
    }
}

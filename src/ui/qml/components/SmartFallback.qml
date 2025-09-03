/*
 * 🧱 File: SmartFallback.qml
 * --------------------------
 * 📌 Purpose   : Placeholder visuals for generic failure or error fallback in the UI.
 * 🧱 Layer     : UI (QML Frontend)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-02
 *
 * 🧠 Description:
 * Provides a simple and reusable visual component that appears when content
 * fails to load (e.g., due to network error or unknown issue). Designed to be
 * lightweight and non-blocking. Can be extended later with retry logic or
 * advanced loaders (Phase 2.5).
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root
    anchors.fill: parent

    property string message: qsTr("⚠️ Oops! Something went wrong.")
    property bool retryVisible: false
    signal retryRequested()

    Rectangle {
        anchors.centerIn: parent
        width: parent.width * 0.6
        height: parent.height * 0.4
        radius: 12
        color: Qt.rgba(0.15, 0.15, 0.15, 0.85)
        border.color: "#666"
        border.width: 1

        Column {
            anchors.centerIn: parent
            spacing: 12

            Text {
                text: root.message
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 16
                color: "white"
            }

            Button {
                visible: root.retryVisible
                text: qsTr("Retry")
                onClicked: root.retryRequested()
            }
        }
    }
}

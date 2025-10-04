/*
 * 🧱 File: SettingsPage.qml
 * -------------------------
 * 📌 Purpose   : Application settings page (theme toggle).
 * 🧱 Layer     : UI (QML Frontend)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-05
 * 🔖 Version   : 1.2 (Qt6 migration + consistent module import)
 * 🛡️ Stability : Stable (with planned refactor for setThemeMode API)
 *
 * 🧠 Description:
 * Provides a simple UI to toggle between Light/Dark themes at runtime.
 * Binds directly to App.Theme (QmlSingletonProvider).
 * Palette colors are applied to demonstrate runtime switching.
 *
 * 🔗 Depends:
 *   - Theme (C++ facade via QmlSingletonProvider)
 *   - ThemeMode (domain enum exposed to QML)
 *   - Settings (C++ facade via QmlSingletonProvider)
 *
 * 🎨 Style     : Theme.palette (background, text, primary)
 * ✅ Tested In : Manual theme toggle demo + persistence validation
 * 🕒 Last Major Change: 2025-09-13 (Qt6 migration + unified imports)
 */


import QtQuick 6.9
import QtQuick.Controls 6.9
import Tasqly_V1 1.0

Page {
    id: root
    title: qsTr("Settings")

    Column {
        anchors.centerIn: parent
        spacing: 20

        Label {
            text: qsTr("Theme Mode")
            font.pixelSize: 18
            color: Theme.palette.text
        }

        Switch {
            id: themeSwitch
            checked: Theme.mode === ThemeMode.Dark
            text: checked ? qsTr("Dark") : qsTr("Light")

            onToggled: {
                Theme.switchTo(
                    checked ? ThemeMode.Dark : ThemeMode.Light
                )
                Settings.setThemeMode(
                    checked ? ThemeMode.Dark : ThemeMode.Light
                )
                Settings.save()
            }
        }

        Rectangle {
            width: 200; height: 100
            radius: 8
            color: Theme.palette.background
            border.color: Theme.palette.primary
            Text {
                anchors.centerIn: parent
                text: qsTr("Preview")
                color: Theme.palette.text
            }
        }
    }
}

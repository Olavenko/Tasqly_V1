/*
 * 🧱 File: TaskListPage.qml
 * -------------------------
 * 📌 Purpose   : Page to list tasks with loading/error/empty states.
 * 🧱 Layer     : UI (Page)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 * 🔖 Version   : 1.2 (Qt6 migration + updated imports for components/feedback)
 * 🛡️ Stability : Experimental
 *
 * 🧠 Description:
 * - Displays a paginated list of tasks.
 * - Integrates with SmartPageLoader for loading/error/empty handling.
 * - Applies ThemeManager palette & fonts for styling.
 * - Shows Notifier toasts for info and error states.
 * - Logs errors via LogManager for diagnostics.
 * - Exposes current theme mode, palette, and fonts as QML-friendly properties.
 * - Supports switching between Light/Dark and notifies QML about updates.
 * - Provides static font tokens (heading/body/caption) for consistent typography.
 *
 * 🔗 Depends:
 *   - SmartPageLoader (src/ui/qml/components)
 *   - SmartFallback   (src/ui/qml/components)
 *   - ThemeManager (C++ facade via QmlSingletonProvider)
 *   - Notifier (C++ facade via QmlSingletonProvider)
 *   - Toast (src/ui/qml/components/feedback)
 *   - ListTasksUC (C++ use case)
 *   - InitRepositories (C++ use case)
 *   - Task / ITaskRepository (domain entities & repos)
 *   - AppContext (app core)
 *   - InMemoryTaskRepository (infra)
 *   - AppLoadingManager (app core)
 *   - SystemClock / SystemUuidGen (infra utils)
 *   - LogManager (infra logging)
 *
 * 🎨 Style     : ThemeManager.palette (background, text, secondary, error, transparent) + ThemeManager.fonts
 * ✅ Tested In : Manual demo with in-memory repositories (TODO: add QtTest snapshot test)
 * ♿ Accessibility: Accessible names provided for task items
 * ⚡ Performance : Delegates are lightweight, model bound via ListTasksUC
 * 🕒 Last Major Change: 2025-09-13 (Qt6 migration + import path updates)
 */


import QtQuick 6.9
import QtQuick.Controls 6.9
import QtQuick.Layouts 6.9
import Tasqly_V1 1.0

// 🧩 Local QML imports
import "src/ui/qml/components"
import "src/ui/qml/components/feedback"

Page {
    id: root
    title: qsTr("Tasks")

    property int limit: 20
    property int offset: 0
    property var tasksModel: []

    Rectangle {
        anchors.fill: parent
        color: Theme.palette.background

        SmartPageLoader {
            id: loader
            anchors.fill: parent

            pageState: AppLoadingManager.repositoriesReady ? PageState.Ready : PageState.Loading
            pageSource: listViewComponent

            onRetryRequested: {
                Notifier.show(qsTr("🔁 Retrying task list..."), "info")
                root.loadTasks()
            }
        }
    }

    Component {
        id: listViewComponent
        Item {
            anchors.fill: parent

            ListView {
                id: taskListView
                anchors.fill: parent
                spacing: 8
                clip: true
                model: root.tasksModel

                delegate: Rectangle {
                    width: taskListView.width
                    height: 56
                    color: Theme.palette.transparent

                    Accessible.role: Accessible.ListItem
                    Accessible.name: modelData.title + ", " + modelData.status

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 16

                        Label {
                            text: modelData.title
                            color: Theme.palette.text
                            font.pixelSize: Theme.fonts.body
                            Layout.fillWidth: true
                        }

                        Label {
                            text: modelData.status
                            color: Theme.palette.secondary
                            font.pixelSize: Theme.fonts.caption
                        }

                        Label {
                            text: modelData.createdAt.toString("yyyy-MM-dd hh:mm")
                            color: Theme.palette.secondary
                            font.pixelSize: Theme.fonts.caption
                        }
                    }
                }
            }
        }
    }

    function loadTasks() {
        try {
            var result = ListTasksUC.run(limit, offset)
            if (result.isOk) {
                tasksModel = result.value
                if (tasksModel.length === 0) {
                    loader.pageState = PageState.Empty
                } else {
                    loader.pageState = PageState.Ready
                    Notifier.show(qsTr("✅ Loaded %1 tasks").arg(tasksModel.length), "info")
                }
            } else {
                loader.pageState = PageState.Error
                Notifier.show(qsTr("❌ Failed to load tasks: %1").arg(result.error.message), "error")
                LogManager.logError("TaskListPage: loadTasks failed → " + result.error.message)
            }
        } catch (e) {
            loader.pageState = PageState.Error
            Notifier.show(qsTr("💥 Exception: %1").arg(e), "critical")
            LogManager.logError("TaskListPage exception: " + e)
        }
    }

    Connections {
        target: AppLoadingManager
        function onRepositoriesReadyChanged(ready) {
            if (ready) {
                loader.pageState = PageState.Loading
                root.loadTasks()
            }
        }
    }
}

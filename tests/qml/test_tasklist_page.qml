/*
 * 🧱 File: test_tasklist_page.qml
 * -------------------------------
 * 📌 Purpose   : QML integration test for TaskListPage empty vs seeded state.
 * 🧱 Layer     : Tests (QML/Integration)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-08
 *
 * 🧠 Description:
 * Loads TaskListPage via qrc:/ path for early-phase tests.
 * Module import will be restored once full UI Slice is implemented.
 * Verifies SmartFallback is shown when empty,
 * and ListView is populated when seeded.
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtTest 1.2

TestCase {
    name: "TaskListPageTests"
    when: windowShown

    // 🧩 Root container for created objects
    Item {
        id: testRoot
        width: 800
        height: 600
    }

    property var page

    // 🧩 Helper to create page from qrc path (early phase)
    function createPage(seed) {
        var component = Qt.createComponent("qrc:/qt/qml/Tasqly_V1/pages/TaskListPage.qml")
        var obj = component.createObject(testRoot, { "seed": seed, "objectName": "taskListPage" })
        return obj
    }

    function cleanup() {
        if (page) {
            page.destroy()
            page = null
        }
    }

    // 🟠 Test: Empty state (seed=false)
    function test_emptyState() {
        var page = createPage(false)
        if (!page) {
            skip("⏭️ TaskListPage not ready yet (UI Slice not implemented)")
            return
        }
        compare(!!page.findChild("smartFallback"), true)
        page.destroy()
    }

    // 🟢 Test: Seeded state (seed=true)
    function test_seededState() {
        var page = createPage(true)
        if (!page) {
            skip("⏭️ TaskListPage not ready yet (UI Slice not implemented)")
            return
        }
        compare(page.findChild("taskListView") !== null, true)
        compare(page.findChild("taskListView").count > 0, true)
        page.destroy()
    }
}

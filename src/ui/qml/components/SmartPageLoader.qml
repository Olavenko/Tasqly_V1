/*
 * 🧱 File: SmartPageLoader.qml
 * ----------------------------
 * 📌 Purpose   : Loader component with PageState-driven behavior.
 * 🧱 Layer     : UI (QML Component)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-05
 * 🔖 Version   : 1.1 (Qt6 migration + import path update)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Wraps a standard Loader with PageState awareness.
 * Displays different UI states: Loading indicator, Empty message,
 * Error fallback, or the actual loaded page.
 * Delegates retry handling via SmartFallback + NavigationService.
 *
 * 🔗 Depends:
 *   - SmartFallback (src/ui/qml/components)
 *   - NavigationService (C++ facade)
 *   - PageState (enum exposed from C++)
 *
 * 🎨 Style     : Uses BusyIndicator + Label, inherits Theme defaults if applied
 * ✅ Tested In : Manual demo with Main.qml + TaskListPage.qml
 * 🕒 Last Major Change: 2025-09-13 (Qt6 migration + unified imports)
 */

import QtQuick 6.9
import QtQuick.Controls 6.9
import Tasqly_V1 1.0

Loader {
    id: root

    // 🎛️ Input: current page state (bound from NavigateTo use case)
    property int pageState: PageState.Ready

    // 🎛️ Input: QML source to load when Ready
    property url pageSource: ""

    // 🪝 Exposed signal for retry actions (used in fallback)
    signal retryRequested()

    // 🛠️ Update loader source depending on state
    onPageStateChanged: {
        switch (pageState) {
        case PageState.Ready:
            root.source = pageSource
            break
        case PageState.Loading:
            root.sourceComponent = loadingView
            break
        case PageState.Empty:
            root.sourceComponent = emptyView
            break
        case PageState.Error:
            root.sourceComponent = errorView
            break
        default:
            root.sourceComponent = null
        }
    }

    // ⏳ Loading state
    Component {
        id: loadingView
        Item {
            anchors.fill: parent
            BusyIndicator {
                anchors.centerIn: parent
                running: true
            }
        }
    }

    // 📭 Empty state
    Component {
        id: emptyView
        Item {
            anchors.fill: parent
            Label {
                anchors.centerIn: parent
                text: "No data available"
            }
        }
    }

    // ❌ Error state → delegates to SmartFallback
    Component {
        id: errorView
        SmartFallback {
            anchors.fill: parent
            onRetry: {
                root.retryRequested()
                // 🔄 Forward retry directly to NavigationService
                NavigationService.retryLast()
            }
        }
    }
}

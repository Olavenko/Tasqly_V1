/*
 * 🧱 File: Toast.qml
 * -------------------
 * 📌 Purpose   : UI toast notifier with queue, severity styling, auto-dismiss, and rate-limiting
 * 🧱 Layer     : UI (QML Frontend)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-02
 *
 * 🧠 Description:
 * A lightweight toast component to display transient messages at the bottom of the window.
 * Features:
 *  - FIFO queue with single active toast
 *  - Severity levels (Info/Warning/Error/Critical) with visual accents
 *  - Auto-dismiss durations per severity + manual close
 *  - Rate-limit identical messages to avoid spam; optional duplicate merge with counter (xN)
 *  - Public API:
 *      • enqueue({ severity, message, context, durationMs })
 *      • bindToReporter(reporter) → connects to ErrorReporter.toastRequested(sev, msg, ctx)
 *
 * Notes:
 *  - No dependency on QtQuick.Controls to keep runtime minimal.
 *  - Colors are overridable via properties; integrate later with ThemeManager if desired.
 */

import QtQuick 6.5

Item {
    id: toastHost
    anchors {
        left: parent ? parent.left : undefined
        right: parent ? parent.right : undefined
        bottom: parent ? parent.bottom : undefined
        margins: 16
    }
    height: container.implicitHeight
    visible: _active !== null
    z: 9999
    focus: false
    clip: false

    // 🧰 Public configuration
    property int   maxQueue: 5                          // drop extras beyond this
    property bool  enableRateLimit: true
    property int   rateLimitMs: 800
    property bool  mergeDuplicates: true
    property int   basePadding: 12
    property int   baseRadius: 12
    property real  baseOpacity: 0.98
    property int   elevation: 24

    // 🎨 Colors (override from outside / ThemeManager later)
    property color colorBg:    "#202225"
    property color colorFg:    "#FFFFFF"
    property color colorInfo:  "#2d8cff"
    property color colorWarn:  "#ffb020"
    property color colorError: "#e03131"
    property color colorCrit:  "#ad1d1d"

    // ⏱️ Durations per severity (ms)
    property int durInfo: 2500
    property int durWarn: 3500
    property int durError: 5000
    property int durCrit: 7000

    // 🔢 Severity enum mirror (matches ErrorReporter.Severity)
    // 0=Info, 1=Warning, 2=Error, 3=Critical
    readonly property var levels: ({ Info: 0, Warning: 1, Error: 2, Critical: 3 })

    // 🧳 Internal queue + active item
    property var _queue: []    // array of {severity, message, context, durationMs, count, t}
    property var _active: null // current toast object from queue head
    property double _lastTs: 0
    property string _lastKey: ""

    // ➕ API: connect to ErrorReporter (signals: toastRequested(int sev, QString msg, QVariantMap ctx))
    // ➕ Binds once; safe to call repeatedly (guarded)
    function bindToReporter(reporter) { // ➕ expose bind helper
        if (!reporter || reporter.__bound_to_toast__) return
        reporter.toastRequested.connect(function(sev, msg, ctx) {
            enqueue({ severity: sev, message: msg, context: ctx })
        })
        reporter.__bound_to_toast__ = true
    }

    // ➕ API: enqueue new toast
    function enqueue(opts) {
        // Normalize
        var sev = (opts && opts.severity !== undefined) ? opts.severity : levels.Info
        var msg = (opts && opts.message) ? String(opts.message) : "Unknown error"
        var ctx = (opts && opts.context) ? opts.context : ({})
        var dur = (opts && opts.durationMs) ? opts.durationMs :
                        (sev === levels.Info ? durInfo :
                        sev === levels.Warning ? durWarn :
                        sev === levels.Error ? durError : durCrit)

        // 🪵 Rate limit identical messages
        var key = sev + "|" + msg
        var now = Date.now()
        if (enableRateLimit && _lastKey === key && (now - _lastTs) < rateLimitMs) {
            if (mergeDuplicates) {
                // If active is same key, bump counter; else bump last in queue if same
                if (_active && (_active.key === key)) {
                    _active.count += 1
                    banner.count = _active.count
                } else if (_queue.length > 0 && _queue[_queue.length - 1].key === key) {
                    _queue[_queue.length - 1].count += 1
                }
            }
            return
        }
        _lastKey = key
        _lastTs  = now

        // Push to queue
        var item = { severity: sev, message: msg, context: ctx, durationMs: dur, count: 1, key: key, t: now }
        _queue.push(item)
        // Drop overflow (oldest tail beyond maxQueue)
        if (_queue.length > maxQueue) {
            _queue.splice(maxQueue, _queue.length - maxQueue)
        }
        if (_active === null) {
            _startNext()
        }
    }

    // 🔁 Advance the queue
    function _startNext() {
        if (_queue.length === 0) {
            _active = null
            return
        }
        _active = _queue.shift()
        // Feed UI
        banner.visible = true
        banner.opacity = 0.0
        banner.message = _active.message
        banner.severity = _active.severity
        banner.count = _active.count
        banner.show()
    }

    // 🧱 Visual container
    Item {
        id: container
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
        }
        width: Math.min(parent ? parent.width - 32 : 600, 520)
        height: banner.implicitHeight
    }

    // 🎭 Single active toast banner
    Item {
        id: banner
        property alias message: textMessage.text
        property int severity: levels.Info
        property int count: 1

        // ➕ API: show current active toast
        function show() { // ➕ animate in and arm timer
            // choose color and duration
            chip.color = (severity === levels.Info ? colorInfo :
                          severity === levels.Warning ? colorWarn :
                          severity === levels.Error ? colorError : colorCrit)
            // Auto-dismiss (critical still auto-dismisses after durCrit; adjust if needed)
            dismissTimer.interval = Math.max(1200, (_active ? _active.durationMs : durInfo))
            dismissTimer.restart()
            animIn.start()
        }

        // ➕ API: close current toast and advance
        function closeNow() {
            dismissTimer.stop()
            animOut.running = true
        }

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        opacity: 0.0
        visible: false

        readonly property int pad: toastHost.basePadding
        readonly property int radius: toastHost.baseRadius

        implicitHeight: box.implicitHeight

        // 🧱 Background box
        Rectangle {
            id: box
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            color: colorBg
            radius: banner.radius
            opacity: baseOpacity
            layer.enabled: true
            layer.samples: 4
            border.color: chip.color
            border.width: 1

            // Shadow-like spacer (cheap)
            anchors.margins: 0
            width: container.width
            implicitHeight: contentRow.implicitHeight + pad * 2

            // Content
            Row {
                id: contentRow
                spacing: 10
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                    leftMargin: pad
                    rightMargin: pad
                    bottomMargin: pad
                    top: parent.top
                    topMargin: pad
                }

                // Severity chip
                Rectangle {
                    id: chip
                    width: 10
                    radius: 5
                    color: colorInfo
                    anchors.verticalCenter: parent.verticalCenter
                    height: Math.max(10, textMessage.implicitHeight)
                }

                // Message text + (xN)
                Text {
                    id: textMessage
                    text: banner.count > 1 ? (banner.message + " (x" + banner.count + ")") : banner.message
                    color: colorFg
                    wrapMode: Text.Wrap
                    elide: Text.ElideRight
                    font.pixelSize: 14
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                    anchors.verticalCenter: parent.verticalCenter
                    width: Math.max(0, parent.width - (chip.width + closeBtn.width + 40))
                }

                // Close button (minimal, no Controls)
                MouseArea {
                    id: closeBtn
                    anchors.verticalCenter: parent.verticalCenter
                    width: 28; height: 28
                    hoverEnabled: true
                    onClicked: banner.closeNow()

                    // Visual "X"
                    Canvas {
                        anchors.fill: parent
                        onPaint: {
                            var ctx = getContext("2d")
                            ctx.reset()
                            ctx.lineWidth = 2
                            ctx.strokeStyle = colorFg
                            var m = 8
                            ctx.beginPath()
                            ctx.moveTo(m, m)
                            ctx.lineTo(width - m, height - m)
                            ctx.moveTo(width - m, m)
                            ctx.lineTo(m, height - m)
                            ctx.stroke()
                        }
                    }
                }
            }
        }

        // ⏱️ Auto dismiss
        Timer {
            id: dismissTimer
            repeat: false
            running: false
            onTriggered: animOut.start()
        }

        // 🎞️ Animations
        NumberAnimation {
            id: animIn
            target: banner
            property: "opacity"
            from: 0.0; to: 1.0
            duration: 160
            easing.type: Easing.OutQuad
        }

        SequentialAnimation {
            id: animOut
            PropertyAnimation { target: banner; property: "opacity"; to: 0.0; duration: 140; easing.type: Easing.InQuad }
            ScriptAction {
                script: {
                    banner.visible = false
                    // Advance queue
                    toastHost._active = null
                    toastHost._startNext()
                }
            }
        }
    }
}

/*
 * 🧱 File: Notifier.cpp
 * ---------------------
 * 📌 Purpose   : Implementation of Notifier subsystem with QML integration.
 * 🧱 Layer     : Application (Notifier Subsystem)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-06
 *
 * 🧠 Description:
 * Implements a singleton notifier which bridges C++ calls (`show()`)
 * to QML signals (`notify()`). Used for global toast/error notifications.
 */

#include "Notifier.h"
#include <QMetaObject>

// 🟢 Define static instance
Notifier* Notifier::s_instance = nullptr;

// ➕ Constructor
Notifier::Notifier(QObject* parent)
    : QObject(parent)
{
  // Register self as singleton instance if not already
  if (!s_instance) {
    s_instance = this;
  }
}

// 🎯 Static entrypoint → forwards to singleton instance
void Notifier::show(const QString& message, const QString& level)
{
  if (s_instance) {
    // 🪢 Use queued connection to stay thread-safe
    QMetaObject::invokeMethod(
        s_instance, [=]() { emit s_instance->notify(message, level); }, Qt::QueuedConnection);
  }
}

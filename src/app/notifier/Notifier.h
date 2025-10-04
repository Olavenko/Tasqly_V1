/*
 * 🧱 File: Notifier.h
 * -------------------
 * 📌 Purpose   : Facade for user notifications (toast messages).
 * 🧱 Layer     : Application (Notifier Subsystem)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-06
 *
 * 🧠 Description:
 * Provides a simple interface to dispatch notifications from C++ core
 * to QML UI (Toast / SmartFallback). Exposes a `show()` static helper
 * and emits `notify()` signals consumed by QML layer.
 */

#pragma once

#include <QObject>
#include <QString>

// ➡️ Provides notification API (C++ → QML)
class Notifier : public QObject
{
  Q_OBJECT

public:
  explicit Notifier(QObject* parent = nullptr);

  // 🎯 Static entrypoint to show toast messages (thread-safe via signal)
  Q_INVOKABLE static void show(const QString& message, const QString& level = "info");

  // ✅ Accessor for singleton instance (for tests & internal use)
  static Notifier* instance() { return s_instance; }

  // 🧹 Reset API for tests (clears singleton safely)
  static void resetInstance() { s_instance = nullptr; }

signals:
  // 🔔 Signal emitted to QML → handled by Toast/SmartFallback
  void notify(const QString& message, const QString& level);

private:
  // ⚙️ Singleton instance (lifetime = app / test fixture)
  static Notifier* s_instance;
};

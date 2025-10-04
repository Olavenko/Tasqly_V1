/*
 * 🧱 File: ErrorReporter.h
 * ------------------------
 * 📌 Purpose   : Application use-case to report errors: map severity → log level,
 *                delegate to LogManager, and emit a UI toast event.
 * 🧱 Layer     : Application (Logging / Error Handling)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-02
 *
 * 🧠 Description:
 * Bridges UI/QML and logging backends. Accepts Error data, maps severity to
 * logging Level, logs via ILogManager, and emits a toastRequested(...) signal
 * for the UI (Notifier/Toast component handles queuing & rate-limiting).
 *
 * 🔒 Note:
 *  `mapSeverityToLevel()` and `merge()` were made public static helpers.
 *  Reason: they are part of the explicit ErrorReporter contract
 *  (used across subsystems + tested independently).
 *  This aligns with Testing Standards and Coverage Policy.
 */

#pragma once

#include "ILogManager.h"
#include "domain/core/Error.h"
#include <memory>
#include <QObject>
#include <QString>
#include <QVariantMap>

namespace tasqly::app::logging {

// 🎛️ UI-facing severity (decoupled from log levels)
enum class Severity : int { Info = 0, Warning = 1, Error = 2, Critical = 3 };

class ErrorReporter final : public QObject
{
  Q_OBJECT

public:
  // 🧱 Ctor DI: inject ILogManager
  explicit ErrorReporter(std::shared_ptr<ILogManager> logManager, QObject* parent = nullptr);

  // 🧰 Report using structured Error (preferred)
  Q_INVOKABLE void report(const tasqly::domain::core::Error& err,
                          Severity severity,
                          const QString& category = QStringLiteral("app"),
                          const QVariantMap& extraCtx = {});

  // 🧰 Overload: raw fields (helps QML)
  Q_INVOKABLE void reportRaw(int code,
                             const QString& message,
                             Severity severity,
                             const QString& category = QStringLiteral("app"),
                             const QVariantMap& context = {});

  // 🧭 Map UI severity → logging level (public helper for testing + contract)
  static Level mapSeverityToLevel(Severity s);

  // 🧩 Merge contexts (last-writer-wins, public helper for testing + contract)
  static QVariantMap merge(const QVariantMap& a, const QVariantMap& b);

signals:
  // 🔔 UI event for Notifier/Toast
  void toastRequested(int severity, QString message, QVariantMap context);

private:
  std::shared_ptr<ILogManager> m_log; // injected logger
  bool m_loggingEnabled;              // cache of features.logging
};

} // namespace tasqly::app::logging

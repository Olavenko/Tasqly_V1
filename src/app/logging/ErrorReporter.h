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
 */

#pragma once

// Qt
#include <QObject>
#include <QString>
#include <QVariantMap>

// STL
#include <memory>

// Domain/Core
#include "../../domain/core/Error.h"

// App interface
#include "ILogManager.h"

namespace tasqly::app::logging {

// 🎛️ UI-facing severity (decoupled from log levels)
enum class Severity : int { Info = 0, Warning = 1, Error = 2, Critical = 3 };

class ErrorReporter final : public QObject
{
  Q_OBJECT

public:
  // 🧱 Ctor DI: inject ILogManager
  // ➕ Constructs the use-case with a logging backend
  explicit ErrorReporter(std::shared_ptr<ILogManager> logManager, QObject* parent = nullptr);

  // 🧰 Report using structured Error (preferred)
  // 🔔 Emits toastRequested and logs via LogManager
  // extraCtx (optional) merges into Error.context (last-writer-wins)
  Q_INVOKABLE void report(const tasqly::domain::core::Error& err,
                          Severity severity,
                          const QString& category = QStringLiteral("app"),
                          const QVariantMap& extraCtx = {});

  // 🧰 Overload: raw fields (helps QML)
  // 📝 Normalizes empty message → "Unknown error"
  Q_INVOKABLE void reportRaw(int code,
                             const QString& message,
                             Severity severity,
                             const QString& category = QStringLiteral("app"),
                             const QVariantMap& context = {});

signals:
  // 🔔 UI event for Notifier/Toast; the UI decides queue/auto-dismiss/rate-limit
  void toastRequested(int severity, QString message, QVariantMap context);

private:
  // 🧭 Map UI severity → logging level
  static Level mapSeverityToLevel(Severity s);

  // 🧩 Merge contexts (last-writer-wins)
  static QVariantMap merge(const QVariantMap& a, const QVariantMap& b);

private:
  std::shared_ptr<ILogManager> m_log; // injected logger
};

} // namespace tasqly::app::logging

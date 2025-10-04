/*
 * 🧱 File: ErrorReporter.cpp
 * --------------------------
 * 📌 Purpose   : Implementation of ErrorReporter use-case.
 * 🧱 Layer     : Application (Logging / Error Handling)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-02
 *
 * 🧠 Description:
 * Maps Severity → Level, logs via ILogManager with merged context, and emits
 * a toastRequested signal for the UI. Delegates to LogManager & Notifier separately.
 */

#include "ErrorReporter.h"
#include "app/settings/FeatureFlagsManager.h"

using tasqly::domain::core::Error;

namespace tasqly::app::logging {

// ➕ Ctor
ErrorReporter::ErrorReporter(std::shared_ptr<ILogManager> logManager, QObject* parent)
    : QObject(parent)
    , m_log(std::move(logManager))
    , m_loggingEnabled(FeatureFlagsManager::instance().isEnabled("features.logging"))
{
  QObject::connect(&FeatureFlagsManager::instance(),
                   &FeatureFlagsManager::featureChanged,
                   this,
                   [this](const QString& key, bool value) {
                     if (key == QStringLiteral("features.logging")) {
                       m_loggingEnabled = value;
                     }
                   });
  // 🧪 Defensive: ensure logger exists (could be replaced by a NullLogger later if needed)
}

// 🧰 Report using structured Error
void ErrorReporter::report(const Error& err,
                           Severity severity,
                           const QString& category,
                           const QVariantMap& extraCtx)
{
  // 🚫 Respect feature flag: skip logging if disabled
  if (!m_loggingEnabled) {
    // 🔔 Still notify UI (toast) even if logging is off
    emit toastRequested(static_cast<int>(severity), err.message(), extraCtx);
    return;
  }

  // 🧳 Merge contexts (extra overrides existing keys)
  const QVariantMap ctx = merge(err.context(), extraCtx);

  // 🪵 Log with mapped level
  const auto lvl = mapSeverityToLevel(severity);
  if (m_log && m_log->isEnabled(lvl)) {
    m_log->log(lvl, category, err.message(), ctx);
  }

  // 🔔 Notify UI (Notifier/Toast)
  emit toastRequested(static_cast<int>(severity), err.message(), ctx);
}

// 🧰 Overload: raw fields (for QML convenience)
void ErrorReporter::reportRaw(int code,
                              const QString& message,
                              Severity severity,
                              const QString& category,
                              const QVariantMap& context)
{
  const QString normalized = message.trimmed().isEmpty() ? QStringLiteral("Unknown error")
                                                         : message;
  Error e(code, normalized, context);
  report(e, severity, category, {});
}

// 🧭 Map UI severity → logging Level
Level ErrorReporter::mapSeverityToLevel(Severity s)
{
  switch (s) {
  case Severity::Info:
    return Level::Info;
  case Severity::Warning:
    return Level::Warn;
  case Severity::Error:
    return Level::Error;
  case Severity::Critical:
    return Level::Critical;
  }
  return Level::Info;
}

// 🧩 Merge contexts (last-writer-wins)
QVariantMap ErrorReporter::merge(const QVariantMap& a, const QVariantMap& b)
{
  QVariantMap out = a;
  for (auto it = b.cbegin(); it != b.cend(); ++it) {
    out.insert(it.key(), it.value());
  }
  return out;
}

} // namespace tasqly::app::logging

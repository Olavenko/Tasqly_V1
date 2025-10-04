/*
 * 🧱 File: AppErrors.h
 * --------------------
 * 📌 Purpose   : QML-exposed registry/factory for common application errors.
 * 🧱 Layer     : Application (Errors)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-02
 *
 * 🧠 Description:
 * Provides convenient Q_INVOKABLE helpers to create standardized Error values
 * (code + message + context). Registered as a QML Singleton (AppErrors) so QML
 * can construct errors consistently and pass them to ErrorReporter.
 */

#pragma once

// Qt
#include <QObject>
#include <QString>
#include <QVariantMap>

// Domain
#include "../../domain/core/Error.h"

namespace tasqly::app::errors {

class AppErrors final : public QObject
{
  Q_OBJECT
public:
  explicit AppErrors(QObject* parent = nullptr)
      : QObject(parent)
  {}

  // ➕ Creates a generic error (useful for custom cases from QML)
  // message is normalized in Error ctor if empty
  Q_INVOKABLE tasqly::domain::core::Error make(int code,
                                               const QString& message,
                                               const QVariantMap& context = {}) const
  {
    return tasqly::domain::core::Error(code, message, context);
  }

  // 🌐 Network-related errors
  Q_INVOKABLE tasqly::domain::core::Error networkUnavailable(const QVariantMap& ctx = {}) const
  {
    return tasqly::domain::core::Error(2001, QStringLiteral("Network unavailable"), ctx);
  }

  Q_INVOKABLE tasqly::domain::core::Error httpError(int status, const QVariantMap& ctx = {}) const
  {
    QVariantMap m = ctx;
    m.insert(QStringLiteral("status"), status);
    return tasqly::domain::core::Error(2002, QStringLiteral("HTTP request failed"), m);
  }

  // ⏳ Timeouts / Retries
  Q_INVOKABLE tasqly::domain::core::Error timeout(const QVariantMap& ctx = {}) const
  {
    return tasqly::domain::core::Error(2003, QStringLiteral("Operation timed out"), ctx);
  }

  // 🧼 Validation / User input
  Q_INVOKABLE tasqly::domain::core::Error validationFailed(const QString& field,
                                                           const QString& why = {},
                                                           const QVariantMap& ctx = {}) const
  {
    QVariantMap m = ctx;
    m.insert(QStringLiteral("field"), field);
    if (!why.trimmed().isEmpty())
      m.insert(QStringLiteral("reason"), why);
    return tasqly::domain::core::Error(2004, QStringLiteral("Validation failed"), m);
  }

  // 🗄️ Database / Persistence
  Q_INVOKABLE tasqly::domain::core::Error dbUnavailable(const QVariantMap& ctx = {}) const
  {
    return tasqly::domain::core::Error(2005, QStringLiteral("Database unavailable"), ctx);
  }

  // ❓ Unknown (fallback)
  Q_INVOKABLE tasqly::domain::core::Error unknown(const QVariantMap& ctx = {}) const
  {
    return tasqly::domain::core::Error::Unknown(ctx);
  }
};

} // namespace tasqly::app::errors

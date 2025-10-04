/*
 * 🧱 File: Error.h
 * ----------------
 * 📌 Purpose   : Core error data type used across layers (domain/app/infra/ui)
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-02
 *
 * 🧠 Description:
 * A lightweight, header-only error struct that carries an error code,
 * a human-readable message, and an optional context map for extra metadata.
 * Designed for interop with Qt/QML (Q_GADGET + Q_DECLARE_METATYPE) and to be
 * used by Result<T, Error>, ErrorReporter, and LogManager.
 */

#pragma once

// Qt
#include <QMetaType>
#include <QString>
#include <QVariantMap>
#include <QtGlobal>

namespace tasqly::domain::core {

/// Core error value to be propagated with Result<T, Error> and across use-cases
class Error
{
  Q_GADGET
  Q_PROPERTY(int code READ code CONSTANT)
  Q_PROPERTY(QString message READ message CONSTANT)
  Q_PROPERTY(QVariantMap context READ context CONSTANT)

public:
  // 🧱 Constants
  static constexpr int kUnknownCode = 0;

  // ➕ Default-constructs an unknown error (code=0, message="Unknown error")
  Error() noexcept
      : m_code(kUnknownCode)
      , m_message(QStringLiteral("Unknown error"))
  {}

  // ➕ Constructs with code and message; normalizes empty message to "Unknown error"
  Error(int code, QString message, QVariantMap context = {}) noexcept
      : m_code(code)
      , m_message(message.trimmed().isEmpty() ? QStringLiteral("Unknown error") : std::move(message))
      , m_context(std::move(context))
  {}

  // ℹ️ Getters
  // 🔎 Returns the numeric error code
  int code() const noexcept { return m_code; }

  // 🗒️ Returns the human-readable error message
  const QString& message() const noexcept { return m_message; }

  // 🧳 Returns structured metadata attached to the error
  const QVariantMap& context() const noexcept { return m_context; }

  // 🧪 Utility
  // 🧵 toString for logging and debugging
  QString toString() const
  {
    return QStringLiteral("Error{code=%1, message=\"%2\", context=%3 item(s)}")
        .arg(QString::number(m_code), m_message, QString::number(m_context.size()));
  }

  // 🧩 Returns a copy with additional/overridden context key-values
  Error withContext(QVariantMap extra) const
  {
    QVariantMap merged = m_context;
    for (auto it = extra.cbegin(); it != extra.cend(); ++it) {
      merged.insert(it.key(), it.value());
    }
    return Error{m_code, m_message, std::move(merged)};
  }

  // 🧯 Factory: unknown error (optionally with context)
  static Error Unknown(QVariantMap ctx = {})
  {
    return Error{kUnknownCode, QStringLiteral("Unknown error"), std::move(ctx)};
  }

  // 🔁 Equality operators (useful for tests)
  friend bool operator==(const Error& a, const Error& b)
  {
    return a.m_code == b.m_code && a.m_message == b.m_message && a.m_context == b.m_context;
  }
  friend bool operator!=(const Error& a, const Error& b) { return !(a == b); }

private:
  // 🧱 Data
  int m_code;
  QString m_message;
  QVariantMap m_context;
};

// 📌 Repository error codes for persistence consistency
enum class RepoErrorCode {
  NotFound = 404,      // entity not found
  AlreadyExists = 409, // conflict on insert
  StorageFailure = 500 // internal failure
};

} // namespace tasqly::domain::core

Q_DECLARE_METATYPE(tasqly::domain::core::Error)

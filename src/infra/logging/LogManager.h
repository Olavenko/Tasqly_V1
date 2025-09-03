/*
 * 🧱 File: LogManager.h
 * ---------------------
 * 📌 Purpose   : Concrete logging backend (infra) implementing ILogManager.
 * 🧱 Layer     : Infrastructure (Logging)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-02
 * 🗓️ Updated   : 2025-09-03
 *
 * 🧠 Description:
 * Provides console logging and rotating file logging with configurable level,
 * sensitive-field masking (by key), and best-effort fallbacks to console when
 * file I/O fails (including disk-full during rotation). Thread-safe via mutex.
 */

#pragma once

// Qt
#include <QFile>
#include <QMutex>
#include <QString>
#include <QStringList>
#include <QVariantMap>

// App interface
#include "../../app/logging/ILogManager.h"

namespace tasqly::infra::logging {

class LogManager final : public tasqly::app::logging::ILogManager
{
public:
  using Level = tasqly::app::logging::Level;
  using RotationPolicy = tasqly::app::logging::RotationPolicy;

  // 🧱 Ctors/Dtor
  LogManager();
  ~LogManager() override;

  // 🎚️ Level configuration
  void setLevel(Level level) override;
  Level level() const override;
  bool isEnabled(Level level) const override;

  // 🖥️ Console sink
  void enableConsole(bool enabled) override;

  // 💾 File sink
  void enableFileSink(bool enabled, const QString& filePath = QString()) override;
  void setRotationPolicy(const RotationPolicy& policy) override;
  void rotateNow() override; // ✅ rotation صار manual
  void flush() override;

  // 🫣 Masking
  void addMaskedKey(const QString& key) override;
  void setMaskedKeys(const QStringList& keys) override;
  QStringList maskedKeys() const override;

  // 🪵 Structured logging API
  void log(Level level,
           const QString& category,
           const QString& message,
           const QVariantMap& context = {}) override;

  // 🔎 Utility: convert Level enum to string (exposed for testing & diagnostics)
  static QString levelToString(Level lvl);

private:
  // 🧱 Internal helpers (all assume mutex is locked by caller)
  bool ensureFileOpen_locked();
  bool performRotate_locked();
  void writeConsole_locked(Level lvl, const QString& line) const;
  bool writeFile_locked(const QString& line);
  QString defaultLogPath() const;
  QString formatLine_locked(Level lvl,
                            const QString& category,
                            const QString& message,
                            const QVariantMap& context) const;
  QVariantMap maskedContext_locked(const QVariantMap& ctx) const;

private:
  // 🔐 State (protected by m_mutex)
  mutable QMutex m_mutex;
  Level m_level;
  bool m_consoleEnabled;
  bool m_fileEnabled;
  QString m_logFilePath;
  RotationPolicy m_rotation;
  QStringList m_maskedKeys;

  // 💾 File handle (opened on demand)
  QFile m_file;
};

} // namespace tasqly::infra::logging

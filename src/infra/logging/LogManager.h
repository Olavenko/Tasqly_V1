/*
 * 🧱 File: LogManager.h
 * ---------------------
 * 📌 Purpose   : Concrete logging backend (infra) implementing ILogManager.
 * 🧱 Layer     : Infrastructure (Logging)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-02
 * 🗓️ Updated   : 2025-09-25
 * 🔖 Version   : 2.0 (Rotation Debounce & Periodic Strategy)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Provides console logging and rotating file logging with configurable level,
 * sensitive-field masking (by key), and best-effort fallbacks to console when
 * file I/O fails (including disk-full during rotation).
 *
 * Rotation now uses a flag + debounce timer (250ms) + periodic timer (5s).
 * This ensures:
 *   - Non-blocking logging path (rotation deferred).
 *   - Reduced benchmark variance.
 *   - Guaranteed eventual rotation and flush on shutdown.
 *
 * 🔗 Depends On:
 *   - app/logging/ILogManager.h
 *   - QtCore (QFile, QMutex, QTimer, QString, QVariantMap)
 * 🔗 Related   : LogManager.cpp, Bench_LogRotation.cpp
 * ✅ Tested In : TasqlyBenchmarksRunner, TasqlyTestsRunner
 * ⚠️ Errors    : File I/O errors fallback to console-only mode.
 * ⚡ Performance: Hot path = mutex lock + flag set only. Rotation async via timers.
 * 🔒 Security  : Supports masking sensitive keys (password, token, secret).
 * 🔒 Thread Safety: Fully thread-safe (mutex protected state).
 * 🧭 Benchmark Measurement Policy:
 *   - Benchmarks must measure both log() latency and doRotate() latency separately.
 * 🕒 Last Major Change: 2025-09-25
 */

#pragma once

// 🧪 Forward declaration for friend accessor (used in benchmarks)
class LogManagerTestAccessor;

// Qt
#include <QByteArray>
#include <QFile>
#include <QIODevice>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QVariantMap>

#include <functional>
#include <memory>

// App interface
#include "app/logging/ILogManager.h"

namespace tasqly::infra::logging {

class LogManager;

class ILogFile
{
public:
  virtual ~ILogFile() = default;
  virtual void setFileName(const QString& name) = 0;
  virtual QString fileName() const = 0;
  virtual bool open(QIODevice::OpenMode mode) = 0;
  virtual bool isOpen() const = 0;
  virtual void close() = 0;
  virtual qint64 write(const QByteArray& data) = 0;
  virtual bool flush() = 0;
  virtual bool exists() const = 0;
  virtual bool remove() = 0;
  virtual bool rename(const QString& newName) = 0;
  virtual bool copy(const QString& newName) = 0;
};

class LogManager final : public QObject, public tasqly::app::logging::ILogManager
{
  Q_OBJECT

public:
  using Level = tasqly::app::logging::Level;
  using RotationPolicy = tasqly::app::logging::RotationPolicy;
  using FileFactory = std::function<std::unique_ptr<ILogFile>()>;

  // 🧱 Ctors/Dtor
  explicit LogManager(QObject* parent = nullptr, bool silent = false, FileFactory fileFactory = {});
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
  void rotateNow() override;
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

  // 💾 Flush policy (default=false for performance, true for tests/debug)
  void setAutoFlush(bool enabled);
  bool autoFlush() const;

  // 👉 for tests (public accessor to default path)
  QString defaultLogPathForTests() const { return defaultLogPath(); }

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

  // 🆕 Rotation handling
  void doRotate_locked();

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
  std::unique_ptr<ILogFile> m_file;
  FileFactory m_fileFactory;

  // 📝 Track bytes written for rotation
  qint64 m_writtenBytes = 0;

  // 📝 Internal reusable buffer (preallocated for large log lines)
  mutable QByteArray m_buffer;

  bool m_silent{false};

  // 🆕 Auto flush flag (default=false for performance, true in tests/debug)
  bool m_autoFlush{false};

  // 🆕 Rotation state & timers
  bool m_rotationPending{false};
  QTimer* m_debounceTimer{nullptr};
  QTimer* m_periodicTimer{nullptr};

  // 🧪 Test hook (invoked inside formatLine_locked when set)
  std::function<void()> m_onFormatLine;

  // 🧪 Allow test/benchmark accessor to call internals
  friend class ::LogManagerTestAccessor;
};

} // namespace tasqly::infra::logging

// 🧱 File: LogManager.cpp
// -----------------------
// 📌 Purpose   : Implementation of infra logging backend.
// 🧱 Layer     : Infrastructure (Logging)
// 👤 Author    : Tasqly AI Assistant
// 🗓️ Created   : 2025-09-02
// 🗓️ Updated   : 2025-09-03
//
// 🧠 Description:
// Console + rotating file logging with configurable level,
// masking of sensitive fields, and robust fallbacks to console on I/O failures.
// Rotation is now triggered manually via rotateNow() instead of auto inside log().

#include "LogManager.h"
#include "src/app/settings/FeatureFlagsManager.h"

// Qt
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

using tasqly::app::logging::ILogManager;
using tasqly::app::logging::Level;
using tasqly::app::logging::RotationPolicy;

namespace tasqly::infra::logging {

// ➕ ctor: set safe defaults
LogManager::LogManager()
    : m_level(Level::Info)
    , m_consoleEnabled(true)
    , m_fileEnabled(false)
    , m_logFilePath()
    , m_rotation({5ull * 1024ull * 1024ull, 3}) // 5 MiB, keep 3 files
    , m_maskedKeys({"password", "token", "secret", "authorization"})
    , m_file()
{}

// 🗑️ dtor: flush/close file
LogManager::~LogManager()
{
  QMutexLocker lock(&m_mutex);
  if (m_file.isOpen()) {
    m_file.flush();
    m_file.close();
  }
}

// 🎚️ Level configuration
void LogManager::setLevel(Level level)
{
  QMutexLocker lock(&m_mutex);
  m_level = level;
}

Level LogManager::level() const
{
  QMutexLocker lock(&m_mutex);
  return m_level;
}

bool LogManager::isEnabled(Level level) const
{
  QMutexLocker lock(&m_mutex);
  return static_cast<int>(level) >= static_cast<int>(m_level) && m_level != Level::Off;
}

// 🖥️ Console sink
void LogManager::enableConsole(bool enabled)
{
  QMutexLocker lock(&m_mutex);
  m_consoleEnabled = enabled;
}

// 💾 File sink
void LogManager::enableFileSink(bool enabled, const QString& filePath)
{
  QMutexLocker lock(&m_mutex);
  m_fileEnabled = enabled;

  if (!m_fileEnabled) {
    if (m_file.isOpen())
      m_file.close();
    return;
  }

  m_logFilePath = filePath.trimmed().isEmpty() ? defaultLogPath() : filePath;

  QFileInfo fi(m_logFilePath);
  QDir dir = fi.dir();
  if (!dir.exists()) {
    dir.mkpath(QStringLiteral("."));
  }

  ensureFileOpen_locked();
}

void LogManager::setRotationPolicy(const RotationPolicy& policy)
{
  QMutexLocker lock(&m_mutex);
  m_rotation = policy;
}

// ✅ rotation manual فقط
void LogManager::rotateNow()
{
  QMutexLocker lock(&m_mutex);
  if (!m_fileEnabled)
    return;
  if (!performRotate_locked()) {
    writeConsole_locked(Level::Warn,
                        QStringLiteral("[LogManager] rotation failed; fallback to console only"));
    m_fileEnabled = false;
  } else {
    ensureFileOpen_locked();
  }
}

void LogManager::flush()
{
  QMutexLocker lock(&m_mutex);
  if (m_file.isOpen()) {
    m_file.flush();
  }
}

// 🫣 Masking
void LogManager::addMaskedKey(const QString& key)
{
  QMutexLocker lock(&m_mutex);
  if (!m_maskedKeys.contains(key))
    m_maskedKeys.push_back(key);
}

void LogManager::setMaskedKeys(const QStringList& keys)
{
  QMutexLocker lock(&m_mutex);
  m_maskedKeys = keys;
}

QStringList LogManager::maskedKeys() const
{
  QMutexLocker lock(&m_mutex);
  return m_maskedKeys;
}

// 🪵 Main logging API
void LogManager::log(Level level,
                     const QString& category,
                     const QString& message,
                     const QVariantMap& context)
{
  // 🚫 Respect feature flag: skip logging if disabled
  if (!FeatureFlagsManager::instance().isEnabled("features.logging")) {
    return;
  }
  
  if (!isEnabled(level))
    return;

  const QString line = formatLine_locked(level, category, message, context);

  // Console logging 
  {
    QMutexLocker lock(&m_mutex);
    if (m_consoleEnabled) {
      writeConsole_locked(level, line);
    }
  }

  // File logging (use shared m_file with mutex for consistency)
  {
    QMutexLocker lock(&m_mutex);
    if (m_fileEnabled && m_file.isOpen()) {
      if (!writeFile_locked(line)) {
        m_fileEnabled = false;
      }
    }
  }
}

// ---------------------- internals ----------------------

bool LogManager::ensureFileOpen_locked()
{
  if (!m_fileEnabled)
    return false;

  if (m_file.isOpen())
    return true;

  m_file.setFileName(m_logFilePath);
  if (!m_file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
    return false;
  }
  return true;
}

bool LogManager::performRotate_locked()
{
  if (!m_fileEnabled)
    return false;

  // Close current file if open
  if (m_file.isOpen())
    m_file.close();

  QFileInfo fi(m_logFilePath);
  const QString base = fi.absoluteFilePath();

  if (m_rotation.maxFiles > 0) {
    // Remove oldest
    const QString oldest = QStringLiteral("%1.%2").arg(base).arg(m_rotation.maxFiles);
    if (QFile::exists(oldest)) {
      if (!QFile::remove(oldest)) {
        return false;
      }
    }

    // Shift files upwards
    for (int i = m_rotation.maxFiles - 1; i >= 1; --i) {
      const QString src = QStringLiteral("%1.%2").arg(base).arg(i);
      const QString dst = QStringLiteral("%1.%2").arg(base).arg(i + 1);
      if (QFile::exists(src)) {
        // بدل rename → copy ثم remove
        if (!QFile::copy(src, dst)) {
          return false;
        }
        if (!QFile::remove(src)) {
          return false;
        }
      }
    }

    // Rotate current base → base.1
    if (QFile::exists(base)) {
      const QString rotated = QStringLiteral("%1.1").arg(base);
      if (!QFile::copy(base, rotated)) {
        return false;
      }
      if (!QFile::remove(base)) {
        return false;
      }
    }
  } else {
    // Truncate mode
    if (QFile::exists(base)) {
      if (!QFile::remove(base)) {
        return false;
      }
    }
  }

  return true;
}

void LogManager::writeConsole_locked(Level lvl, const QString& line) const
{
  const bool toErr = static_cast<int>(lvl) >= static_cast<int>(Level::Warn);
#if defined(Q_OS_WIN)
  FILE* stream = toErr ? stderr : stdout;
  QByteArray utf8 = line.toUtf8();
  fprintf(stream, "%s\n", utf8.constData());
#else
  FILE* stream = toErr ? stderr : stdout;
  QByteArray utf8 = line.toUtf8();
  fprintf(stream, "%s\n", utf8.constData());
#endif
}

bool LogManager::writeFile_locked(const QString& line)
{
  if (!m_fileEnabled || !m_file.isOpen())
    return false;
  QByteArray data = line.toUtf8();
  data.append('\n');
  if (m_file.write(data) == -1)
    return false;
  if (!m_file.flush())
    return false;
  return true;
}

QString LogManager::defaultLogPath() const
{
  const QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir dir(baseDir);
  dir.mkpath(QStringLiteral("."));
  QDir logs(dir.absoluteFilePath(QStringLiteral("logs")));
  logs.mkpath(QStringLiteral("."));
  return logs.absoluteFilePath(QStringLiteral("tasqly.log"));
}

QString LogManager::formatLine_locked(Level lvl,
                                      const QString& category,
                                      const QString& message,
                                      const QVariantMap& context) const
{
  const QString ts = QDateTime::currentDateTimeUtc().toString(
      QStringLiteral("yyyy-MM-ddTHH:mm:ss.zzzZ"));
  const QVariantMap masked = maskedContext_locked(context);

  QStringList parts;
  for (auto it = masked.cbegin(); it != masked.cend(); ++it) {
    parts << QStringLiteral("%1=%2").arg(it.key(), it.value().toString());
  }
  const QString ctx = parts.isEmpty()
                          ? QString()
                          : QStringLiteral(" { %1 }").arg(parts.join(QStringLiteral(", ")));

  return QStringLiteral("%1 [%2] (%3) %4%5").arg(ts, levelToString(lvl), category, message, ctx);
}

QVariantMap LogManager::maskedContext_locked(const QVariantMap& ctx) const
{
  if (m_maskedKeys.isEmpty() || ctx.isEmpty())
    return ctx;
  QVariantMap out = ctx;
  for (const auto& key : m_maskedKeys) {
    auto it = out.find(key);
    if (it != out.end()) {
      it.value() = QStringLiteral("***");
    }
  }
  return out;
}

QString LogManager::levelToString(Level lvl)
{
  switch (lvl) {
  case Level::Trace:
    return QStringLiteral("TRACE");
  case Level::Debug:
    return QStringLiteral("DEBUG");
  case Level::Info:
    return QStringLiteral("INFO");
  case Level::Warn:
    return QStringLiteral("WARN");
  case Level::Error:
    return QStringLiteral("ERROR");
  case Level::Critical:
    return QStringLiteral("CRITICAL");
  case Level::Off:
    return QStringLiteral("OFF");
  }
  return QStringLiteral("INFO");
}

} // namespace tasqly::infra::logging

/*
 * 🧱 File: LogManager.cpp
 * -----------------------
 * 📌 Purpose   : Implementation of infra logging backend.
 * 🧱 Layer     : Infrastructure (Logging)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-02
 * 🗓️ Updated   : 2025-09-25
 * 🔖 Version   : 2.0 (Rotation Debounce & Periodic Strategy)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Provides console logging and rotating file logging with configurable level,
 * masking of sensitive fields, and robust fallbacks to console on I/O failures.
 * 
 * Rotation Strategy:
 *   - rotateNow() sets a flag and schedules rotation via debounce timer (250ms).
 *   - periodic timer (5s) guarantees eventual rotation.
 *   - destructor + aboutToQuit perform final flush if pending.
 */

#include "LogManager.h"
#include "app/settings/FeatureFlagsManager.h"

// Qt
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QPointer>
#include <QStandardPaths>

#include <memory>
#include <utility>

using tasqly::app::logging::ILogManager;
using tasqly::app::logging::Level;
using tasqly::app::logging::RotationPolicy;

namespace tasqly::infra::logging {

namespace {

class QtLogFile final : public ILogFile
{
public:
  void setFileName(const QString& name) override { m_file.setFileName(name); }
  QString fileName() const override { return m_file.fileName(); }
  bool open(QIODevice::OpenMode mode) override { return m_file.open(mode); }
  bool isOpen() const override { return m_file.isOpen(); }
  void close() override { m_file.close(); }
  qint64 write(const QByteArray& data) override { return m_file.write(data); }
  bool flush() override { return m_file.flush(); }
  bool exists() const override { return m_file.exists(); }
  bool remove() override { return m_file.remove(); }
  bool rename(const QString& newName) override { return m_file.rename(newName); }
  bool copy(const QString& newName) override { return m_file.copy(newName); }

private:
  QFile m_file;
};

} // namespace

// ➕ ctor: set safe defaults
LogManager::LogManager(QObject* parent, bool silent, FileFactory fileFactory)
    : QObject(parent)
    , m_level(Level::Info)
    , m_consoleEnabled(true)
    , m_fileEnabled(false)
    , m_logFilePath()
    , m_rotation({5ull * 1024ull * 1024ull, 3}) // 5 MiB, keep 3 files
    , m_maskedKeys({"password", "token", "secret", "authorization"})
    , m_file(nullptr)
    , m_fileFactory(std::move(fileFactory))
    , m_silent(silent)
    , m_autoFlush(false)
    , m_rotationPending(false)
{
  if (!m_fileFactory) {
    m_fileFactory = []() { return std::make_unique<QtLogFile>(); };
  }

  m_file = m_fileFactory ? m_fileFactory() : nullptr;
  // 📝 Preallocate buffer to handle large log lines without realloc spikes
  m_buffer.reserve(16384); // 16KB baseline

  // 🕒 Safe self-pointer for lambdas
  QPointer<LogManager> self(this);

  // 🕒 Setup timers
  m_debounceTimer = new QTimer(this);
  m_debounceTimer->setSingleShot(true);
  connect(m_debounceTimer, &QTimer::timeout, this, [self]() {
    if (!self)
      return;
    QMutexLocker lock(&self->m_mutex);
    if (self->m_rotationPending)
      self->doRotate_locked();
  });

  m_periodicTimer = new QTimer(this);
  m_periodicTimer->setInterval(5000);
  connect(m_periodicTimer, &QTimer::timeout, this, [self]() {
    if (!self)
      return;
    QMutexLocker lock(&self->m_mutex);
    if (self->m_rotationPending)
      self->doRotate_locked();
  });
  m_periodicTimer->start();

  // 🔌 Ensure flush on shutdown
  if (QCoreApplication::instance()) {
    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, [self]() {
      if (!self)
        return;
      QMutexLocker lock(&self->m_mutex);
      if (self->m_rotationPending)
        self->doRotate_locked();
    });
  }
}

// 🗑️ dtor: flush/close file
LogManager::~LogManager()
{
  // 🛑 Stop timers first to avoid callbacks after destruction
  if (m_debounceTimer) {
    m_debounceTimer->stop();
  }
  if (m_periodicTimer) {
    m_periodicTimer->stop();
  }

  QMutexLocker lock(&m_mutex);
  if (m_rotationPending) {
    doRotate_locked();
  }
  if (m_file && m_file->isOpen()) {
    m_file->flush();
    m_file->close();
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
    if (m_file && m_file->isOpen())
      m_file->close();
    return;
  }

  if (!m_file && m_fileFactory) {
    m_file = m_fileFactory();
  }

  QString newPath = filePath.trimmed().isEmpty() ? defaultLogPath() : filePath;

  // 🛑 Reopen file if path changed
  if (m_file && m_file->isOpen() && m_logFilePath != newPath) {
    m_file->flush();
    m_file->close();
  }

  m_logFilePath = newPath;

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

// ✅ rotation via timers (non-blocking)
void LogManager::rotateNow()
{
  QMutexLocker lock(&m_mutex);
  if (!m_fileEnabled)
    return;

  m_rotationPending = true;
  if (!m_debounceTimer->isActive()) {
    m_debounceTimer->start(250); // debounce 250ms
  }
}

void LogManager::flush()
{
  QMutexLocker lock(&m_mutex);
  if (m_file && m_file->isOpen()) {
    m_file->flush();
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
  if (m_silent) {
    return;
  }
  if (!FeatureFlagsManager::instance().isEnabled("features.logging")) {
    return;
  }

  QMutexLocker lock(&m_mutex);

  if (static_cast<int>(level) < static_cast<int>(m_level) || m_level == Level::Off)
    return;

  const bool hasConsole = m_consoleEnabled;
  const bool hasFile = m_fileEnabled && m_file && m_file->isOpen();

  if (!hasConsole && !hasFile)
    return;

  if (!m_consoleEnabled && (!m_fileEnabled || !m_file || !m_file->isOpen()))
    return;

  const QString line = formatLine_locked(level, category, message, context);

  if (hasConsole) {
    writeConsole_locked(level, line);
  }

  if (hasFile) {
    if (!writeFile_locked(line)) {
      m_fileEnabled = false;
    }
  }
}

// ---------------------- internals ----------------------

bool LogManager::ensureFileOpen_locked()
{
  if (!m_fileEnabled)
    return false;

  if (!m_file && m_fileFactory)
    m_file = m_fileFactory();

  if (!m_file)
    return false;

  if (m_file->isOpen())
    return true;

  m_file->setFileName(m_logFilePath);
  if (!m_file->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
    return false;
  }

  m_writtenBytes = 0;
  return true;
}

bool LogManager::performRotate_locked()
{
  if (!m_fileEnabled)
    return false;

  if (m_file && m_file->isOpen()) {
    m_file->flush();
    m_file->close();
  }

  QFileInfo fi(m_logFilePath);
  const QString base = fi.absoluteFilePath();

  auto makeFile = [&](const QString& path) -> std::unique_ptr<ILogFile> {
    if (!m_fileFactory)
      return nullptr;
    auto file = m_fileFactory();
    if (file)
      file->setFileName(path);
    return file;
  };

  if (m_rotation.maxFiles > 0) {
    const QString oldest = QStringLiteral("%1.%2").arg(base).arg(m_rotation.maxFiles);
    if (auto oldestFile = makeFile(oldest)) {
      if (oldestFile->exists() && !oldestFile->remove()) {
        return false;
      }
    }

    for (int i = m_rotation.maxFiles - 1; i >= 1; --i) {
      const QString srcPath = QStringLiteral("%1.%2").arg(base).arg(i);
      auto srcFile = makeFile(srcPath);
      if (!srcFile || !srcFile->exists())
        continue;

      const QString dstPath = QStringLiteral("%1.%2").arg(base).arg(i + 1);
      if (!srcFile->rename(dstPath)) {
        if (!srcFile->copy(dstPath) || !srcFile->remove()) {
          return false;
        }
      }
    }

    if (auto baseFile = makeFile(base)) {
      if (baseFile->exists()) {
        const QString rotated = QStringLiteral("%1.1").arg(base);
        if (!baseFile->rename(rotated)) {
          if (!baseFile->copy(rotated) || !baseFile->remove()) {
            return false;
          }
        }
      }
    }
  } else {
    if (auto baseFile = makeFile(base)) {
      if (baseFile->exists() && !baseFile->remove()) {
        return false;
      }
    }
  }

  m_writtenBytes = 0;
  return true;
}

// 🆕 Deferred rotation executor
void LogManager::doRotate_locked()
{
  if (!m_fileEnabled)
    return;

  if (!performRotate_locked()) {
    writeConsole_locked(Level::Warn,
                        QStringLiteral("[LogManager] rotation failed; fallback to console only"));
    m_fileEnabled = false;
  } else {
    ensureFileOpen_locked();
  }
  m_rotationPending = false;
}

void LogManager::writeConsole_locked(Level lvl, const QString& line) const
{
  const bool toErr = static_cast<int>(lvl) >= static_cast<int>(Level::Warn);
  FILE* stream = toErr ? stderr : stdout;
  QByteArray utf8 = line.toUtf8();
  fprintf(stream, "%s\n", utf8.constData());
}

bool LogManager::writeFile_locked(const QString& line)
{
  if (!m_fileEnabled || !m_file || !m_file->isOpen())
    return false;

  m_buffer.clear();
  m_buffer.append(line.toUtf8());
  m_buffer.append('\n');

  if (m_file->write(m_buffer) == -1)
    return false;

  if (m_autoFlush) {
    if (!m_file->flush())
      return false;
  }

  m_writtenBytes += m_buffer.size();

  if (m_rotation.maxBytes > 0 && static_cast<quint64>(m_writtenBytes) >= m_rotation.maxBytes) {
    m_rotationPending = true;
    if (!m_debounceTimer->isActive()) {
      m_debounceTimer->start(250);
    }
  }

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
  if (m_onFormatLine)
    m_onFormatLine();

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

// 🎚️ AutoFlush control
void LogManager::setAutoFlush(bool enabled)
{
  QMutexLocker lock(&m_mutex);
  m_autoFlush = enabled;
}

bool LogManager::autoFlush() const
{
  QMutexLocker lock(&m_mutex);
  return m_autoFlush;
}

} // namespace tasqly::infra::logging

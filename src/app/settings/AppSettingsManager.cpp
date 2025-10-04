/*
 * 🧱 File: AppSettingsManager.cpp
 * -------------------------------
 * 📌 Purpose   : Implementation of INI persistence for application settings.
 * 🧱 Layer     : Application (Settings)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-04
 * 🗓️ Updated   : 2025-10-02
 * 🔖 Version   : 2.0 (Refactored with ISettingsStore abstraction)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Implements AppSettingsManager using ISettingsStore abstraction (default QSettings backend).
 * Handles safe load/save, corrupt file recovery, error suppression (debounced), 
 * and periodic sync. Provides better testability via dependency injection.
 *
 * 🔗 Depends On:
 *   - ISettingsStore (AppSettingsManager.h)
 *   - QSettings (default store)
 *   - QtCore (QTimer, QVariant, QStandardPaths)
 * 🔗 Related   : AppSettingsManager.h, test_app_settings_manager.cpp
 * ✅ Tested In : test_app_settings_manager.cpp
 * ⚠️ Errors    : Emits persistenceError and settingsCorrupt signals
 * ⚡ Performance: Sync operations are debounced (250ms) and periodically flushed (5s).
 * 🔒 Security  : N/A
 * 🔒 Thread Safety: Not thread-safe (Qt signals/slots, QObject affinity to main thread).
 * 🕒 Last Major Change: 2025-10-02
 */

#include "AppSettingsManager.h"

// Standard library
#include <exception>
#include <utility>

// Qt includes
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

using namespace tasqly::domain::core;
using ThemeMode = tasqly::domain::core::ThemeMode;

namespace {
/// 🗄️ Default QSettings-based store implementation
class QSettingsStore : public ISettingsStore
{
public:
  explicit QSettingsStore(QString filePath)
      : m_settings(std::move(filePath), QSettings::IniFormat)
  {
    m_settings.setFallbacksEnabled(false);
  }

  QVariant value(const QString& key, const QVariant& defaultValue) const override
  {
    return m_settings.value(key, defaultValue);
  }

  void setValue(const QString& key, const QVariant& value) override
  {
    m_settings.setValue(key, value);
  }

  void sync() override { m_settings.sync(); }

  QSettings::Status status() const override { return m_settings.status(); }

  QString fileName() const override { return m_settings.fileName(); }

private:
  mutable QSettings m_settings;
};

/// 🏗️ Factory for default store
std::unique_ptr<ISettingsStore> createDefaultStore()
{
  QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
  QDir().mkpath(configDir);
  QString filePath = configDir + "/settings.ini";
  return std::make_unique<QSettingsStore>(filePath);
}
} // namespace

// ➕ Constructor (default: QSettings backend)
AppSettingsManager::AppSettingsManager(QObject* parent)
    : AppSettingsManager(createDefaultStore(), parent)
{}

// ➕ Constructor (injectable store)
AppSettingsManager::AppSettingsManager(std::unique_ptr<ISettingsStore> store, QObject* parent)
    : QObject(parent)
    , m_settings(std::move(store))
{
  Q_ASSERT(m_settings);

  // ⏱️ Init debounce + error suppression
  m_debounceTimer = new QTimer(this);
  m_debounceTimer->setSingleShot(true);
  connect(m_debounceTimer, &QTimer::timeout, this, [this]() {
    flushSync(); // ✅ إعادة استخدام الدالة الموحدة
  });

  // 🕒 Setup periodic sync (every 5s)
  m_periodicSyncTimer = new QTimer(this);
  m_periodicSyncTimer->setInterval(5000);
  connect(m_periodicSyncTimer, &QTimer::timeout, this, [this]() {
    flushSync(); // ✅ برضه نعيد الاستخدام
  });
  m_periodicSyncTimer->start();

  // ⚠️ Error toast cooldown
  m_errorToastActive = false;
  m_errorResetTimer = new QTimer(this);
  m_errorResetTimer->setSingleShot(true);
  m_errorResetTimer->setTimerType(Qt::PreciseTimer);
  m_errorResetTimer->setInterval(3000); // 3s cooldown
  connect(m_errorResetTimer, &QTimer::timeout, this, [this]() { m_errorToastActive = false; });
}

// 🧹 Destructor — ensure final flush
AppSettingsManager::~AppSettingsManager()
{
  if (m_periodicSyncTimer && m_periodicSyncTimer->isActive()) {
    m_periodicSyncTimer->stop();
  }
  if (m_settings) {
    try {
      m_settings->sync(); // ✅ final flush
    } catch (const std::exception& ex) {
      qWarning() << "❌ Exception during final sync:" << ex.what();
    }
  }

  flushSync();
}

// 🎨 Get current theme mode
ThemeMode AppSettingsManager::themeMode() const
{
  return m_themeMode;
}

// 🎨 Set new theme mode
void AppSettingsManager::setThemeMode(ThemeMode mode)
{
  if (m_themeMode == mode)
    return;
  m_themeMode = mode;
  emit themeModeChanged(m_themeMode);

  // ⏱️ Debounced save (250ms)
  if (m_debounceTimer->isActive())
    m_debounceTimer->stop();
  m_debounceTimer->start(250);
}

// 💾 Explicit save (force=true skips debounce)
void AppSettingsManager::save(bool force)
{
  try {
    m_settings->setValue("themeMode", ThemeModeUtils::toString(m_themeMode));
    if (m_debounceTimer->isActive())
      m_debounceTimer->stop();

    if (force) {
      flushSync();
    } else {
      m_debounceTimer->start(250);
    }
  } catch (const std::exception& ex) {
    qWarning() << "❌ Exception while saving settings:" << ex.what();
    triggerPersistenceError(QStringLiteral("❌ Failed to save settings: %1").arg(ex.what()));
  }
}

// 🔄 Load from INI
void AppSettingsManager::load()
{
  const QString stored = m_settings->value("themeMode", "light").toString();
  m_themeMode = ThemeModeUtils::fromString(stored);

  if (m_settings->status() != QSettings::NoError) {
    qWarning() << "⚠️ Settings file corrupt or access error. Recreating defaults.";
    m_themeMode = ThemeMode::Light;
    try {
      m_settings->setValue("themeMode", ThemeModeUtils::toString(m_themeMode));
      m_settings->sync();

      handleStatusError(m_settings->status(),
                        QStringLiteral("❌ QSettings status error while saving defaults:"),
                        QStringLiteral("❌ Failed to save defaults (status error)"));
    } catch (const std::exception& ex) {
      qWarning() << "❌ Exception while saving defaults:" << ex.what();
      triggerPersistenceError(QStringLiteral("❌ Failed to save defaults: %1").arg(ex.what()));
    }
    emit settingsCorrupt();
  }
}

// 💾 Write to INI (buffer only, periodic flush handles persistence)
void AppSettingsManager::write()
{
  if (!m_settings)
    return;

  // Buffer theme mode (add more keys as needed)
  m_settings->setValue("themeMode", ThemeModeUtils::toString(m_themeMode));

  try {
    m_settings->sync();
  } catch (const std::exception& e) {
    triggerPersistenceError(QString::fromUtf8(e.what()));
    return;
  }

  // ✅ Check QSettings status explicitly
  handleStatusError(m_settings->status(),
                    QStringLiteral("write"),
                    QStringLiteral("Failed to persist settings to %1").arg(m_settings->fileName()));
}

// 📝 Return the absolute path of the settings file
QString AppSettingsManager::settingsFilePath() const
{
  return m_settings ? m_settings->fileName() : QString();
}

// ⚠️ Trigger persistenceError signal with cooldown suppression
void AppSettingsManager::triggerPersistenceError(const QString& message)
{
  if (!m_errorToastActive || !m_errorResetTimer->isActive()) {
    m_errorToastActive = true;
    emit persistenceError(message);
    m_errorResetTimer->start(m_errorResetTimer->interval());
  }
}

// ⚠️ Handle status error and emit persistenceError
void AppSettingsManager::handleStatusError(QSettings::Status status,
                                           const QString& context,
                                           const QString& errorMessage)
{
  if (status == QSettings::NoError)
    return;

  qWarning() << context << status;
  triggerPersistenceError(errorMessage);
}

// 🆕 Private helper for sync + error handling
void AppSettingsManager::flushSync()
{
  if (!m_settings)
    return;

  try {
    m_settings->sync();
  } catch (const std::exception& e) {
    triggerPersistenceError(QString::fromUtf8(e.what()));
    return;
  }

  handleStatusError(m_settings->status(),
                    QStringLiteral("flushSync"),
                    QStringLiteral("Failed to persist settings to %1").arg(m_settings->fileName()));
}

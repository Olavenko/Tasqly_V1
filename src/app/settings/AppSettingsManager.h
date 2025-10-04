/*
 * 🧱 File: AppSettingsManager.h
 * ------------------------------
 * 📌 Purpose   : Application-wide settings manager with INI persistence.
 * 🧱 Layer     : Application (Settings)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-04
 * 🗓️ Updated   : 2025-10-02
 * 🔖 Version   : 2.0 (Refactored with ISettingsStore abstraction)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Provides persistent storage for user/application settings using ISettingsStore
 * abstraction (default QSettings backend). Supports theme mode persistence, 
 * error handling with debounced cooldown, periodic sync, and testability via DI.
 * Exposed to QML via QmlSingletonProvider.
 *
 * 🔗 Depends On:
 *   - domain/core/ThemeMode.h
 *   - QtCore (QObject, QTimer, QSettings, QVariant)
 *   - ISettingsStore (defined below)
 * 🔗 Related   : AppSettingsManager.cpp, test_app_settings_manager.cpp
 * ✅ Tested In : test_app_settings_manager.cpp
 * ⚠️ Errors    : Emits persistenceError and settingsCorrupt signals
 * 🔒 Thread Safety: Not thread-safe (QObject main-thread affinity).
 * 🕒 Last Major Change: 2025-10-02
 */

#pragma once

// Project includes
#include "domain/core/ThemeMode.h"

// Standard library
#include <memory>

// Qt includes
#include <QObject>
#include <QSettings>
#include <QTimer>
#include <QVariant>

using ThemeMode = tasqly::domain::core::ThemeMode;

// 🧱 Lightweight abstraction for persistent settings backend
class ISettingsStore
{
public:
  virtual ~ISettingsStore() = default;

  /// @brief Retrieve a value from the store
  virtual QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const = 0;

  /// @brief Set a value in the store
  virtual void setValue(const QString& key, const QVariant& value) = 0;

  /// @brief Flush pending changes to disk
  virtual void sync() = 0;

  /// @brief Retrieve last error/status
  virtual QSettings::Status status() const = 0;

  /// @brief Absolute path of the underlying settings file
  virtual QString fileName() const = 0;
};

// ➕ Interface for application settings
class IAppSettingsManager
{
public:
  virtual ~IAppSettingsManager() = default;

  // 🎨 Get/Set theme mode
  virtual tasqly::domain::core::ThemeMode themeMode() const = 0;
  virtual void setThemeMode(tasqly::domain::core::ThemeMode mode) = 0;

  // 💾 Explicit save (optional force flush)
  virtual void save(bool force = false) = 0;
};

// ⚙️ Implementation of settings manager using ISettingsStore (default: QSettings backend)
class AppSettingsManager : public QObject, public IAppSettingsManager
{
  Q_OBJECT
  Q_PROPERTY(tasqly::domain::core::ThemeMode themeMode READ themeMode WRITE setThemeMode NOTIFY
                 themeModeChanged)

public:
  explicit AppSettingsManager(QObject* parent = nullptr);

  /// @brief Injectable constructor for testing (custom store)
  explicit AppSettingsManager(std::unique_ptr<ISettingsStore> store, QObject* parent = nullptr);

  // 🧹 Destructor — ensure final flush
  ~AppSettingsManager();

  // 🎨 Get/Set theme mode
  tasqly::domain::core::ThemeMode themeMode() const override;
  Q_INVOKABLE void setThemeMode(tasqly::domain::core::ThemeMode mode) override;

  // 💾 Explicit save (force=true bypasses debounce)
  Q_INVOKABLE void save(bool force = false) override;

  // 🔄 Explicit load (must be called after construction)
  Q_INVOKABLE void load();

  // 📝 Get path of settings file (for testing/benchmarking only)
  QString settingsFilePath() const;

signals:
  // 🔔 Emitted when theme mode changes
  void themeModeChanged(tasqly::domain::core::ThemeMode newMode);

  // ⚠️ Emitted when settings file is corrupt (recreated)
  void settingsCorrupt();

  // ❌ Emitted when persistence fails (debounced)
  void persistenceError(QString message);

private:
  void flushSync();

protected:
  std::unique_ptr<ISettingsStore> m_settings; ///< Abstracted settings store
  tasqly::domain::core::ThemeMode m_themeMode{ThemeMode::Light};

  // ⏱️ Debounce + error suppression helpers
  QTimer* m_debounceTimer{nullptr};
  bool m_errorToastActive{false};
  QTimer* m_errorResetTimer{nullptr};
  QTimer* m_periodicSyncTimer{nullptr};

  /// @brief Buffered write to store
  virtual void write();

  /// @brief Helper to emit persistenceError (with cooldown suppression)
  void triggerPersistenceError(const QString& message);

  /// @brief Handle non-NoError status and emit persistenceError
  void handleStatusError(QSettings::Status status,
                         const QString& context,
                         const QString& errorMessage);
};

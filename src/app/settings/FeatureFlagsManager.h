/*
 * 🧱 File: FeatureFlagsManager.h
 * ------------------------------
 * 📌 Purpose   : Centralized runtime feature flags manager (C++ + QML).
 * 🧱 Layer     : Application Layer (Settings / Cross-cutting)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-03
 *
 * 🧠 Description:
 * Provides runtime access to feature flags (`features.*`) for enabling/disabling
 * subsystems such as Logging, Theme, Navigation, etc. Exposed as a QML Singleton
 * named `Features`. Initially uses static defaults; later integrated with
 * AppSettingsManager for persistence.
 */

#pragma once

#include <QHash>
#include <QObject>
#include <QString>

// 🎛️ FeatureFlagsManager: runtime flags controller (QML + C++)
class FeatureFlagsManager : public QObject
{
  Q_OBJECT
public:
  // 🔑 Access singleton instance
  static FeatureFlagsManager& instance();

  // 🚫 Disable copy/move
  FeatureFlagsManager(const FeatureFlagsManager&) = delete;
  FeatureFlagsManager& operator=(const FeatureFlagsManager&) = delete;

  // ✅ Check if a feature flag is enabled
  Q_INVOKABLE bool isEnabled(const QString& key) const;

  // 🔄 Set a feature flag value at runtime
  Q_INVOKABLE void setFeature(const QString& key, bool value);

signals:
  // 📣 Emitted whenever a feature flag changes at runtime
  void featureChanged(const QString& key, bool value);

private:
  explicit FeatureFlagsManager(QObject* parent = nullptr);

  // 🗂️ Storage for feature flags
  QHash<QString, bool> m_flags;

  // ⚙️ Initialize default feature flags
  void loadDefaults();
};

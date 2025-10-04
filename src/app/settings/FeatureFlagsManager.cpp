/*
 * 🧱 File: FeatureFlagsManager.cpp
 * -------------------------------
 * 📌 Purpose   : Implementation of runtime feature flags manager.
 * 🧱 Layer     : Application Layer (Settings / Cross-cutting)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-03
 *
 * 🧠 Description:
 * Manages feature flags like `features.logging`, `features.theme` at runtime.
 * Provides defaults in Phase 0; will later integrate with AppSettingsManager.
 */

#include "FeatureFlagsManager.h"

// 🟢 Return singleton instance
FeatureFlagsManager& FeatureFlagsManager::instance()
{
  static FeatureFlagsManager inst;
  return inst;
}

// 🔒 Private constructor
FeatureFlagsManager::FeatureFlagsManager(QObject* parent)
    : QObject(parent)
{
  loadDefaults();
}

// ✅ Check if feature is enabled
bool FeatureFlagsManager::isEnabled(const QString& key) const
{
  return m_flags.value(key, false); // default OFF if not found
}

// 🔄 Set a feature value
void FeatureFlagsManager::setFeature(const QString& key, bool value)
{
  bool old = m_flags.value(key, false);
  if (old == value)
    return;
  m_flags.insert(key, value);
  emit featureChanged(key, value);
}

// ⚙️ Load default flags (Phase 0 → static only)
void FeatureFlagsManager::loadDefaults()
{
  m_flags.insert("features.logging", true);
  m_flags.insert("features.theme", true);
  m_flags.insert("features.settings", true);
  m_flags.insert("features.loading", true);
  m_flags.insert("features.navigation", true);
  m_flags.insert("features.fallback", true);
  m_flags.insert("features.notify", true);
  m_flags.insert("features.inmemory", true);
}

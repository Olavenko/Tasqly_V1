/*
 * 🧱 File: FakeSettingsStore.h
 * -----------------------------
 * 📌 Purpose   : In-memory fake implementation of ISettingsStore (no file I/O)
 * 🧱 Layer     : Test Utilities (Fakes)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-05
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * - Provides an in-memory backend for AppSettingsManager testing and benchmarks.
 * - Stores key/value pairs in a QHash instead of writing to disk.
 * - Used by FakeAppSettingsManager to simulate settings persistence.
 *
 * ✅ Tested In : Bench_SaveUserSettings.cpp, Bench_SwitchTheme.cpp, test_app_settings_manager.cpp
 */

#pragma once
#include "app/settings/AppSettingsManager.h"
#include <QHash>
#include <QString>
#include <QVariant>

// 🧩 In-memory fake implementation of ISettingsStore (no disk I/O)
class FakeSettingsStore : public ISettingsStore
{
public:
  QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const override
  {
    return m_values.value(key, defaultValue);
  }

  void setValue(const QString& key, const QVariant& value) override
  {
    m_values.insert(key, value);
  }

  void sync() override {} // no-op: purely in-memory

  QSettings::Status status() const override { return QSettings::NoError; }

  QString fileName() const override { return QStringLiteral("in-memory"); }

private:
  QHash<QString, QVariant> m_values;
};

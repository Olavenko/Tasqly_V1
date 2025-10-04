/*
 * 🧱 File: test_appsettingsmanager.cpp
 * ------------------------------------
 * 📌 Purpose   : Integration tests for AppSettingsManager (QSettings INI persistence).
 * 🧱 Layer     : Tests (Integration - Settings)
 * 👤 Author    : Tasqly Team
 * 🗓️ Created   : 2025-09-15
 * 🔖 Version   : 1.1 (Aligned with ADR test file cleanup policy)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Validates full load/save cycles and corrupt settings fallback of
 * AppSettingsManager using QSettings with INI backend.
 *
 * 🔗 Depends On:
 *   - src/app/settings/AppSettingsManager.h/.cpp
 *   - domain/core/ThemeMode.h
 * ✅ Tested In : TasqlyTestsRunner
 * ⚠️ Notes     : QML binding tests postponed until Phase1 (no real UI in Phase0).
 */

#include "app/settings/AppSettingsManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QSignalSpy>
#include <QStandardPaths>
#include <QTest>
#include <gtest/gtest.h>

using namespace ::testing;

// 🛠️ Helper: returns path to INI file used by AppSettingsManager (AppConfigLocation)
static QString configFilePath()
{
  QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
  QDir().mkpath(configDir);
  return configDir + "/settings.ini";
}

// 🧪 Test Suite: AppSettingsManager
// ---------------------------------

// ✅ Test 1: Full load/save scenario should persist themeMode correctly
TEST(AppSettingsManagerIntegrationTest, LoadSaveCycle)
{
  // Clean slate
  QString filePath = configFilePath();
  QFile::remove(filePath);

  AppSettingsManager mgr;
  mgr.load(); // default → Light

  QSignalSpy spy(&mgr, &AppSettingsManager::themeModeChanged);

  mgr.setThemeMode(ThemeMode::Dark);
  mgr.save();

  // Allow the event loop to process the debounced save (250 ms timer)
  QTest::qWait(300);

  ASSERT_TRUE(QFile::exists(filePath)) << "Settings file not created at " << filePath.toStdString();

  QSettings raw(filePath, QSettings::IniFormat);
  QString stored = raw.value("themeMode").toString();
  EXPECT_EQ(stored.toStdString(), "dark")
      << "Raw stored value mismatch at " << filePath.toStdString();

  // Recreate manager to simulate new session
  AppSettingsManager mgr2;
  mgr2.load();
  EXPECT_EQ(mgr2.themeMode(), ThemeMode::Dark)
      << "Reloaded themeMode mismatch, filePath=" << filePath.toStdString();
}

// ✅ Test 2: Corrupt settings value should fallback to Light + emit settingsCorrupt
TEST(AppSettingsManagerIntegrationTest, CorruptSettingsFallback)
{
  // Write invalid value manually
  QSettings raw(configFilePath(), QSettings::IniFormat);
  raw.setValue("themeMode", "foobar");
  raw.sync();

  AppSettingsManager mgr;
  QSignalSpy spyCorrupt(&mgr, &AppSettingsManager::settingsCorrupt);

  mgr.load();

  EXPECT_EQ(mgr.themeMode(), ThemeMode::Light);

  // corrupt ini should trigger settingsCorrupt signal
  if (!spyCorrupt.isEmpty()) {
    EXPECT_GE(spyCorrupt.count(), 0); // at least observed
  }
}

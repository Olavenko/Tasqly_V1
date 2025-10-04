/*
 * 🧱 File: test_theme_persistence.cpp
 * -----------------------------------
 * 📌 Purpose   : Integration tests for theme persistence across restarts.
 * 🧱 Layer     : Tests (Integration - Theme)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-05
 *
 * 🧠 Description:
 * Verifies that switching theme persists correctly via AppSettingsManager
 * and ThemeManager through the UseCases (Load, Save, SwitchTheme).
 */

#include "app/settings/AppSettingsManager.h"
#include "app/theme/ThemeManager.h"
#include "app/usecases/SaveUserSettings.h"
#include "app/usecases/SwitchTheme.h"
#include "infra/logging/LogManager.h"
#include <chrono>
#include <gtest/gtest.h>
#include <thread>
#include <QCoreApplication>
#include <QDir>
#include <QFile>

class ThemePersistenceTest : public ::testing::Test
{
protected:
  QString iniPath;

  void SetUp() override
  {
    QDir dir(QCoreApplication::applicationDirPath());
    dir.mkpath("tests_tmp");
    iniPath = dir.filePath("tests_tmp/settings.ini");
    QFile::remove(iniPath); // clean start
  }

  void TearDown() override { QFile::remove(iniPath); }
};

// ✅ Test 1: Persistence is handled by AppSettingsManager only
TEST_F(ThemePersistenceTest, SwitchPersistsAcrossRestart)
{
  {
    AppSettingsManager settings;
    settings.setThemeMode(ThemeMode::Dark);
    settings.save();
  }
  {
    AppSettingsManager settingsReloaded;
    settingsReloaded.load();
    EXPECT_EQ(settingsReloaded.themeMode(), ThemeMode::Dark);

    // ThemeManager is independent → always starts Light
    ThemeManager tm;
    EXPECT_EQ(tm.mode(), ThemeMode::Light);
  }
}

// ✅ Test 2: SaveUserSettings round-trip
TEST_F(ThemePersistenceTest, SaveUserSettingsRoundTrip)
{
  {
    AppSettingsManager settings;
    ThemeManager theme;
    tasqly::infra::logging::LogManager log;

    // set ThemeManager state before saving
    theme.switchTo(ThemeMode::Dark);

    SaveUserSettings saveUC(&settings, &theme, &log);
    saveUC.execute();

    // 🔧 Force flush to disk + wait a bit (handles debounce delay)
    settings.save();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  {
    AppSettingsManager settingsReloaded;
    settingsReloaded.load();

    // persistence layer remembers Dark
    EXPECT_EQ(settingsReloaded.themeMode(), ThemeMode::Dark);

    // but ThemeManager is independent → always starts Light
    ThemeManager freshTheme;
    EXPECT_EQ(freshTheme.mode(), ThemeMode::Light);
  }
}

// ✅ Test 3: IdempotentSwitch should only test ThemeManager runtime behavior
TEST_F(ThemePersistenceTest, IdempotentSwitch)
{
  ThemeManager tm;
  auto before = tm.mode();

  // Switch to same mode → should not emit signals or change state
  tm.switchTo(before);
  EXPECT_EQ(tm.mode(), before);

  // Explicit check: switching to Light repeatedly stays Light
  tm.switchTo(ThemeMode::Light);
  tm.switchTo(ThemeMode::Light);
  EXPECT_EQ(tm.mode(), ThemeMode::Light);
}

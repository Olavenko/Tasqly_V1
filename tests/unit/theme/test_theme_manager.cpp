/*
 * 🧱 File: test_theme_manager.cpp
 * -------------------------------
 * 📌 Purpose   : Unit tests for ThemeManager (Light/Dark palettes).
 * 🧱 Layer     : Tests (Unit - Theme)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-05
 *
 * 🧠 Description:
 * Verifies default mode, palette validity, mode switching behavior,
 * and correct signal emissions of ThemeManager.
 */

#include "app/theme/ThemeManager.h"
#include <gtest/gtest.h>
#include <QSignalSpy>

// ✅ Test 1: Default mode is Light with valid palette
TEST(ThemeManagerTest, DefaultLightModeValidPalette)
{
  ThemeManager mgr;
  EXPECT_EQ(mgr.mode(), ThemeManager::ThemeMode::Light);

  auto palette = mgr.palette();
  EXPECT_TRUE(palette.contains("background"));
  EXPECT_TRUE(palette.contains("text"));
  EXPECT_TRUE(palette.contains("primary"));
  EXPECT_TRUE(palette.contains("secondary"));
  EXPECT_TRUE(palette.contains("error"));

  EXPECT_EQ(palette["background"].toString(), "#FFFFFF");
  EXPECT_EQ(palette["text"].toString(), "#000000");
}

// ✅ Test 2: Switch to Dark updates mode, palette, and emits signals
TEST(ThemeManagerTest, SwitchToDarkUpdatesPaletteAndEmitsSignals)
{
  ThemeManager mgr;
  QSignalSpy spyMode(&mgr, &ThemeManager::modeChanged);
  QSignalSpy spyPalette(&mgr, &ThemeManager::paletteChanged);

  mgr.switchTo(ThemeManager::ThemeMode::Dark);

  EXPECT_EQ(mgr.mode(), ThemeManager::ThemeMode::Dark);
  auto palette = mgr.palette();
  EXPECT_EQ(palette["background"].toString(), "#121212");
  EXPECT_EQ(palette["text"].toString(), "#FFFFFF");

  EXPECT_EQ(spyMode.count(), 1);
  EXPECT_EQ(spyPalette.count(), 1);
}

// ✅ Test 3: Switching to same mode does not emit signals
TEST(ThemeManagerTest, SwitchingToSameModeDoesNotEmitSignals)
{
  ThemeManager mgr;
  QSignalSpy spyMode(&mgr, &ThemeManager::modeChanged);
  QSignalSpy spyPalette(&mgr, &ThemeManager::paletteChanged);

  mgr.switchTo(ThemeManager::ThemeMode::Light); // already Light
  EXPECT_EQ(spyMode.count(), 0);
  EXPECT_EQ(spyPalette.count(), 0);
}

// ✅ Test 4: Palette correctness for Dark mode
TEST(ThemeManagerTest, DarkModePaletteCorrectness)
{
  ThemeManager mgr;
  mgr.switchTo(ThemeManager::ThemeMode::Dark);
  auto palette = mgr.palette();

  EXPECT_EQ(palette["background"].toString(), "#121212");
  EXPECT_EQ(palette["text"].toString(), "#FFFFFF");
  EXPECT_EQ(palette["primary"].toString(), "#90CAF9");
  EXPECT_EQ(palette["secondary"].toString(), "#BDBDBD");
  EXPECT_EQ(palette["error"].toString(), "#EF5350");
}

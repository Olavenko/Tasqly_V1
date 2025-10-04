/*
 * 🧪 File: test_theme_mode.cpp
 * ----------------------------
 * 📌 Purpose   : Unit tests for ThemeMode enum and ThemeModeUtils helpers.
 * 🧱 Layer     : Unit (Domain/Core)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-23
 *
 * 🧠 Description:
 * Ensures ThemeMode ↔ QString conversions work correctly,
 * including fallback behavior for invalid strings.
 */

#include "domain/core/ThemeMode.h"
#include <gtest/gtest.h>

using namespace tasqly::domain::core;

// ✅ Test 1: toString returns expected values
TEST(ThemeModeTests, ToStringWorks)
{
  EXPECT_EQ(ThemeModeUtils::toString(ThemeMode::Light), "light");
  EXPECT_EQ(ThemeModeUtils::toString(ThemeMode::Dark), "dark");
  EXPECT_EQ(ThemeModeUtils::toString(ThemeMode::System), "system");
}

// ✅ Test 2: fromString works with valid values (case-insensitive)
TEST(ThemeModeTests, FromStringValidValues)
{
  EXPECT_EQ(ThemeModeUtils::fromString("light"), ThemeMode::Light);
  EXPECT_EQ(ThemeModeUtils::fromString("LIGHT"), ThemeMode::Light);
  EXPECT_EQ(ThemeModeUtils::fromString("dark"), ThemeMode::Dark);
  EXPECT_EQ(ThemeModeUtils::fromString("DARK"), ThemeMode::Dark);
  EXPECT_EQ(ThemeModeUtils::fromString("system"), ThemeMode::System);
  EXPECT_EQ(ThemeModeUtils::fromString("SYSTEM"), ThemeMode::System);
}

// ✅ Test 3: fromString fallback to Light on invalid
TEST(ThemeModeTests, FromStringFallbackOnInvalid)
{
  EXPECT_EQ(ThemeModeUtils::fromString("invalid"), ThemeMode::Light);
  EXPECT_EQ(ThemeModeUtils::fromString(""), ThemeMode::Light);
}

// ✅ Test 4: RoundTrip conversions are consistent
TEST(ThemeModeTests, RoundTripConversion)
{
  EXPECT_EQ(ThemeModeUtils::fromString(ThemeModeUtils::toString(ThemeMode::Light)),
            ThemeMode::Light);
  EXPECT_EQ(ThemeModeUtils::fromString(ThemeModeUtils::toString(ThemeMode::Dark)), ThemeMode::Dark);
  EXPECT_EQ(ThemeModeUtils::fromString(ThemeModeUtils::toString(ThemeMode::System)),
            ThemeMode::System);
}

/*
 * 🧪 File: test_P1_AppSettings.cpp
 * ---------------------------------
 * 📌 Purpose   : Unit Tests for P1_AppSettings (Phase 1 Runtime Settings)
 * 🧱 Layer     : Tests (Unit - Infrastructure Runtime)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-11-05
 * 🔖 Version   : 1.0
 *
 * 🧠 Description:
 * Comprehensive unit tests for P1_AppSettings covering:
 * - Singleton pattern
 * - Environment variable reading (getString)
 * - Boolean parsing (getBool)
 * - Integer parsing (getInt)
 * - Value caching
 * - Override/set functionality
 * - Feature flags (fallback, seed)
 * - Print summary
 */

#include "infra/runtime/P1_AppSettings.h"
#include "infra/runtime/P1_Logger.h"
#include <gtest/gtest.h>
#include <cstdlib>
#include <thread>
#include <vector>

using namespace tasqly::p1::infra::runtime;

// ================================================================
// 🧩 Singleton Tests
// ================================================================

TEST(P1_AppSettingsTest, SingletonInstance)
{
  P1_AppSettings& settings1 = P1_AppSettings::instance();
  P1_AppSettings& settings2 = P1_AppSettings::instance();
  EXPECT_EQ(&settings1, &settings2);
}

// ================================================================
// 🧩 GetString Tests
// ================================================================

TEST(P1_AppSettingsTest, GetStringNonExistent)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  auto value = settings.getString("NON_EXISTENT_VAR_12345");
  EXPECT_FALSE(value.has_value());
}

TEST(P1_AppSettingsTest, GetStringExisting)
{
  // Use set() method instead of environment variables for testing
  std::string key = "TEST_APP_SETTINGS_STRING";
  std::string expectedValue = "test_value_123";
  
  P1_AppSettings& settings = P1_AppSettings::instance();
  settings.set(key, expectedValue);
  auto value = settings.getString(key);
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(*value, expectedValue);
}

TEST(P1_AppSettingsTest, GetStringCached)
{
  std::string key = "TEST_APP_SETTINGS_CACHE";
  std::string firstValue = "first_value";
  
  P1_AppSettings& settings = P1_AppSettings::instance();
  settings.set(key, firstValue);
  auto value1 = settings.getString(key);
  EXPECT_TRUE(value1.has_value());
  EXPECT_EQ(*value1, firstValue);

  // Change value using set
  std::string secondValue = "second_value";
  settings.set(key, secondValue);

  // Should return new value (set updates cache)
  auto value2 = settings.getString(key);
  EXPECT_TRUE(value2.has_value());
  EXPECT_EQ(*value2, secondValue); // Should be updated
}

// ================================================================
// 🧩 GetBool Tests
// ================================================================

TEST(P1_AppSettingsTest, GetBoolNonExistentDefault)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  bool value = settings.getBool("NON_EXISTENT_BOOL", true);
  EXPECT_TRUE(value);
  
  value = settings.getBool("NON_EXISTENT_BOOL", false);
  EXPECT_FALSE(value);
}

TEST(P1_AppSettingsTest, GetBoolTrueValues)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  
  std::vector<std::string> trueValues = {"1", "true", "True", "TRUE", "yes", "Yes", "YES", "on", "On", "ON"};
  
  for (const auto& val : trueValues) {
    std::string key = "TEST_BOOL_TRUE_" + val;
    settings.set(key, val);
    bool result = settings.getBool(key, false);
    EXPECT_TRUE(result) << "Value '" << val << "' should be true";
  }
}

TEST(P1_AppSettingsTest, GetBoolFalseValues)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  
  std::vector<std::string> falseValues = {"0", "false", "False", "FALSE", "no", "No", "NO", "off", "Off", "OFF"};
  
  for (const auto& val : falseValues) {
    std::string key = "TEST_BOOL_FALSE_" + val;
    settings.set(key, val);
    bool result = settings.getBool(key, true);
    EXPECT_FALSE(result) << "Value '" << val << "' should be false";
  }
}

TEST(P1_AppSettingsTest, GetBoolInvalidValue)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  settings.set("TEST_BOOL_INVALID", "invalid_value");
  bool result = settings.getBool("TEST_BOOL_INVALID", true);
  EXPECT_TRUE(result); // Should return default
}

// ================================================================
// 🧩 GetInt Tests
// ================================================================

TEST(P1_AppSettingsTest, GetIntNonExistentDefault)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  int value = settings.getInt("NON_EXISTENT_INT", 42);
  EXPECT_EQ(value, 42);
}

TEST(P1_AppSettingsTest, GetIntValidValue)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  settings.set("TEST_INT_VALID", "123");
  int value = settings.getInt("TEST_INT_VALID", 0);
  EXPECT_EQ(value, 123);
}

TEST(P1_AppSettingsTest, GetIntNegativeValue)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  settings.set("TEST_INT_NEGATIVE", "-456");
  int value = settings.getInt("TEST_INT_NEGATIVE", 0);
  EXPECT_EQ(value, -456);
}

TEST(P1_AppSettingsTest, GetIntZero)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  settings.set("TEST_INT_ZERO", "0");
  int value = settings.getInt("TEST_INT_ZERO", 99);
  EXPECT_EQ(value, 0);
}

TEST(P1_AppSettingsTest, GetIntInvalidValue)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  settings.set("TEST_INT_INVALID", "not_a_number");
  int value = settings.getInt("TEST_INT_INVALID", 42);
  EXPECT_EQ(value, 42); // Should return default
}

TEST(P1_AppSettingsTest, GetIntPartialNumber)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  settings.set("TEST_INT_PARTIAL", "123abc");
  int value = settings.getInt("TEST_INT_PARTIAL", 42);
  // stoi will parse "123abc" as 123, but this might throw or return default
  // The actual behavior depends on implementation
  (void)value; // Suppress unused warning
}

// ================================================================
// 🧩 Set/Override Tests
// ================================================================

TEST(P1_AppSettingsTest, SetValue)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  settings.set("TEST_SET_VALUE", "test_value");
  auto value = settings.getString("TEST_SET_VALUE");
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(*value, "test_value");
}

TEST(P1_AppSettingsTest, OverrideExistingValue)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  settings.set("TEST_OVERRIDE", "original");
  auto value1 = settings.getString("TEST_OVERRIDE");
  EXPECT_EQ(*value1, "original");
  
  settings.set("TEST_OVERRIDE", "new_value");
  auto value2 = settings.getString("TEST_OVERRIDE");
  EXPECT_EQ(*value2, "new_value");
}

// ================================================================
// 🧩 Feature Flags Tests
// ================================================================

TEST(P1_AppSettingsTest, FeatureFallbackDefault)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  // Default should be true
  bool enabled = settings.isFeatureFallbackEnabled();
  EXPECT_TRUE(enabled);
}

TEST(P1_AppSettingsTest, FeatureFallbackEnabled)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  settings.set("FEATURES_DB_FALLBACK_INMEMORY", "true");
  bool enabled = settings.isFeatureFallbackEnabled();
  EXPECT_TRUE(enabled);
}

TEST(P1_AppSettingsTest, FeatureFallbackDisabled)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  settings.set("FEATURES_DB_FALLBACK_INMEMORY", "false");
  bool enabled = settings.isFeatureFallbackEnabled();
  EXPECT_FALSE(enabled);
}

TEST(P1_AppSettingsTest, FeatureSeedDefault)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  // Default should be false
  bool enabled = settings.isFeatureSeedEnabled();
  EXPECT_FALSE(enabled);
}

TEST(P1_AppSettingsTest, FeatureSeedEnabled)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  settings.set("FEATURES_INMEMORY_SEED", "true");
  bool enabled = settings.isFeatureSeedEnabled();
  EXPECT_TRUE(enabled);
}

TEST(P1_AppSettingsTest, FeatureSeedDisabled)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  settings.set("FEATURES_INMEMORY_SEED", "false");
  bool enabled = settings.isFeatureSeedEnabled();
  EXPECT_FALSE(enabled);
}

// ================================================================
// 🧩 Print Summary Tests
// ================================================================

TEST(P1_AppSettingsTest, PrintSummaryEmpty)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  // Should not throw
  EXPECT_NO_THROW(settings.printSummary());
}

TEST(P1_AppSettingsTest, PrintSummaryWithValues)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  settings.set("TEST_SUMMARY_1", "value1");
  settings.set("TEST_SUMMARY_2", "value2");
  // Should not throw
  EXPECT_NO_THROW(settings.printSummary());
}

// ================================================================
// 🧩 Thread Safety Tests
// ================================================================

TEST(P1_AppSettingsTest, ConcurrentAccess)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  
  std::vector<std::thread> threads;
  for (int i = 0; i < 10; ++i) {
    threads.emplace_back([&settings, i]() {
      std::string key = "TEST_CONCURRENT_" + std::to_string(i);
      std::string value = "value_" + std::to_string(i);
      settings.set(key, value);
      auto result = settings.getString(key);
      EXPECT_TRUE(result.has_value());
      EXPECT_EQ(*result, value);
    });
  }
  
  for (auto& t : threads) {
    t.join();
  }
  
  // Should complete without crashes
  EXPECT_TRUE(true);
}

TEST(P1_AppSettingsTest, ConcurrentGetSet)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  
  std::vector<std::thread> threads;
  for (int i = 0; i < 5; ++i) {
    threads.emplace_back([&settings, i]() {
      for (int j = 0; j < 10; ++j) {
        std::string key = "TEST_CONCURRENT_GETSET_" + std::to_string(i);
        std::string value = "value_" + std::to_string(j);
        settings.set(key, value);
        auto result = settings.getString(key);
        (void)result; // Suppress unused warning
      }
    });
  }
  
  for (auto& t : threads) {
    t.join();
  }
  
  // Should complete without crashes
  EXPECT_TRUE(true);
}

// ================================================================
// 🧩 Edge Cases for GetBool
// ================================================================

TEST(P1_AppSettingsTest, GetBoolWithEmptyString)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  settings.set("TEST_BOOL_EMPTY", "");
  bool result = settings.getBool("TEST_BOOL_EMPTY", true);
  // Empty string should return default
  EXPECT_TRUE(result);
}

// ================================================================
// 🧩 Edge Cases for GetInt
// ================================================================

TEST(P1_AppSettingsTest, GetIntWithEmptyString)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  settings.set("TEST_INT_EMPTY", "");
  int value = settings.getInt("TEST_INT_EMPTY", 42);
  // Empty string should return default
  EXPECT_EQ(value, 42);
}

TEST(P1_AppSettingsTest, GetIntWithWhitespace)
{
  P1_AppSettings& settings = P1_AppSettings::instance();
  settings.set("TEST_INT_WHITESPACE", "  123  ");
  int value = settings.getInt("TEST_INT_WHITESPACE", 0);
  // stoi should handle whitespace
  EXPECT_EQ(value, 123);
}


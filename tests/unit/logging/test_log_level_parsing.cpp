/*
 * 🧱 File: test_log_level_parsing.cpp
 * -----------------------------------
 * 📌 Purpose   : Unit tests for log level parsing and enable/disable logic.
 * 🧱 Layer     : Tests (Unit - Infrastructure)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-03
 * 🗓️ Updated   : 2025-09-25
 * 🔖 Version   : 2.0 (Aligned with C++ Standards)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Verifies the correctness of:
 * - Conversion from Level enum to string via LogManager::levelToString
 * - Default log level state
 * - Behavior of isEnabled() when changing log levels
 *
 * 🔗 Depends On:
 *   - infra/logging/LogManager.h
 * 🔗 Related   : test_log_manager_misc.cpp, test_masking.cpp
 * ✅ Tested In : TasqlyTestsRunner
 * ⚠️ Errors    : N/A
 * 🕒 Last Major Change: 2025-09-25
 */

#include <gtest/gtest.h>

// Project
#include "infra/logging/LogManager.h"

using tasqly::app::logging::Level;
using tasqly::infra::logging::LogManager;

// 🧪 Test 1: levelToString must return expected string values
TEST(LogLevelParsingTests, LevelToStringMapping)
{
  EXPECT_EQ(LogManager::levelToString(Level::Trace), "TRACE");
  EXPECT_EQ(LogManager::levelToString(Level::Debug), "DEBUG");
  EXPECT_EQ(LogManager::levelToString(Level::Info), "INFO");
  EXPECT_EQ(LogManager::levelToString(Level::Warn), "WARN");
  EXPECT_EQ(LogManager::levelToString(Level::Error), "ERROR");
  EXPECT_EQ(LogManager::levelToString(Level::Critical), "CRITICAL");
  EXPECT_EQ(LogManager::levelToString(Level::Off), "OFF");
}

// 🧪 Test 2: default log level must be INFO
TEST(LogLevelParsingTests, DefaultLogLevel)
{
  LogManager lm;
  EXPECT_EQ(lm.level(), Level::Info);
  EXPECT_TRUE(lm.isEnabled(Level::Info));
  EXPECT_FALSE(lm.isEnabled(Level::Debug)); // lower than Info
}

// 🧪 Test 3: raising threshold disables lower levels
TEST(LogLevelParsingTests, RaiseThreshold)
{
  LogManager lm;
  lm.setLevel(Level::Warn);

  EXPECT_TRUE(lm.isEnabled(Level::Warn));
  EXPECT_TRUE(lm.isEnabled(Level::Error));
  EXPECT_FALSE(lm.isEnabled(Level::Info));  // filtered out
  EXPECT_FALSE(lm.isEnabled(Level::Debug)); // filtered out
}

// 🧪 Test 4: Level::Off disables all logging
TEST(LogLevelParsingTests, LevelOffDisablesAll)
{
  LogManager lm;
  lm.setLevel(Level::Off);

  EXPECT_FALSE(lm.isEnabled(Level::Trace));
  EXPECT_FALSE(lm.isEnabled(Level::Error));
  EXPECT_FALSE(lm.isEnabled(Level::Critical));
}

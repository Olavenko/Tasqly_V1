/*
 * 🧱 File: test_log_level_parsing.cpp
 * -----------------------------------
 * 📌 Purpose   : Unit tests for log level parsing and enable/disable logic.
 * 🧱 Layer     : Tests (Unit)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-03
 *
 * 🧠 Description:
 * Verifies the correctness of:
 * - Conversion from Level enum to string via LogManager::levelToString
 * - Default log level state
 * - Behavior of isEnabled() when changing log levels
 */

#include "src/infra/logging/LogManager.h"
#include <gtest/gtest.h>

using namespace tasqly::infra::logging;
using tasqly::app::logging::Level;

// ✅ Test: levelToString returns expected values
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

// ✅ Test: default log level is INFO
TEST(LogLevelParsingTests, DefaultLogLevel)
{
  LogManager lm;
  EXPECT_EQ(lm.level(), Level::Info);
  EXPECT_TRUE(lm.isEnabled(Level::Info));
  EXPECT_FALSE(lm.isEnabled(Level::Debug)); // lower than Info
}

// ✅ Test: raising threshold disables lower levels
TEST(LogLevelParsingTests, RaiseThreshold)
{
  LogManager lm;
  lm.setLevel(Level::Warn);

  EXPECT_TRUE(lm.isEnabled(Level::Warn));
  EXPECT_TRUE(lm.isEnabled(Level::Error));
  EXPECT_FALSE(lm.isEnabled(Level::Info)); // filtered out
  EXPECT_FALSE(lm.isEnabled(Level::Debug));
}

// ✅ Test: level Off disables all logging
TEST(LogLevelParsingTests, LevelOffDisablesAll)
{
  LogManager lm;
  lm.setLevel(Level::Off);

  EXPECT_FALSE(lm.isEnabled(Level::Trace));
  EXPECT_FALSE(lm.isEnabled(Level::Error));
  EXPECT_FALSE(lm.isEnabled(Level::Critical));
}

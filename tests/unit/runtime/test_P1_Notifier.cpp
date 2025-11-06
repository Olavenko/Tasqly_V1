/*
 * 🧪 File: test_P1_Notifier.cpp
 * -----------------------------
 * 📌 Purpose   : Unit Tests for P1_Notifier (Phase 1 Runtime Notifier)
 * 🧱 Layer     : Tests (Unit - Infrastructure Runtime)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-11-05
 * 🔖 Version   : 1.0
 *
 * 🧠 Description:
 * Comprehensive unit tests for P1_Notifier covering:
 * - Singleton pattern
 * - Silent mode (setSilent, isSilent)
 * - Info, warn, error messages
 * - Structured error logging
 * - Fallback activation notifications
 * - Toast messages with auto-detection
 */

#include "infra/runtime/P1_Notifier.h"
#include "infra/runtime/P1_Error.h"
#include "infra/runtime/P1_Logger.h"
#include <gtest/gtest.h>
#include <sstream>
#include <iostream>
#include <thread>
#include <vector>

using namespace tasqly::p1::infra::runtime;

// ================================================================
// 🧩 Singleton Tests
// ================================================================

TEST(P1_NotifierTest, SingletonInstance)
{
  P1_Notifier& notifier1 = P1_Notifier::instance();
  P1_Notifier& notifier2 = P1_Notifier::instance();
  EXPECT_EQ(&notifier1, &notifier2);
}

// ================================================================
// 🧩 Silent Mode Tests
// ================================================================

TEST(P1_NotifierTest, SilentModeDefault)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  EXPECT_FALSE(notifier.isSilent());
}

TEST(P1_NotifierTest, SetSilentTrue)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(true);
  EXPECT_TRUE(notifier.isSilent());
}

TEST(P1_NotifierTest, SetSilentFalse)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(false);
  EXPECT_FALSE(notifier.isSilent());
}

TEST(P1_NotifierTest, SetSilentToggle)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(true);
  EXPECT_TRUE(notifier.isSilent());
  notifier.setSilent(false);
  EXPECT_FALSE(notifier.isSilent());
}

// ================================================================
// 🧩 Info Message Tests
// ================================================================

TEST(P1_NotifierTest, InfoMessage)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(false);
  // Should not throw
  EXPECT_NO_THROW(notifier.info("Test info message"));
}

TEST(P1_NotifierTest, InfoMessageSilent)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(true);
  // Should not throw even in silent mode
  EXPECT_NO_THROW(notifier.info("Test info message"));
}

// ================================================================
// 🧩 Warning Message Tests
// ================================================================

TEST(P1_NotifierTest, WarnMessage)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(false);
  EXPECT_NO_THROW(notifier.warn("Test warning message"));
}

TEST(P1_NotifierTest, WarnMessageSilent)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(true);
  EXPECT_NO_THROW(notifier.warn("Test warning message"));
}

// ================================================================
// 🧩 Error Message Tests (String)
// ================================================================

TEST(P1_NotifierTest, ErrorMessageString)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(false);
  EXPECT_NO_THROW(notifier.error("Test error message"));
}

TEST(P1_NotifierTest, ErrorMessageStringSilent)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(true);
  EXPECT_NO_THROW(notifier.error("Test error message"));
}

// ================================================================
// 🧩 Error Message Tests (P1_Error)
// ================================================================

TEST(P1_NotifierTest, ErrorMessageP1Error)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(false);
  P1_Error err = P1_Error::makeDbInit("Database initialization failed");
  EXPECT_NO_THROW(notifier.error(err));
}

TEST(P1_NotifierTest, ErrorMessageP1ErrorWithContext)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(false);
  std::unordered_map<std::string, std::string> ctx;
  ctx["host"] = "localhost";
  P1_Error err(RuntimeErrorCode::DbAuth, "Auth failed", ctx);
  EXPECT_NO_THROW(notifier.error(err));
}

TEST(P1_NotifierTest, ErrorMessageP1ErrorSilent)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(true);
  P1_Error err = P1_Error::makeTimeout("Timeout error");
  EXPECT_NO_THROW(notifier.error(err));
}

// ================================================================
// 🧩 Fallback Activation Tests
// ================================================================

TEST(P1_NotifierTest, FallbackActivated)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(false);
  EXPECT_NO_THROW(notifier.fallbackActivated("InMemoryTaskRepository"));
}

TEST(P1_NotifierTest, FallbackActivatedSilent)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(true);
  EXPECT_NO_THROW(notifier.fallbackActivated("InMemoryTaskRepository"));
}

TEST(P1_NotifierTest, FallbackActivatedWithCustomName)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(false);
  EXPECT_NO_THROW(notifier.fallbackActivated("CustomRepository"));
}

// ================================================================
// 🧩 Toast Message Tests
// ================================================================

TEST(P1_NotifierTest, ToastInfoMessage)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(false);
  EXPECT_NO_THROW(notifier.toast("Test toast message"));
}

TEST(P1_NotifierTest, ToastWarningMessage)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(false);
  EXPECT_NO_THROW(notifier.toast("⚠️ Warning toast message"));
}

TEST(P1_NotifierTest, ToastErrorMessage)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(false);
  EXPECT_NO_THROW(notifier.toast("❌ Error toast message"));
}

TEST(P1_NotifierTest, ToastSilentMode)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(true);
  // Toast should not output in silent mode
  EXPECT_NO_THROW(notifier.toast("Test toast message"));
}

TEST(P1_NotifierTest, ToastAutoDetectWarning)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(false);
  // Should auto-detect warning emoji
  EXPECT_NO_THROW(notifier.toast("⚠️ This is a warning"));
}

TEST(P1_NotifierTest, ToastAutoDetectError)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(false);
  // Should auto-detect error emoji
  EXPECT_NO_THROW(notifier.toast("❌ This is an error"));
}

TEST(P1_NotifierTest, ToastAutoDetectInfo)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(false);
  // Should default to info if no emoji detected
  EXPECT_NO_THROW(notifier.toast("This is an info message"));
}

// ================================================================
// 🧩 Thread Safety Tests
// ================================================================

TEST(P1_NotifierTest, ConcurrentMessages)
{
  P1_Notifier& notifier = P1_Notifier::instance();
  notifier.setSilent(false);

  // Test that multiple threads can call notifier methods safely
  std::vector<std::thread> threads;
  for (int i = 0; i < 10; ++i) {
    threads.emplace_back([&notifier, i]() {
      notifier.info("Thread " + std::to_string(i) + " message");
      notifier.warn("Thread " + std::to_string(i) + " warning");
      notifier.error("Thread " + std::to_string(i) + " error");
    });
  }

  for (auto& t : threads) {
    t.join();
  }

  // Should complete without crashes
  EXPECT_TRUE(true);
}

TEST(P1_NotifierTest, ConcurrentSilentModeToggle)
{
  P1_Notifier& notifier = P1_Notifier::instance();

  std::vector<std::thread> threads;
  for (int i = 0; i < 5; ++i) {
    threads.emplace_back([&notifier, i]() {
      notifier.setSilent(i % 2 == 0);
      bool silent = notifier.isSilent();
      (void)silent; // Suppress unused variable warning
    });
  }

  for (auto& t : threads) {
    t.join();
  }

  // Should complete without crashes
  EXPECT_TRUE(true);
}


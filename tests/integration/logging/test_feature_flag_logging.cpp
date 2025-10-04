/*
 * 🧱 File: test_feature_flag_logging.cpp
 * -------------------------------------
 * 📌 Purpose   : Integration test — verify that logging subsystem respects `features.logging` flag.
 * 🧱 Layer     : Tests (Integration)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-03
 *
 * 🧠 Description:
 * Ensures that when `features.logging` is disabled via FeatureFlagsManager,
 * LogManager does not produce log output. When enabled, logs are written normally.
 */

#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include "app/settings/FeatureFlagsManager.h"
#include "infra/logging/LogManager.h"

using namespace tasqly::infra::logging;

TEST(Integration_FeatureFlags, LoggingRespectsFeatureFlag)
{
  auto& features = FeatureFlagsManager::instance();
  LogManager logger;
  logger.setAutoFlush(true);

  QDir dir((QCoreApplication::applicationDirPath()));
  QString logPath = dir.filePath("tests_tmp/feature_flag_logging.log");
  dir.mkpath("tests_tmp");

  logger.enableFileSink(true, logPath);

  // 🟢 Case 1: Enable logging → expect output
  features.setFeature("features.logging", true);
  logger.log(LogManager::Level::Info, "test_enabled", "Message should appear");
  logger.flush(); // ✅ force flush

  QFile file1(logPath);
  ASSERT_TRUE(file1.open(QIODevice::ReadOnly | QIODevice::Text));
  QString contents1 = QString::fromUtf8(file1.readAll());
  file1.close();

  EXPECT_TRUE(contents1.contains("Message should appear"))
      << "Expected log message not found when logging enabled";

  // 🔴 Case 2: Disable logging → expect no new output
  features.setFeature("features.logging", false);
  logger.log(LogManager::Level::Info, "test_disabled", "Message should NOT appear");
  logger.flush(); // ✅ force flush

  QFile file2(logPath);
  ASSERT_TRUE(file2.open(QIODevice::ReadOnly | QIODevice::Text));
  QString contents2 = QString::fromUtf8(file2.readAll());
  file2.close();

  EXPECT_EQ(contents1, contents2)
      << "Log file changed even though logging feature flag was disabled";
}

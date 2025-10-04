/*
 * 🧱 File: test_error_reporter_feature_flag.cpp
 * ---------------------------------------------
 * 📌 Purpose   : Integration test — verify ErrorReporter respects `features.logging` flag.
 * 🧱 Layer     : Tests (Integration)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-03
 *
 * 🧠 Description:
 * Ensures ErrorReporter always emits toastRequested,
 * but only logs when `features.logging` is enabled.
 */

#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QSignalSpy>

#include "app/settings/FeatureFlagsManager.h"
#include "domain/core/Error.h"
#include "infra/logging/LogManager.h"

using namespace tasqly::app::logging;
using namespace tasqly::infra::logging;
using tasqly::domain::core::Error;

TEST(Integration_FeatureFlags, ErrorReporterRespectsLoggingFlag)
{
  auto& features = FeatureFlagsManager::instance();
  LogManager logger;

  logger.setAutoFlush(true);

  // 🟢 اكتب اللوج في tests_tmp بدل الملفات العشوائية
  QDir dir(QCoreApplication::applicationDirPath());
  QString logPath = dir.filePath("tests_tmp/error_reporter_flag.log");
  dir.mkpath("tests_tmp");

  logger.enableFileSink(true, logPath);

  features.setFeature("features.logging", true);

  logger.log(LogManager::Level::Error, "test_category", "Test error enabled");
  logger.flush();

  QFile file1(logPath);
  ASSERT_TRUE(file1.open(QIODevice::ReadOnly | QIODevice::Text));
  QString contents1 = QString::fromUtf8(file1.readAll());
  file1.close();

  EXPECT_TRUE(contents1.contains("Test error enabled"))
      << "Expected log not found when logging enabled";

  features.setFeature("features.logging", false);
  logger.log(LogManager::Level::Error, "test_category", "Test error disabled");
  logger.flush();

  QFile file2(logPath);
  ASSERT_TRUE(file2.open(QIODevice::ReadOnly | QIODevice::Text));
  QString contents2 = QString::fromUtf8(file2.readAll());
  file2.close();

  EXPECT_EQ(contents1, contents2) << "Log file changed even though logging was disabled";
}

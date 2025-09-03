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

#include <QFile>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <gtest/gtest.h>

#include "src/app/logging/ErrorReporter.h"
#include "src/app/settings/FeatureFlagsManager.h"
#include "src/domain/core/Error.h"
#include "src/infra/logging/LogManager.h"

using namespace tasqly::app::logging;
using namespace tasqly::infra::logging;
using tasqly::domain::core::Error;

TEST(Integration_FeatureFlags, ErrorReporterRespectsLoggingFlag)
{
  auto& features = FeatureFlagsManager::instance();

  // 🔧 Setup: temporary log file
  QTemporaryFile tmpFile;
  ASSERT_TRUE(tmpFile.open());
  QString logPath = tmpFile.fileName();
  tmpFile.close();

  auto logger = std::make_shared<LogManager>();
  logger->enableFileSink(true, logPath);

  ErrorReporter reporter(logger);
  QSignalSpy spy(&reporter, &ErrorReporter::toastRequested);

  // 🟢 Case 1: logging enabled → expect file output + toast
  features.setFeature("features.logging", true);

  Error err1(1001, "Test error enabled", {});
  reporter.report(err1, Severity::Error, "test_category");

  // check toast emitted
  EXPECT_EQ(spy.count(), 1) << "Toast not emitted when logging enabled";

  // check log file written
  QFile file1(logPath);
  ASSERT_TRUE(file1.open(QIODevice::ReadOnly | QIODevice::Text));
  QString contents1 = QString::fromUtf8(file1.readAll());
  file1.close();
  EXPECT_TRUE(contents1.contains("Test error enabled"))
      << "Expected log not found when logging enabled";

  spy.clear();

  // 🔴 Case 2: logging disabled → expect NO file output but toast still emitted
  features.setFeature("features.logging", false);

  Error err2(1002, "Test error disabled", {});
  reporter.report(err2, Severity::Error, "test_category");

  // check toast emitted again
  EXPECT_EQ(spy.count(), 1) << "Toast not emitted when logging disabled";

  // check file unchanged
  QFile file2(logPath);
  ASSERT_TRUE(file2.open(QIODevice::ReadOnly | QIODevice::Text));
  QString contents2 = QString::fromUtf8(file2.readAll());
  file2.close();
  EXPECT_EQ(contents1, contents2) << "Log file changed even though logging was disabled";
}

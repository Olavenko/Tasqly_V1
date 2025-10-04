/*
 * 🧱 File: test_masking_concurrency.cpp
 * ------------------------------------
 * 📌 Purpose   : Ensure masking remains thread-safe under concurrent logging.
 * 🧱 Layer     : Tests (Unit - Infrastructure)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-04
 * 🗓️ Updated   : 2025-09-25
 * 🔖 Version   : 2.0 (Strict validation & cleanup)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Spawns multiple threads logging with sensitive context fields and verifies
 * that all lines are properly masked, indicating absence of data races.
 * Enhancements in v2:
 * - Rotated logs cleanup before run
 * - Strict equality on expected line count (200 total)
 *
 * 🔗 Depends On:
 *   - infra/logging/LogManager.h
 *   - app/settings/FeatureFlagsManager.h
 * 🔗 Related   : test_rotation_policy.cpp, test_log_manager_misc.cpp
 * ✅ Tested In : TasqlyTestsRunner
 * ⚠️ Errors    : Test will fail if masking is partial or if any race condition skips lines
 * 🕒 Last Major Change: 2025-09-25
 */

#include <gtest/gtest.h>

// Qt
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFuture>
#include <QVariantMap>
#include <QtConcurrent/qtconcurrentrun.h>

// Project
#include "app/settings/FeatureFlagsManager.h"
#include "infra/logging/LogManager.h"

using tasqly::app::logging::Level;
using tasqly::infra::logging::LogManager;

/// 🧪 Test fixture: ensures logging feature flag is enabled
class MaskingConcurrencyTests : public ::testing::Test
{
protected:
  void SetUp() override { FeatureFlagsManager::instance().setFeature("features.logging", true); }
};

/// 🧪 Test: masking must remain correct under concurrent writes
TEST_F(MaskingConcurrencyTests, MaskingIsThreadSafe)
{
  QDir dir(QCoreApplication::applicationDirPath());
  dir.mkpath("tests_tmp");
  QString basePath = dir.filePath("tests_tmp/masking_concurrency.log");

  // Cleanup base + rotated logs
  QFile::remove(basePath);
  for (int i = 1; i <= 3; ++i) {
    QFile::remove(basePath + "." + QString::number(i));
  }

  LogManager lm;
  lm.enableConsole(false);
  lm.enableFileSink(true, basePath);

  // Worker lambda: log 50 entries with sensitive fields
  auto worker = [&lm]() {
    for (int i = 0; i < 50; ++i) {
      QVariantMap ctx;
      ctx["password"] = QString("p%1").arg(i);
      ctx["token"] = QString("t%1").arg(i);
      lm.log(Level::Info, "mask.concurrent", "msg", ctx);
    }
  };

  // Run 4 concurrent workers
  QList<QFuture<void>> futures;
  for (int t = 0; t < 4; ++t) {
    futures << QtConcurrent::run(worker);
  }
  for (auto& f : futures) {
    f.waitForFinished();
  }

  lm.flush();
  lm.enableFileSink(false);

  // Verify file contents
  QFile f(basePath);
  ASSERT_TRUE(f.open(QIODevice::ReadOnly));
  QByteArray content = f.readAll();
  f.close();

  QList<QByteArray> lines = content.split('\n');
  int checked = 0;
  for (const auto& line : std::as_const(lines)) {
    if (line.isEmpty())
      continue;
    ++checked;
    ASSERT_NE(line.indexOf("password=***"), -1);
    ASSERT_NE(line.indexOf("token=***"), -1);
  }

  // Expect exactly 200 lines (4 threads × 50 entries each)
  EXPECT_EQ(checked, 200);
}

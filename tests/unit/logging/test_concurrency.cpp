/*
 * 🧱 File: test_concurrency.cpp
 * -----------------------------
 * 📌 Purpose   : Stress test concurrent logging.
 * 🧱 Layer     : Tests (Unit - Infrastructure)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-03
 * 🗓️ Updated   : 2025-09-25
 * 🔖 Version   : 2.0 (Stricter validation & cleanup)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Spawns multiple threads writing logs in parallel.
 * Verifies:
 * - No crash or deadlock under load
 * - All log entries are persisted (strict count check)
 *
 * 🔗 Depends On:
 *   - infra/logging/LogManager.h
 *   - app/settings/FeatureFlagsManager.h
 * 🔗 Related   : test_masking_concurrency.cpp
 * ✅ Tested In : TasqlyTestsRunner
 * ⚠️ Errors    : Test fails if logs are lost or concurrency introduces corruption
 * 🕒 Last Major Change: 2025-09-25
 */

#include <gtest/gtest.h>

// Qt
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFuture>
#include <QtConcurrent/qtconcurrentrun.h>

// Project
#include "app/settings/FeatureFlagsManager.h"
#include "infra/logging/LogManager.h"

using tasqly::app::logging::Level;
using tasqly::infra::logging::LogManager;

/// 🧪 Fixture: enable logging and provide cleanup
class ConcurrencyTests : public ::testing::Test
{
protected:
  void SetUp() override { FeatureFlagsManager::instance().setFeature("features.logging", true); }

  void cleanup(const QString& basePath)
  {
    QFile::remove(basePath);
    for (int i = 1; i <= 3; ++i) {
      QFile::remove(basePath + "." + QString::number(i));
    }
  }
};

// 🧪 Test: multi-threaded logging must persist all entries
TEST_F(ConcurrencyTests, MultiThreadedLogging)
{
  QDir dir((QCoreApplication::applicationDirPath()));
  dir.mkpath("tests_tmp");
  QString basePath = dir.filePath("tests_tmp/concurrency.log");

  cleanup(basePath);

  LogManager lm;
  lm.enableConsole(false);
  lm.enableFileSink(true, basePath);

  auto worker = [&lm]() {
    for (int i = 0; i < 100; ++i) {
      lm.log(Level::Info, "test.concurrent", QString("msg %1").arg(i));
    }
  };

  // Run 4 concurrent workers (400 log lines expected)
  QList<QFuture<void>> futures;
  for (int t = 0; t < 4; ++t) {
    futures << QtConcurrent::run(worker);
  }
  for (auto& f : futures) {
    f.waitForFinished();
  }

  lm.flush();
  lm.enableFileSink(false);

  QFile f(basePath);
  ASSERT_TRUE(f.open(QIODevice::ReadOnly));
  QByteArray content = f.readAll();
  f.close();

  QList<QByteArray> lines = content.split('\n');
  int counted = 0;
  for (const auto& line : std::as_const(lines)) {
    if (!line.isEmpty())
      ++counted;
  }

  EXPECT_EQ(counted, 400); // strict check: all lines must exist
}

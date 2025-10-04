/*
 * 🧱 File: test_log_manager_misc.cpp
 * ----------------------------------
 * 📌 Purpose   : Miscellaneous unit tests for LogManager (infra logging).
 * 🧱 Layer     : Tests (Unit - Infrastructure)
 * 👤 Author    : Tasqly Team
 * 🗓️ Created   : 2025-09-23
 * 🗓️ Updated   : 2025-09-25
 * 🔖 Version   : 2.0 (Adapted to Debounced Rotation Strategy)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Covers LogManager features not tested in other suites:
 * - Console sink toggle
 * - Flush behavior (manual required)
 * - Async rotation scheduling (debounce/periodic)
 * - DefaultLogPath accessor
 * - Silent mode
 * - FeatureFlag disable
 *
 * 🔗 Depends On:
 *   - infra/logging/LogManager.h
 *   - app/settings/FeatureFlagsManager.h
 *   - tests/fakes/LogManagerTestAccessor.h
 * 🔗 Related   : Bench_LogRotation.cpp
 * ✅ Tested In : TasqlyTestsRunner
 * ⚠️ Errors    : File I/O errors fallback to console
 * 🧭 Benchmark Measurement Policy: N/A (unit tests only)
 * 🕒 Last Major Change: 2025-09-25
 */

#include <atomic>
#include <gtest/gtest.h>

// Qt
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>

// Project
#include "app/settings/FeatureFlagsManager.h"
#include "infra/logging/LogManager.h"
#include "tests/fakes/LogManagerTestAccessor.h"

using tasqly::app::logging::Level;
using tasqly::infra::logging::LogManager;

// 🛠️ Helper: read file content as QString
static QString readFile(const QString& path)
{
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly)) {
    return {};
  }
  QString content = QString::fromUtf8(f.readAll());
  f.close();
  return content;
}

// 🧪 Test 1: Console sink toggle must not crash
TEST(LogManagerMiscTests, ConsoleSinkToggle)
{
  LogManager lm;
  lm.enableConsole(true);
  lm.enableConsole(false);
  SUCCEED(); // pass if no crash
}

// 🧪 Test 2: Flush must persist data to file
TEST(LogManagerMiscTests, FlushWritesData)
{
  QDir dir(QCoreApplication::applicationDirPath());
  dir.mkpath("tests_tmp");
  QString basePath = dir.filePath("tests_tmp/misc_flush.log");
  QFile::remove(basePath);

  LogManager lm;
  lm.enableConsole(false);
  lm.enableFileSink(true, basePath);

  lm.log(Level::Info, "misc", "hello flush");
  lm.flush();
  lm.enableFileSink(false);

  QString content = readFile(basePath);
  EXPECT_TRUE(content.contains("hello flush"));
}

// 📝 Test 3: Manual flush required (adapted for new behavior)
TEST(LogManagerMiscTests, ManualFlushRequired)
{
  QDir dir(QCoreApplication::applicationDirPath());
  dir.mkpath("tests_tmp");
  QString basePath = dir.filePath("tests_tmp/misc_manualflush.log");
  QFile::remove(basePath);

  LogManager lm;
  lm.enableConsole(false);
  lm.enableFileSink(true, basePath);

  lm.log(Level::Info, "misc", "needs flush");

  // قبل ما نقفل sink → الملف غالبًا فاضي
  QString preClose = readFile(basePath);
  EXPECT_TRUE(preClose.isEmpty());

  // بعد ما نقفل sink → ممكن يحصل flush تلقائي
  lm.enableFileSink(false);
  QString afterClose = readFile(basePath);
  EXPECT_TRUE(afterClose.isEmpty() || afterClose.contains("needs flush"));

  // reopen + log + flush → لازم ي persist
  lm.enableFileSink(true, basePath);
  lm.log(Level::Info, "misc", "flushed content");
  lm.flush();
  lm.enableFileSink(false);

  QString content2 = readFile(basePath);
  EXPECT_TRUE(content2.contains("flushed content"));
}

// 🧪 Test 4: RotateNow schedules async rotation (must not crash)
TEST(LogManagerMiscTests, RotateNowSchedulesRotation)
{
  LogManager lm;
  lm.enableConsole(false);
  // even if no file sink enabled, should not crash
  lm.rotateNow();
  SUCCEED();
}

// 🧪 Test 5: Default log path must end with "tasqly.log"
TEST(LogManagerMiscTests, DefaultLogPathEndsWithTasqlyLog)
{
  LogManager lm;
  QString def = LogManagerTestAccessor::defaultLogPath(lm);
  EXPECT_TRUE(def.endsWith("tasqly.log"));
}

// 🧪 Test 6: Silent mode disables logging
TEST(LogManagerMiscTests, SilentModeDisablesLogging)
{
  QDir dir(QCoreApplication::applicationDirPath());
  dir.mkpath("tests_tmp");
  QString basePath = dir.filePath("tests_tmp/misc_silent.log");
  QFile::remove(basePath);

  LogManager lm(nullptr, true); // silent mode
  lm.enableConsole(false);
  lm.enableFileSink(true, basePath);
  lm.log(Level::Info, "misc", "this should not appear");
  lm.flush();
  lm.enableFileSink(false);

  QString content = readFile(basePath);
  EXPECT_TRUE(content.isEmpty());
}

// 🧪 Test 7: Feature flag disables logging when turned off
TEST(LogManagerMiscTests, FeatureFlagDisableSuppressesLogging)
{
  QDir dir(QCoreApplication::applicationDirPath());
  dir.mkpath("tests_tmp");
  QString basePath = dir.filePath("tests_tmp/misc_flagdisable.log");
  QFile::remove(basePath);

  FeatureFlagsManager::instance().setFeature("features.logging", false);

  LogManager lm;
  lm.enableConsole(false);
  lm.enableFileSink(true, basePath);
  lm.log(Level::Info, "misc", "should be suppressed");
  lm.flush();
  lm.enableFileSink(false);

  QString content = readFile(basePath);
  EXPECT_TRUE(content.isEmpty());

  // reset for other tests
  FeatureFlagsManager::instance().setFeature("features.logging", true);
}

// 🧪 Test 8: log() must be a no-op when every sink is disabled
TEST(LogManagerMiscTests, LogNoOpWhenAllSinksDisabled)
{
  LogManager lm;
  lm.enableConsole(false);
  lm.enableFileSink(false);

  std::atomic<int> formatted{0};
  LogManagerTestAccessor::setFormatHook(lm,
                                        [&] { formatted.fetch_add(1, std::memory_order_relaxed); });

  lm.log(Level::Info, "misc", "suppressed message");

  EXPECT_EQ(0, formatted.load(std::memory_order_relaxed));

  // cleanup hook to avoid leaking state into other tests
  LogManagerTestAccessor::setFormatHook(lm, {});
}

/*
 * 🧱 File: test_periodic_rotation.cpp
 * -----------------------------------
 * 📌 Purpose   : Verify that periodic timer forces rotation after delay.
 * 🧱 Layer     : Tests (Unit - Infrastructure)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-27
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Ensures that even if rotateNow() is not called, the periodic rotation timer
 * (5s) will eventually perform rotation once maxBytes is exceeded.
 *
 * 🔗 Depends On:
 *   - infra/logging/LogManager.h
 * 🔗 Related   : test_rotation_policy.cpp
 * ✅ Tested In : TasqlyTestsRunner
 */

#include <gtest/gtest.h>

// Qt
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTest>
#include <QThread>

// Project
#include "infra/logging/LogManager.h"

using tasqly::app::logging::Level;
using tasqly::infra::logging::LogManager;

TEST(LogManagerPeriodicRotationTests, PeriodicTimerForcesRotation)
{
  QDir dir(QCoreApplication::applicationDirPath());
  dir.mkpath("tests_tmp");
  QString basePath = dir.filePath("tests_tmp/periodic_rotation.log");

  QFile::remove(basePath);
  QFile::remove(basePath + ".1");

  LogManager lm;
  lm.enableConsole(false);
  lm.enableFileSink(true, basePath);
  lm.setRotationPolicy({128, 1}); // tiny size, 1 rotated file max

  // Write enough data to exceed 128 bytes
  QString payload(64, 'X');
  for (int i = 0; i < 5; ++i) {
    lm.log(Level::Info, "periodic", payload);
  }

  // Run event loop for >5s to let periodic timer fire
  QElapsedTimer t;
  t.start();
  while (t.elapsed() < 6000) {
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
  }

  lm.flush();
  lm.enableFileSink(false);

  QFileInfo rotated(basePath + ".1");
  EXPECT_TRUE(rotated.exists()) << "Periodic timer should have rotated the log file";
}

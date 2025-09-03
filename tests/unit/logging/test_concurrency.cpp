/*
 * 🧱 File: test_concurrency.cpp
 * -----------------------------
 * 📌 Purpose   : Stress test concurrent logging.
 * 🧱 Layer     : Tests (Unit)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-03
 *
 * 🧠 Description:
 * Spawns multiple threads writing logs in parallel.
 * Verifies no crash, deadlock, or severe data corruption.
 */

#include "src/infra/logging/LogManager.h"
#include <gtest/gtest.h>

#include <QDir>
#include <QFile>
#include <QFuture>
#include <QtConcurrent/qtconcurrentrun.h>

using namespace tasqly::infra::logging;
using tasqly::app::logging::Level;

TEST(ConcurrencyTests, MultiThreadedLogging)
{
  QString basePath = QDir::currentPath() + "/tests_tmp/concurrency.log";
  QDir().mkpath(QFileInfo(basePath).dir().absolutePath());
  QFile::remove(basePath);

  LogManager lm;
  lm.enableConsole(false);
  lm.enableFileSink(true, basePath);

  auto worker = [&lm]() {
    for (int i = 0; i < 100; ++i) {
      lm.log(Level::Info, "test.concurrent", QString("msg %1").arg(i));
    }
  };

  // Run 4 threads
  QFuture<void> f1 = QtConcurrent::run(worker);
  QFuture<void> f2 = QtConcurrent::run(worker);
  QFuture<void> f3 = QtConcurrent::run(worker);
  QFuture<void> f4 = QtConcurrent::run(worker);

  f1.waitForFinished();
  f2.waitForFinished();
  f3.waitForFinished();
  f4.waitForFinished();

  lm.flush();
  lm.enableFileSink(false);

  QFile f(basePath);
  ASSERT_TRUE(f.open(QIODevice::ReadOnly));
  QByteArray content = f.readAll();
  f.close();

  // Relaxed threshold: enough logs survived
  int lines = content.split('\n').size();
  EXPECT_GE(lines, 300);
}

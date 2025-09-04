/*
 * 🧱 File: test_masking_concurrency.cpp
 * ------------------------------------
 * 📌 Purpose   : Ensure masking remains thread-safe under concurrent logging.
 * 🧱 Layer     : Tests (Unit)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-04
 *
 * 🧠 Description:
 * Spawns multiple threads logging with sensitive context fields and verifies
 * that all lines are properly masked, indicating absence of data races.
 */

#include "src/infra/logging/LogManager.h"
#include <gtest/gtest.h>

#include <QDir>
#include <QFile>
#include <QFuture>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QVariantMap>

using namespace tasqly::infra::logging;
using tasqly::app::logging::Level;

TEST(MaskingConcurrencyTests, MaskingIsThreadSafe)
{
  QString basePath = QDir::currentPath() + "/tests_tmp/masking_concurrency.log";
  QDir().mkpath(QFileInfo(basePath).dir().absolutePath());
  QFile::remove(basePath);

  LogManager lm;
  lm.enableConsole(false);
  lm.enableFileSink(true, basePath);

  auto worker = [&lm]() {
    for (int i = 0; i < 50; ++i) {
      QVariantMap ctx;
      ctx["password"] = QString("p%1").arg(i);
      ctx["token"] = QString("t%1").arg(i);
      lm.log(Level::Info, "mask.concurrent", "msg", ctx);
    }
  };

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
  int checked = 0;
  for (const auto& line : lines) {
    if (line.isEmpty())
      continue;
    ++checked;
    ASSERT_NE(line.indexOf("password=***"), -1);
    ASSERT_NE(line.indexOf("token=***"), -1);
  }
  EXPECT_GE(checked, 150);
}


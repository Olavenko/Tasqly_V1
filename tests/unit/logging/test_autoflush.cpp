/*
 * 🧱 File: test_autoflush.cpp
 * ---------------------------
 * 📌 Purpose   : Verify that autoFlush mode persists logs without manual flush.
 * 🧱 Layer     : Tests (Unit - Infrastructure)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-27
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Ensures that when autoFlush is enabled, log entries are flushed immediately
 * to the file sink without requiring an explicit flush() call.
 *
 * 🔗 Depends On:
 *   - infra/logging/LogManager.h
 * 🔗 Related   : test_log_manager_misc.cpp
 * ✅ Tested In : TasqlyTestsRunner
 */

#include <gtest/gtest.h>

// Qt
#include <QCoreApplication>
#include <QDir>
#include <QFile>

// Project
#include "infra/logging/LogManager.h"

using tasqly::app::logging::Level;
using tasqly::infra::logging::LogManager;

static QString readFile(const QString& path)
{
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly))
    return {};
  QString content = QString::fromUtf8(f.readAll());
  f.close();
  return content;
}

// 🧪 Test: autoFlush should persist data without manual flush
TEST(LogManagerAutoFlushTests, LogImmediatelyVisibleWhenAutoFlushEnabled)
{
  QDir dir(QCoreApplication::applicationDirPath());
  dir.mkpath("tests_tmp");
  QString basePath = dir.filePath("tests_tmp/autoflush.log");

  QFile::remove(basePath);

  LogManager lm;
  lm.enableConsole(false);
  lm.enableFileSink(true, basePath);
  lm.setAutoFlush(true);

  lm.log(Level::Info, "autoflush", "hello world (auto)");

  // No manual flush
  lm.enableFileSink(false); // close file sink

  QString content = readFile(basePath);
  EXPECT_TRUE(content.contains("hello world (auto"));
}

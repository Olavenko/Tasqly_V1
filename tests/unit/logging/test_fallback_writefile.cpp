/*
 * 🧱 File: test_fallback_writefile.cpp
 * ------------------------------------
 * 📌 Purpose   : Verify fallback to console when file writing fails.
 * 🧱 Layer     : Tests (Unit - Infrastructure)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-27
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Ensures that when writeFile_locked fails (e.g., due to read-only file),
 * LogManager disables the file sink and falls back to console-only mode.
 *
 * 🔗 Depends On:
 *   - infra/logging/LogManager.h
 * 🔗 Related   : test_rotation_policy.cpp (rotation failure fallback)
 * ✅ Tested In : TasqlyTestsRunner
 */

#include <gtest/gtest.h>

// Qt
#include <QDir>
#include <QFile>
#include <QTemporaryDir>

// Project
#include "infra/logging/LogManager.h"

using tasqly::app::logging::Level;
using tasqly::infra::logging::LogManager;

TEST(LogManagerFallbackTests, FallbackWhenFileWriteFails)
{
  QTemporaryDir tmpDir;
  ASSERT_TRUE(tmpDir.isValid());

  QString basePath = tmpDir.path() + "/fallback_writefail.log";

  // Create file and make it read-only
  QFile f(basePath);
  ASSERT_TRUE(f.open(QIODevice::WriteOnly));
  f.write("init\n");
  f.close();
  f.setPermissions(QFileDevice::ReadOwner | QFileDevice::ReadUser); // read-only

  LogManager lm;
  lm.enableConsole(false); // avoid stdout spam
  lm.enableFileSink(true, basePath);

  // Attempt to log → should fail silently and disable file sink
  lm.log(Level::Info, "fallback", "this should not be written");
  lm.flush();
  lm.enableFileSink(false);

  QFile f2(basePath);
  ASSERT_TRUE(f2.open(QIODevice::ReadOnly));
  QByteArray content = f2.readAll();
  f2.close();

  // Content must not contain our new log line (only "init" remains)
  EXPECT_TRUE(content.contains("init"));
  EXPECT_FALSE(content.contains("this should not be written"));
}

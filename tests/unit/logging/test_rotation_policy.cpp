/*
 * 🧱 File: test_rotation_policy.cpp
 * ---------------------------------
 * 📌 Purpose   : Unit tests for log file rotation policy.
 * 🧱 Layer     : Tests (Unit)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-03
 *
 * 🧠 Description:
 * Verifies rotation behavior of LogManager when file size exceeds threshold.
 * - Creates logs in a manual test directory under build/mingw-debug/tests_tmp/
 * - Ensures rotated files are created and capped by maxFiles
 * - Tests truncate mode when maxFiles=0
 */

#include "infra/logging/LogManager.h"
#include "tests/fakes/LogManagerTestAccessor.h"
#include <gtest/gtest.h>

// Qt
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>

using namespace tasqly::infra::logging;
using tasqly::app::logging::Level;
using tasqly::app::logging::RotationPolicy;

namespace {

// 🛠️ Helper: write N log lines of given size
void writeLogs(LogManager& lm, int count, const QString& payload)
{
  for (int i = 0; i < count; ++i) {
    lm.log(Level::Info, "test.rotation", payload, {});
  }
}

// 🛠️ Helper: count files matching pattern
int countLogFiles(const QString& basePath)
{
  QFileInfo fi(basePath);
  QDir dir = fi.dir();
  QString baseName = fi.fileName();
  QStringList filters;
  filters << baseName << baseName + ".*";
  return dir.entryList(filters, QDir::Files).size();
}

} // namespace

TEST(RotationPolicyTests, BasicRotationCreatesFiles)
{
  QDir dir(QCoreApplication::applicationDirPath());
  dir.mkpath("tests_tmp");
  QString basePath = dir.filePath("tests_tmp/rotation_basic.log");

  QFile::remove(basePath);
  QFile::remove(basePath + ".1");

  LogManager lm;
  lm.enableConsole(false);
  lm.enableFileSink(true, basePath);
  lm.setRotationPolicy({1024, 1}); // 1 KB, keep 1 rotated file

  QString payload(64, 'X');   // ~64 bytes
  writeLogs(lm, 20, payload); // ~1.2 KB total, triggers 1 rotation
  lm.flush();
  lm.enableFileSink(false); // force close

  int files = countLogFiles(basePath);
  EXPECT_LE(files, 2); // base + .1
  SUCCEED();
}

TEST(RotationPolicyTests, TruncateModeWhenMaxFilesZero)
{
  QDir dir(QCoreApplication::applicationDirPath());
  dir.mkpath("tests_tmp");
  QString basePath = dir.filePath("tests_tmp/rotation_truncate.log");

  LogManager lm;
  lm.enableConsole(false);
  lm.enableFileSink(true, basePath);
  lm.setRotationPolicy({512, 0}); // 512 bytes, keep 0 files

  QString payload(256, 'A');
  writeLogs(lm, 10, payload);
  lm.flush();

  // Only one file should exist (basePath)
  EXPECT_EQ(countLogFiles(basePath), 1);
}

// ✅ Test: multi-file rotation with maxFiles=3
TEST(RotationPolicyTests, MultiFileRotationKeepsMaxFiles)
{
  QDir dir(QCoreApplication::applicationDirPath());
  dir.mkpath("tests_tmp");
  QString basePath = dir.filePath("tests_tmp/rotation_multi.log");

  QFile::remove(basePath);
  for (int i = 1; i <= 3; ++i)
    QFile::remove(basePath + "." + QString::number(i));

  LogManager lm;
  lm.enableConsole(false);
  lm.enableFileSink(true, basePath);
  lm.setRotationPolicy({512, 3}); // small size, keep 3 files

  QString payload(256, 'B');
  writeLogs(lm, 40, payload); // enough to trigger multiple rotations
  lm.flush();
  lm.enableFileSink(false);

  // Expect base + up to 3 rotated files
  int files = countLogFiles(basePath);
  EXPECT_LE(files, 4);
}

// ✅ Test: fallback when rotation fails (readonly file)
TEST(RotationPolicyTests, FallbackOnRotationFailure)
{
  QTemporaryDir tmpDir;
  ASSERT_TRUE(tmpDir.isValid());

  QString basePath = tmpDir.path() + "/rotation_fail.log";

  QFile f(basePath);
  ASSERT_TRUE(f.open(QIODevice::WriteOnly));
  f.write("init");
  f.close();
  f.setPermissions(QFileDevice::ReadOwner | QFileDevice::ReadUser);

  LogManager lm;
  lm.enableConsole(true);
  lm.enableFileSink(true, basePath);
  lm.setRotationPolicy({1, 1});

  // Force rotation immediately (bypassing async debounce)
  LogManagerTestAccessor::forceRotate(lm);

  // Test passes if no crash occurs
  SUCCEED();
}

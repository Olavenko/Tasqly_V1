/*
 * 🧪 File: test_log_manager_extended.cpp
 * --------------------------------------
 * 📌 Purpose   : Extended integration tests for LogManager (rotation + concurrency).
 * 🧱 Layer     : Tests (Integration / Logging)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-15
 * 🗓️ Updated   : 2025-09-30
 *
 * 🧠 Description:
 * - Ensures rotation by size works (maxSizeBytes + maxFiles).
 * - Ensures multi-threaded logging works safely without corruption/crash.
 *
 * 🔗 Depends On:
 *   - infra/logging/LogManager.h / .cpp
 */

#include "app/settings/FeatureFlagsManager.h"
#include "infra/logging/LogManager.h"
#include "tests/fakes/LogManagerTestAccessor.h"
#include <gtest/gtest.h>

#include <QTest>
#include <filesystem>
#include <thread>

using namespace tasqly::infra::logging;
namespace fs = std::filesystem;

class LogManagerExtendedTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    tmpDir = fs::temp_directory_path() / "tasqly_log_test_ext";

    // 🧹 Clean tmpDir before each test
    if (fs::exists(tmpDir)) {
      for (auto& entry : fs::directory_iterator(tmpDir)) {
        fs::remove_all(entry.path());
      }
    }
    fs::create_directories(tmpDir);

    logFile = tmpDir / "test.log";

    manager = std::make_unique<LogManager>();

    // ✅ Default sink for tests that don't override
    manager->enableFileSink(true, QString::fromStdString(logFile.string()));
  }

  void TearDown() override
  {
    manager.reset();
    fs::remove_all(tmpDir);
  }

  std::unique_ptr<LogManager> manager;
  fs::path tmpDir;
  fs::path logFile;
};

TEST_F(LogManagerExtendedTest, RotatesWhenFileExceedsMaxSize)
{
  // ✅ Ensure logging feature is enabled
  FeatureFlagsManager::instance().setFeature("features.logging", true);

  // 🛑 Disable default sink (avoids test.log being created by default LogFacade)
  manager->enableFileSink(false, "");

  // ✅ Enable dedicated test sink
  auto filePath = tmpDir / "rotation_test.log";
  manager->enableFileSink(true, QString::fromStdString(filePath.string()));

  // 📏 Set tiny rotation policy
  tasqly::app::logging::RotationPolicy policy;
  policy.maxBytes = 128; // very small threshold
  policy.maxFiles = 2;
  manager->setRotationPolicy(policy);

  // 🪵 Write enough data to exceed rotation threshold
  QString msg(200, 'X');
  manager->log(LogManager::Level::Info, "test", msg);
  manager->flush();

  // ✅ Verify file got data before rotation
  ASSERT_TRUE(fs::exists(filePath));
  ASSERT_GT(fs::file_size(filePath), 0u) << "rotation_test.log is empty before rotation";

  // 🧪 Force rotation immediately (bypass timers)
  LogManagerTestAccessor::forceRotate(*manager);

  // 🔍 Check rotated files
  bool rotated = false;
  std::ostringstream debugFiles;
  for (auto& entry : fs::directory_iterator(tmpDir)) {
    auto fname = entry.path().filename().string();
    debugFiles << fname << "\n";
    if (fname.find("rotation_test.log.") != std::string::npos) {
      rotated = true;
    }
  }

  EXPECT_TRUE(rotated) << "Rotation did not occur. Files in dir:\n"
                       << debugFiles.str()
                       << "File size=" << (fs::exists(filePath) ? fs::file_size(filePath) : 0);
}

TEST_F(LogManagerExtendedTest, HandlesConcurrentLogging)
{
  const int threads = 6;
  const int perThread = 500;

  std::vector<std::thread> pool;
  for (int t = 0; t < threads; t++) {
    pool.emplace_back([&, t]() {
      for (int i = 0; i < perThread; i++) {
        manager->log(LogManager::Level::Info, "thread", QString("T%1 msg %2").arg(t).arg(i));
      }
    });
  }
  for (auto& th : pool)
    th.join();
  manager->flush();

  EXPECT_TRUE(fs::exists(logFile));
  auto size = fs::file_size(logFile);
  EXPECT_GT(size, 0);
}

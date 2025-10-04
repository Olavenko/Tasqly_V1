/*
 * 🧪 File: test_init_repositories.cpp
 * -----------------------------------
 * 📌 Purpose   : Integration tests for InitRepositories use case.
 * 🧱 Layer     : Tests (Integration / Use Cases)
 * 👤 Authors   : Mohamed Ali, Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-08
 * 🗓️ Updated   : 2025-09-24 (merged duplicate test files)
 *
 * 🧠 Description:
 * Ensures InitRepositories:
 *  - Initializes in-memory repositories (with/without seed).
 *  - Registers clock and uuidGen in AppContext.
 *  - Logs correct levels on success and failure.
 *  - Handles repository constructor failures gracefully.
 *
 * 🔗 Depends On:
 *   - src/app/usecases/InitRepositories.h / .cpp
 *   - src/app/core/AppContext.h
 *   - src/app/loading/AppLoadingManager.h
 *   - src/app/logging/ILogManager.h
 *   - infra/persistence/InMemoryGoalRepository.h
 *   - infra/persistence/InMemoryTaskRepository.h
 */

#include "app/core/AppContext.h"
#include "app/loading/AppLoadingManager.h"
#include "app/logging/ILogManager.h"
#include "app/usecases/InitRepositories.h"
#include "domain/core/Result.h"
#include <gtest/gtest.h>
#include <stdexcept>
#include <utility>
#include <vector>

using namespace tasqly::domain::core;
using namespace tasqly::app::usecases;
using namespace tasqly::app::logging;

// 👉 SpyLogManager: captures log calls for assertions
class SpyLogManager : public ILogManager
{
public:
  struct Call
  {
    Level lvl;
    QString cat;
    QString msg;
  };
  std::vector<Call> calls;
  Level lvl = Level::Info;

  bool isEnabled(Level level) const override
  {
    return static_cast<int>(level) >= static_cast<int>(lvl);
  }

  void log(Level level, const QString& cat, const QString& msg, const QVariantMap& = {}) override
  {
    calls.push_back({level, cat, msg});
  }

  // unused implementations
  void setLevel(Level) override {}
  Level level() const override { return lvl; }
  void enableConsole(bool) override {}
  void enableFileSink(bool, const QString& = QString()) override {}
  void setRotationPolicy(const RotationPolicy&) override {}
  void rotateNow() override {}
  void flush() override {}
  void addMaskedKey(const QString&) override {}
  void setMaskedKeys(const QStringList&) override {}
  QStringList maskedKeys() const override { return {}; }
};

// 👉 Log manager that throws on the first log invocation and records subsequent calls
class FirstCallThrowingLogManager : public ILogManager
{
public:
  struct Call
  {
    Level lvl;
    QString cat;
    QString msg;
  };

  mutable bool firstCall = true;
  std::vector<Call> calls;

  bool isEnabled(Level) const override { return true; }

  void log(Level level, const QString& cat, const QString& msg, const QVariantMap& = {}) override
  {
    if (std::exchange(firstCall, false)) {
      throw std::runtime_error("log sink unavailable");
    }
    calls.push_back({level, cat, msg});
  }

  void setLevel(Level) override {}
  Level level() const override { return Level::Info; }
  void enableConsole(bool) override {}
  void enableFileSink(bool, const QString& = QString()) override {}
  void setRotationPolicy(const RotationPolicy&) override {}
  void rotateNow() override {}
  void flush() override {}
  void addMaskedKey(const QString&) override {}
  void setMaskedKeys(const QStringList&) override {}
  QStringList maskedKeys() const override { return {}; }
};

//
// ✅ Test cases
//

// 🧪 Test 1: initializes repos successfully with seed=true
TEST(InitRepositoriesTests, InitializesRepositoriesSuccessfullyWithSeed)
{
  AppContext ctx;
  AppLoadingManager loading;
  SpyLogManager log;

  auto result = InitRepositories(ctx, loading, log, true);

  EXPECT_TRUE(result.isOk());
  ASSERT_NE(ctx.taskRepository(), nullptr);
  ASSERT_NE(ctx.goalRepository(), nullptr);
  EXPECT_TRUE(loading.repositoriesReady());

  ASSERT_FALSE(log.calls.empty());
  EXPECT_EQ(log.calls[0].lvl, Level::Info);
  EXPECT_TRUE(log.calls[0].msg.contains("Repositories initialized successfully"));

  auto goals = ctx.goalRepository()->list(10, 0).get();
  EXPECT_TRUE(goals.isOk());
  EXPECT_GE(goals.value().size(), 2);
}

// 🧪 Test 2: initializes repos successfully with seed=false
TEST(InitRepositoriesTests, InitializesRepositoriesSuccessfullyWithoutSeed)
{
  AppContext ctx;
  AppLoadingManager loading;
  SpyLogManager log;

  auto result = InitRepositories(ctx, loading, log, false);

  EXPECT_TRUE(result.isOk());
  auto goals = ctx.goalRepository()->list(10, 0).get();
  auto tasks = ctx.taskRepository()->list(10, 0).get();

  EXPECT_TRUE(goals.isOk());
  EXPECT_TRUE(tasks.isOk());
  EXPECT_TRUE(goals.value().empty());
  EXPECT_TRUE(tasks.value().empty());
  EXPECT_TRUE(loading.repositoriesReady());
}

// 🧪 Test 3: registers clock and uuidGen in context
TEST(InitRepositoriesTests, RegistersClockAndUuidGen)
{
  AppContext ctx;
  AppLoadingManager loading;
  SpyLogManager log;

  auto result = InitRepositories(ctx, loading, log, false);

  EXPECT_TRUE(result.isOk());
  EXPECT_NE(ctx.clock(), nullptr);
  EXPECT_NE(ctx.uuidGen(), nullptr);
}

// 🧪 Test 4: logger failure during success log triggers error handling branch
TEST(InitRepositoriesTests, LoggerFailureTriggersErrorBranch)
{
  AppContext ctx;
  AppLoadingManager loading;
  FirstCallThrowingLogManager log;

  auto result = InitRepositories(ctx, loading, log, false);

  EXPECT_TRUE(result.isErr());
  EXPECT_FALSE(loading.repositoriesReady());

  ASSERT_EQ(log.calls.size(), 1u);
  EXPECT_EQ(log.calls[0].lvl, Level::Error);
  EXPECT_TRUE(log.calls[0].msg.contains("Failed to initialize repositories"));
}

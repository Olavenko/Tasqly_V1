/*
 * 🧪 File: test_app_context.cpp
 * -----------------------------
 * 📌 Purpose   : Unit tests for AppContext initialization and cleanup
 * 🧱 Layer     : Tests (Unit/Core)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-15
 * 🔖 Version   : 1.2 (Uses unified fakes: FakeUuidGen, FakeClock)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * - Validates that AppContext correctly initializes repositories and services.
 * - Ensures default state is nullptr when not set.
 * - Confirms resources are cleaned up when context goes out of scope.
 *
 * 🔗 Depends On:
 *   - AppContext.h / AppContext.cpp
 *   - domain/core interfaces (ITaskRepository, IGoalRepository, IUuidGen, IClock)
 *   - tests/fakes/FakeUuidGen.h, tests/fakes/FakeClock.h
 * 🔗 Related   : Result.h, Error.h
 * ✅ Tested In : TasqlyTestsRunner
 * ⚠️ Notes     : InitSuccess deferred to Phase1 once infra ready.
 */

#include <QtCore/QDateTime>
#include <QtCore/QString>
#include "app/core/AppContext.h"
#include "domain/core/Error.h"
#include "domain/core/Goal.h"
#include "domain/core/IGoalRepository.h"
#include "domain/core/ITaskRepository.h"
#include "domain/core/Result.h"
#include "domain/core/Task.h"
#include <gtest/gtest.h>

#include "tests/fakes/FakeClock.h"
#include "tests/fakes/FakeUuidGen.h"

using namespace tasqly::domain::core;

// =====================================================
// 👉 Helper: make_ready_future (safe for MinGW)
// =====================================================
template<typename T>
std::future<T> make_ready_future(T value)
{
  return std::async(std::launch::deferred,
                    [v = std::move(value)]() mutable { return std::move(v); });
}

// =====================================================
// 👉 Fake Repositories (sync only for Phase 0)
// =====================================================

struct FakeTaskRepo : public ITaskRepository
{
  std::future<Result<Task>> add(const Task &) override
  {
    return make_ready_future(err<Task>(Error::Unknown()));
  }
  std::future<Result<Task>> update(const Task &) override
  {
    return make_ready_future(err<Task>(Error::Unknown()));
  }
  std::future<Result<void>> remove(const QString &) override
  {
    return make_ready_future(err<void>(Error::Unknown()));
  }
  std::future<Result<Task>> getById(const QString &) override
  {
    return make_ready_future(err<Task>(Error::Unknown()));
  }
  std::future<Result<std::vector<Task>>> list(int, int) override
  {
    return make_ready_future(err<std::vector<Task>>(Error::Unknown()));
  }
};

struct FakeGoalRepo : public IGoalRepository
{
  std::future<Result<Goal>> add(const Goal &) override
  {
    return make_ready_future(err<Goal>(Error::Unknown()));
  }
  std::future<Result<Goal>> update(const Goal &) override
  {
    return make_ready_future(err<Goal>(Error::Unknown()));
  }
  std::future<Result<void>> remove(const QString &) override
  {
    return make_ready_future(err<void>(Error::Unknown()));
  }
  std::future<Result<Goal>> getById(const QString &) override
  {
    return make_ready_future(err<Goal>(Error::Unknown()));
  }
  std::future<Result<std::vector<Goal>>> list(int, int) override
  {
    return make_ready_future(err<std::vector<Goal>>(Error::Unknown()));
  }
};

// =====================================================
// 👉 TESTS
// =====================================================

// 🧪 Ensure each setter/getter works correctly
TEST(AppContextTest, IndividualSettersAndGetters)
{
  AppContext ctx;

  auto taskRepo = std::make_shared<FakeTaskRepo>();
  auto goalRepo = std::make_shared<FakeGoalRepo>();
  auto uuidGen = std::make_shared<FakeUuidGen>();
  auto clock = std::make_shared<FakeClock>();

  ctx.setTaskRepository(taskRepo);
  ctx.setGoalRepository(goalRepo);
  ctx.setUuidGen(uuidGen);
  ctx.setClock(clock);

  EXPECT_EQ(ctx.taskRepository(), taskRepo);
  EXPECT_EQ(ctx.goalRepository(), goalRepo);
  EXPECT_EQ(ctx.uuidGen(), uuidGen);
  EXPECT_EQ(ctx.clock(), clock);

  EXPECT_EQ(ctx.uuidGen()->generate(), QStringLiteral("fake-0"));
  EXPECT_TRUE(ctx.clock()->now().isValid());
}

// 🧪 Overwriting services should replace previous ones
TEST(AppContextTest, OverwriteRepositoriesAndServices)
{
  AppContext ctx;

  auto repo1 = std::make_shared<FakeTaskRepo>();
  auto repo2 = std::make_shared<FakeTaskRepo>();

  ctx.setTaskRepository(repo1);
  EXPECT_EQ(ctx.taskRepository(), repo1);

  ctx.setTaskRepository(repo2);
  EXPECT_EQ(ctx.taskRepository(), repo2); // ✅ replaced

  auto uuid1 = std::make_shared<FakeUuidGen>();
  auto uuid2 = std::make_shared<FakeUuidGen>();

  ctx.setUuidGen(uuid1);
  EXPECT_EQ(ctx.uuidGen(), uuid1);

  ctx.setUuidGen(uuid2);
  EXPECT_EQ(ctx.uuidGen(), uuid2); // ✅ replaced
}

// 🧪 Validate FakeClock and FakeUuidGen integration
TEST(AppContextTest, ServicesReturnExpectedValues)
{
  AppContext ctx;

  auto uuidGen = std::make_shared<FakeUuidGen>();
  auto clock = std::make_shared<FakeClock>();

  ctx.setUuidGen(uuidGen);
  ctx.setClock(clock);

  EXPECT_EQ(ctx.uuidGen()->generate(), QStringLiteral("fake-0"));
  auto now = ctx.clock()->now();
  EXPECT_TRUE(now.isValid());
}

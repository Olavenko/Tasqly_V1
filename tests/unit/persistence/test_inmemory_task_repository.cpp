/*
 * 🧱 File: test_inmemory_task_repository.cpp
 * -----------------------------------------
 * 📌 Purpose   : Unit tests for InMemoryTaskRepository CRUD semantics.
 * 🧱 Layer     : Tests (Unit - Persistence)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-08
 * 🗓️ Updated   : 2025-09-23
 *
 * 🧠 Description:
 * Validates correctness of InMemoryTaskRepository core behavior:
 * - CRUD operations
 * - pagination
 * - empty state
 * - thread safety under concurrent access
 *
 * 📌 Note:
 * Tests focus purely on repository core logic. No UI-related behavior is checked here.
 */

#include "infra/persistence/InMemoryTaskRepository.h"
#include <gtest/gtest.h>
#include <set>
#include <thread>

using namespace tasqly::domain::core;

// 🛠️ Helper to build a test task
static Task makeTask(const QString& id, const QString& title, QDateTime now)
{
  Task t;
  t.id = id;
  t.title = title;
  t.description = "desc";
  t.createdAt = now;
  t.updatedAt = now;
  return t;
}

// 📝 Test 1: Add then getById returns same task
TEST(InMemoryTaskRepositoryTests, AddAndGetById)
{
  InMemoryTaskRepository repo;
  QDateTime now = QDateTime::fromSecsSinceEpoch(1000);
  Task t = makeTask("t1", "Test Task", now);

  auto addResult = repo.add(t).get();
  ASSERT_TRUE(addResult.isOk());

  auto getResult = repo.getById("t1").get();
  ASSERT_TRUE(getResult.isOk());
  EXPECT_EQ(getResult.value().title, "Test Task");
}

// 📝 Test 2: Update existing task
TEST(InMemoryTaskRepositoryTests, UpdateExistingTask)
{
  InMemoryTaskRepository repo;
  QDateTime now = QDateTime::fromSecsSinceEpoch(1000);
  Task t = makeTask("t2", "Old Title", now);
  repo.add(t).get();

  t.title = "New Title";
  auto updateResult = repo.update(t).get();
  ASSERT_TRUE(updateResult.isOk());
  EXPECT_EQ(updateResult.value().title, "New Title");
}

// 📝 Test 3: Remove task and verify not found
TEST(InMemoryTaskRepositoryTests, RemoveTask)
{
  InMemoryTaskRepository repo;
  QDateTime now = QDateTime::fromSecsSinceEpoch(1000);
  Task t = makeTask("t3", "Remove Me", now);
  repo.add(t).get();

  auto removeResult = repo.remove("t3").get();
  ASSERT_TRUE(removeResult.isOk());

  auto getResult = repo.getById("t3").get();
  ASSERT_TRUE(getResult.isErr());
  EXPECT_EQ(getResult.error().code(), static_cast<int>(RepoErrorCode::NotFound));
}

// 📝 Test 4: List with limit + offset works
TEST(InMemoryTaskRepositoryTests, ListTasksWithLimitOffset)
{
  InMemoryTaskRepository repo;
  QDateTime now = QDateTime::fromSecsSinceEpoch(1000);

  std::set<QString> expectedIds;
  for (int i = 0; i < 5; ++i) {
    QString id = QString("id-%1").arg(i);
    repo.add(makeTask(id, QString("Task%1").arg(i), now)).get();
    expectedIds.insert(id);
  }

  auto listResult = repo.list(2, 1).get();
  ASSERT_TRUE(listResult.isOk());
  auto tasks = listResult.value();

  EXPECT_EQ(tasks.size(), 2);
  for (auto& t : tasks) {
    EXPECT_TRUE(expectedIds.find(t.id) != expectedIds.end());
  }
}

// 📝 Test 5: Empty repo returns empty list
TEST(InMemoryTaskRepositoryTests, EmptyRepoListsEmpty)
{
  InMemoryTaskRepository repo;
  auto listResult = repo.list(10, 0).get();
  ASSERT_TRUE(listResult.isOk());
  EXPECT_TRUE(listResult.value().empty());
}

// 📝 Test 6: Thread safety under concurrent adds
TEST(InMemoryTaskRepositoryTests, ThreadSafetyConcurrentAdds)
{
  InMemoryTaskRepository repo;
  QDateTime now = QDateTime::fromSecsSinceEpoch(1000);

  constexpr int N = 100;
  std::vector<std::thread> threads;
  threads.reserve(N);
  for (int i = 0; i < N; ++i) {
    threads.emplace_back([&, i]() {
      Task t = makeTask(QString("concurrent-%1").arg(i), "T", now);
      repo.add(t).get();
    });
  }
  for (auto& th : threads)
    th.join();

  auto listResult = repo.list(N, 0).get();
  ASSERT_TRUE(listResult.isOk());
  EXPECT_EQ(listResult.value().size(), N);
}

// 📝 Test 7: Add duplicate task fails
TEST(InMemoryTaskRepositoryTests, AddDuplicateTaskFails)
{
  InMemoryTaskRepository repo;
  QDateTime now = QDateTime::fromSecsSinceEpoch(1000);
  Task t = makeTask("dup", "First", now);
  repo.add(t).get();

  auto res2 = repo.add(t).get();
  EXPECT_TRUE(res2.isErr());
  EXPECT_EQ(res2.error().code(), static_cast<int>(RepoErrorCode::AlreadyExists));
}

// 📝 Test 8: Update non-existent task fails
TEST(InMemoryTaskRepositoryTests, UpdateNonExistentTaskFails)
{
  InMemoryTaskRepository repo;
  QDateTime now = QDateTime::fromSecsSinceEpoch(1000);
  Task t = makeTask("missing", "Does not exist", now);

  auto res = repo.update(t).get();
  EXPECT_TRUE(res.isErr());
  EXPECT_EQ(res.error().code(), static_cast<int>(RepoErrorCode::NotFound));
}

// 📝 Test 9: GetById non-existent task fails
TEST(InMemoryTaskRepositoryTests, GetByIdNonExistentTaskFails)
{
  InMemoryTaskRepository repo;
  auto res = repo.getById("ghost").get();
  EXPECT_TRUE(res.isErr());
  EXPECT_EQ(res.error().code(), static_cast<int>(RepoErrorCode::NotFound));
}

// 📝 Test 10: Bulk insert adds multiple tasks and skips duplicates
TEST(InMemoryTaskRepositoryTests, BulkInsertAddsMultipleTasks)
{
  InMemoryTaskRepository repo;
  QDateTime now = QDateTime::currentDateTimeUtc();

  std::vector<Task> tasks = {
      makeTask("b1", "Bulk1", now),
      makeTask("b2", "Bulk2", now),
      makeTask("b1", "Duplicate", now) // duplicate id
  };

  repo.addBulk(std::move(tasks)); // void

  auto listRes = repo.list(10, 0).get();
  ASSERT_TRUE(listRes.isOk());
  EXPECT_EQ(listRes.value().size(), 2);
}

// 📝 Test 11: Seed constructor adds default tasks
TEST(InMemoryTaskRepositoryTests, SeedConstructorAddsDefaultTasks)
{
  InMemoryTaskRepository repo(true);
  auto listRes = repo.list(10, 0).get();
  ASSERT_TRUE(listRes.isOk());
  EXPECT_GE(listRes.value().size(), 2);
}

// 📝 Test 12: Move overloads for add and update work
TEST(InMemoryTaskRepositoryTests, MoveOverloadAddAndUpdateWorks)
{
  InMemoryTaskRepository repo;
  QDateTime now = QDateTime::currentDateTimeUtc();

  // Add via move
  Task t1 = makeTask("m1", "MoveAdd", now);
  auto res1 = repo.add(Task(t1)).get();
  EXPECT_TRUE(res1.isOk());

  // Update via move
  Task t2 = makeTask("m1", "MoveUpdate", now);
  auto res2 = repo.update(Task(t2)).get();
  EXPECT_TRUE(res2.isOk());
  EXPECT_EQ(res2.value().title, "MoveUpdate");
}

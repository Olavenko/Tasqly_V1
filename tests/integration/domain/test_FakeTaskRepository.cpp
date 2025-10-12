/*
 * 🧪 File: test_FakeTaskRepository.cpp
 * -----------------------------------
 * 📌 Purpose   : Integration tests for FakeTaskRepository.
 * 🧱 Layer     : Tests (Integration / Domain)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-12
 * 🔖 Version   : 1.0
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Verifies the correctness of in-memory CRUD and filtering
 * logic inside FakeTaskRepository. Ensures that DomainResult
 * behaves as expected for both success and failure scenarios.
 */

#include "domain/core/entities/Task.h"
#include "domain/core/entities/TaskPriority.h"
#include "domain/core/entities/TaskStatus.h"
#include "tests/fakes/domain/FakeTaskRepository.h"
#include <gtest/gtest.h>

using namespace tasqly::domain::core;

// 🧪 Fixture class for shared setup
class FakeTaskRepositoryTest : public ::testing::Test
{
protected:
  FakeTaskRepository repo;

  Task makeTask(const std::string& title,
                TaskStatus status = TaskStatus::Todo,
                TaskPriority priority = TaskPriority::Normal)
  {
    Task t;
    t.title = title;
    t.status = status;
    t.priority = priority;
    return t;
  }
};

// 🧪 Create Task
TEST_F(FakeTaskRepositoryTest, Create_ShouldStoreNewTask)
{
  auto task = makeTask("New Task 1");
  auto result = repo.create(task);

  ASSERT_TRUE(result.isOk());
  EXPECT_EQ(result.value().title, "New Task 1");
}

// 🧪 Create duplicate ID → Conflict
TEST_F(FakeTaskRepositoryTest, Create_WithDuplicateId_ShouldFail)
{
  auto task = makeTask("Task A");
  repo.create(task);

  auto result = repo.create(task);
  ASSERT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::Conflict);
}

// 🧪 GetById existing task
TEST_F(FakeTaskRepositoryTest, GetById_ShouldReturnCorrectTask)
{
  auto task = makeTask("Find Me");
  repo.create(task);

  auto found = repo.getById(task.id);
  ASSERT_TRUE(found.isOk());
  EXPECT_EQ(found.value().title, "Find Me");
}

// 🧪 GetById non-existent → NotFound
TEST_F(FakeTaskRepositoryTest, GetById_NonExistent_ShouldFail)
{
  auto result = repo.getById("unknown-id");
  ASSERT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::NotFound);
}

// 🧪 Update existing task
TEST_F(FakeTaskRepositoryTest, Update_ShouldModifyExistingTask)
{
  auto task = makeTask("Old Title");
  repo.create(task);

  task.title = "Updated Title";
  auto updated = repo.update(task);

  ASSERT_TRUE(updated.isOk());
  EXPECT_EQ(updated.value().title, "Updated Title");
}

// 🧪 Update non-existent → NotFound
TEST_F(FakeTaskRepositoryTest, Update_NonExistent_ShouldFail)
{
  auto task = makeTask("Ghost");
  auto result = repo.update(task);

  ASSERT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::NotFound);
}

// 🧪 Remove existing
TEST_F(FakeTaskRepositoryTest, Remove_ShouldDeleteTask)
{
  auto task = makeTask("Delete Me");
  repo.create(task);

  auto removed = repo.remove(task.id);
  ASSERT_TRUE(removed.isOk());

  auto after = repo.getById(task.id);
  ASSERT_TRUE(after.isErr());
  EXPECT_EQ(after.error().code, DomainErrorCode::NotFound);
}

// 🧪 Remove non-existent
TEST_F(FakeTaskRepositoryTest, Remove_NonExistent_ShouldFail)
{
  auto result = repo.remove("missing-id");
  ASSERT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::NotFound);
}

// 🧪 List filtering and pagination
TEST_F(FakeTaskRepositoryTest, List_ShouldReturnFilteredResults)
{
  repo.create(makeTask("Task 1", TaskStatus::Todo, TaskPriority::Low));
  repo.create(makeTask("Task 2", TaskStatus::Doing, TaskPriority::High));
  repo.create(makeTask("Task 3", TaskStatus::Done, TaskPriority::Normal));

  auto result = repo.list(TaskStatus::Doing, std::nullopt, 10, 0);
  ASSERT_TRUE(result.isOk());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value()[0].title, "Task 2");
}

/*
 * 🧱 File: test_P1_TaskRepositoryIntegration.cpp
 * ----------------------------------------------
 * 📌 Purpose   : Integration test for P1 Domain Layer (TaskRepository + Mapper)
 * 🧱 Layer     : Domain (Integration)
 * 🧪 Type      : Integration Test (GoogleTest)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-14
 * 🔖 Version   : 1.0 (Initial Integration)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 *   Validates end-to-end flow for Task lifecycle:
 *     - Create → Read → Update → List → Delete
 *   Uses the in-memory FakeTaskRepository implementation.
 */

#include "domain/core/entities/P1_Task.h"
#include "domain/core/errors/P1_DomainResult.h"
#include "tests/fakes/domain/FakeTaskRepository.h"
#include <gtest/gtest.h>

using namespace tasqly::p1::s1::domain::core;

// ----------------------------------------------------------------------------
// 🧩 Test Fixture
// ----------------------------------------------------------------------------
class TaskRepositoryIntegrationTest : public ::testing::Test
{
protected:
  FakeTaskRepository repo;

  void SetUp() override { repo.clear(); }
  void TearDown() override { repo.clear(); }
};

// ----------------------------------------------------------------------------
// 🧪 Test 1: Create and Retrieve a Task
// ----------------------------------------------------------------------------
TEST_F(TaskRepositoryIntegrationTest, CreateAndGetById)
{
  Task task;
  task.id = "int-001";
  task.title = "Integration Create/Get Test";
  task.notes = "Verify end-to-end repository behavior";
  task.status = TaskStatus::Todo;
  task.priority = TaskPriority::Normal;

  auto result = repo.create(task);
  ASSERT_TRUE(result.isOk()) << "Expected creation success";

  auto fetched = repo.getById("int-001");
  ASSERT_TRUE(fetched.isOk()) << "Expected retrieval success";

  EXPECT_EQ(fetched.value().title, task.title);
  EXPECT_EQ(fetched.value().status, TaskStatus::Todo);
  EXPECT_EQ(fetched.value().priority, TaskPriority::Normal);
  EXPECT_TRUE(fetched.value().createdAt <= fetched.value().updatedAt);
}

// ----------------------------------------------------------------------------
// 🧪 Test 2: Duplicate ID should return Conflict error
// ----------------------------------------------------------------------------
TEST_F(TaskRepositoryIntegrationTest, CreateDuplicateIdFails)
{
  Task t1;
  t1.id = "dup-001";
  t1.title = "Original";

  Task t2 = t1;
  t2.title = "Duplicate";

  EXPECT_TRUE(repo.create(t1).isOk());
  auto result = repo.create(t2);

  ASSERT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::Conflict);
  EXPECT_EQ(result.error().message, "Task with same ID already exists");
}

// ----------------------------------------------------------------------------
// 🧪 Test 3: Update an existing task
// ----------------------------------------------------------------------------
TEST_F(TaskRepositoryIntegrationTest, UpdateExistingTask)
{
  Task t;
  t.id = "upd-001";
  t.title = "Before Update";
  t.status = TaskStatus::Todo;
  ASSERT_TRUE(repo.create(t).isOk());

  t.title = "After Update";
  t.status = TaskStatus::Done;
  auto updated = repo.update(t);

  ASSERT_TRUE(updated.isOk());
  EXPECT_EQ(updated.value().title, "After Update");
  EXPECT_EQ(updated.value().status, TaskStatus::Done);
  EXPECT_TRUE(updated.value().updatedAt > updated.value().createdAt);
}

// ----------------------------------------------------------------------------
// 🧪 Test 4: Remove task by ID
// ----------------------------------------------------------------------------
TEST_F(TaskRepositoryIntegrationTest, RemoveTaskById)
{
  Task t;
  t.id = "del-001";
  t.title = "Delete Me";
  ASSERT_TRUE(repo.create(t).isOk());

  auto removed = repo.remove("del-001");
  EXPECT_TRUE(removed.isOk());

  auto fetchAfterDelete = repo.getById("del-001");
  EXPECT_TRUE(fetchAfterDelete.isErr());
  EXPECT_EQ(fetchAfterDelete.error().code, DomainErrorCode::NotFound);
}

// ----------------------------------------------------------------------------
// 🧪 Test 5: List tasks with filters and pagination
// ----------------------------------------------------------------------------
TEST_F(TaskRepositoryIntegrationTest, ListWithFiltersAndPagination)
{
  // Create 3 tasks with different priorities and statuses
  for (int i = 0; i < 3; ++i) {
    Task t;
    t.id = "task-" + std::to_string(i);
    t.title = "Task " + std::to_string(i);
    t.status = (i % 2 == 0) ? TaskStatus::Todo : TaskStatus::Doing;
    t.priority = (i == 2) ? TaskPriority::High : TaskPriority::Normal;
    ASSERT_TRUE(repo.create(t).isOk());
  }

  // List all tasks (limit 10)
  auto listAll = repo.list(std::nullopt, std::nullopt, 10, 0);
  ASSERT_TRUE(listAll.isOk());
  EXPECT_EQ(listAll.value().size(), 3);

  // Filter: status == Todo
  auto listTodo = repo.list(TaskStatus::Todo, std::nullopt, 10, 0);
  ASSERT_TRUE(listTodo.isOk());
  EXPECT_EQ(listTodo.value().size(), 2);

  // Pagination: limit 1, offset 1
  auto paged = repo.list(std::nullopt, std::nullopt, 1, 1);
  ASSERT_TRUE(paged.isOk());
  EXPECT_EQ(paged.value().size(), 1);
}

// ----------------------------------------------------------------------------
// 🧪 Test 6: Attempt to update non-existent task returns NotFound
// ----------------------------------------------------------------------------
TEST_F(TaskRepositoryIntegrationTest, UpdateNonExistentReturnsError)
{
  Task t;
  t.id = "no-such";
  t.title = "Ghost Task";
  auto res = repo.update(t);
  ASSERT_TRUE(res.isErr());
  EXPECT_EQ(res.error().code, DomainErrorCode::NotFound);
  EXPECT_EQ(res.error().message, "Cannot update non-existent task");
}

// ----------------------------------------------------------------------------
// 🧪 Test 7: Remove non-existent task returns NotFound
// ----------------------------------------------------------------------------
TEST_F(TaskRepositoryIntegrationTest, RemoveNonExistentReturnsError)
{
  auto res = repo.remove("missing-123");
  ASSERT_TRUE(res.isErr());
  EXPECT_EQ(res.error().code, DomainErrorCode::NotFound);
}

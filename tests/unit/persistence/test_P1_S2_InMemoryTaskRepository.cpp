/*
 * 🧪 File: test_P1_S2_InMemoryTaskRepository.cpp
 * ----------------------------------------------
 * 📌 Purpose   : Unit Tests for P1_S2_InMemoryTaskRepository (Phase 1 / Slice 2)
 * 🧱 Layer     : Infrastructure (Persistence)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-26
 * 🔖 Version   : 1.0 (Fixture Setup)
 *
 * 🧠 Description:
 * This file defines the base fixture for testing the in-memory repository implementation.
 * It prepares a clean, deterministic environment for each test case.
 *
 * The fixture:
 *   - Initializes a thread-safe in-memory repository instance.
 *   - Silences unnecessary logs for cleaner test output.
 *   - Ensures state reset before each test.
 *
 * 🧩 Next Steps:
 *   1️⃣ Add CRUD test cases (Create, Read, Update, Delete)
 *   2️⃣ Add filtering & pagination tests
 *   3️⃣ Add edge case tests (conflicts, not found)
 */

#include "domain/core/entities/P1_Task.h"
#include "infra/persistence/P1_S2_InMemoryTaskRepository.h"
#include "infra/runtime/P1_Logger.h"
#include <gtest/gtest.h>

using namespace tasqly::p1::infra::persistence;
using namespace tasqly::p1::infra::runtime;
using namespace tasqly::p1::s1::domain::core;

// ================================================================
// 🧩 Test Fixture
// ================================================================
class InMemoryTaskRepoTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // 🧱 Silence logger for unit tests
    P1_Logger::instance().setMinimumLevel(LogLevel::Error);

    // 🧹 Ensure repo starts empty before each test
    repo = std::make_unique<P1_S2_InMemoryTaskRepository>();
  }

  void TearDown() override
  {
    // 🧹 Reset repository explicitly after test
    repo.reset();
  }

  // 🧩 Shared test resource
  std::unique_ptr<P1_S2_InMemoryTaskRepository> repo;
};

// ================================================================
// 🧩 Basic Sanity Test — Fixture Setup
// ================================================================
TEST_F(InMemoryTaskRepoTest, FixtureInitializesCleanly)
{
  ASSERT_NE(repo, nullptr) << "Repository fixture should be initialized.";
}

// ================================================================
// 🧪 Test Case #1 — CreateTask_SuccessfulInsert
// ================================================================
TEST_F(InMemoryTaskRepoTest, CreateTask_SuccessfulInsert)
{
  // 🧱 Arrange
  auto now = std::chrono::system_clock::now();
  Task newTask{"T-001",
               "Implement Postgres Repository",
               "Ensure safe parameterized queries",
               TaskStatus::Todo,
               TaskPriority::High,
               std::nullopt,
               now,
               now};

  // 🧩 Act
  auto result = repo->create(newTask);

  // 🧾 Assert
  ASSERT_TRUE(result.isOk()) << "Expected successful creation of task.";
  const auto& createdTask = result.value();

  EXPECT_EQ(createdTask.id, newTask.id);
  EXPECT_EQ(createdTask.title, newTask.title);
  EXPECT_EQ(createdTask.status, TaskStatus::Todo);
  EXPECT_EQ(createdTask.priority, TaskPriority::High);

  // 🧩 Verify repository actually stores the task
  auto lookup = repo->getById("T-001");
  ASSERT_TRUE(lookup.isOk()) << "Task should be retrievable after creation.";

  const auto& fetchedTask = lookup.value();
  EXPECT_EQ(fetchedTask.id, "T-001");
  EXPECT_EQ(fetchedTask.title, "Implement Postgres Repository");
}

// ================================================================
// 🧪 Test Case #2 — GetTaskById_ReturnsInserted
// ================================================================
TEST_F(InMemoryTaskRepoTest, GetTaskById_ReturnsInserted)
{
  // 🧱 Arrange
  auto now = std::chrono::system_clock::now();
  Task task{"T-002",
            "Implement InMemory Repository",
            "Used as fallback when Postgres fails",
            TaskStatus::Doing,
            TaskPriority::Normal,
            std::nullopt,
            now,
            now};

  // 🧩 Act — create the task
  auto createResult = repo->create(task);
  ASSERT_TRUE(createResult.isOk()) << "Task creation should succeed before retrieval.";

  // 🧩 Act — retrieve it by ID
  auto lookupResult = repo->getById("T-002");

  // 🧾 Assert
  ASSERT_TRUE(lookupResult.isOk()) << "Expected retrieval to succeed for existing ID.";

  const auto& fetched = lookupResult.value();
  EXPECT_EQ(fetched.id, task.id);
  EXPECT_EQ(fetched.title, task.title);
  EXPECT_EQ(fetched.status, TaskStatus::Doing);
  EXPECT_EQ(fetched.priority, TaskPriority::Normal);

  // 🧩 Negative Case — try non-existent ID
  auto missing = repo->getById("UNKNOWN-ID");
  ASSERT_TRUE(missing.isErr()) << "Expected error for unknown ID.";
  EXPECT_EQ(missing.error().code, DomainErrorCode::NotFound);
}

// ================================================================
// 🧪 Test Case #3 — UpdateTask_ModifiesValues
// ================================================================
TEST_F(InMemoryTaskRepoTest, UpdateTask_ModifiesValues)
{
  // 🧱 Arrange
  auto now = std::chrono::system_clock::now();
  Task task{"T-003",
            "Initial Title",
            "Initial Notes",
            TaskStatus::Todo,
            TaskPriority::Low,
            std::nullopt,
            now,
            now};

  // 🧩 Step 1 — Create the task
  auto createResult = repo->create(task);
  ASSERT_TRUE(createResult.isOk()) << "Task creation must succeed before update.";

  // 🧩 Step 2 — Modify fields
  task.title = "Updated Title";
  task.notes = "Updated Notes";
  task.status = TaskStatus::Done;
  task.priority = TaskPriority::High;

  // 🧩 Act — Update existing task
  auto updateResult = repo->update(task);
  ASSERT_TRUE(updateResult.isOk()) << "Expected successful update for existing task.";

  // 🧾 Assert — Re-fetch and verify changes
  auto lookup = repo->getById("T-003");
  ASSERT_TRUE(lookup.isOk()) << "Updated task should still exist after update.";

  const auto& updated = lookup.value();
  EXPECT_EQ(updated.title, "Updated Title");
  EXPECT_EQ(updated.notes, "Updated Notes");
  EXPECT_EQ(updated.status, TaskStatus::Done);
  EXPECT_EQ(updated.priority, TaskPriority::High);

  // 🧩 Step 3 — Negative Case: update non-existent task
  Task nonExistent{"T-999",
                   "Ghost Task",
                   "Should not exist",
                   TaskStatus::Todo,
                   TaskPriority::Low,
                   std::nullopt,
                   now,
                   now};

  auto failUpdate = repo->update(nonExistent);
  ASSERT_TRUE(failUpdate.isErr()) << "Expected update to fail for non-existent task.";
  EXPECT_EQ(failUpdate.error().code, DomainErrorCode::NotFound);
}

// ================================================================
// 🧪 Test Case #4 — RemoveTask_DeletesSuccessfully
// ================================================================
TEST_F(InMemoryTaskRepoTest, RemoveTask_DeletesSuccessfully)
{
  // 🧱 Arrange
  auto now = std::chrono::system_clock::now();
  Task task{"T-004",
            "Task to Delete",
            "This task will be removed",
            TaskStatus::Todo,
            TaskPriority::Low,
            std::nullopt,
            now,
            now};

  // 🧩 Step 1 — Create the task first
  auto createResult = repo->create(task);
  ASSERT_TRUE(createResult.isOk()) << "Task creation must succeed before deletion.";

  // 🧩 Step 2 — Verify exists before delete
  auto lookupBefore = repo->getById("T-004");
  ASSERT_TRUE(lookupBefore.isOk()) << "Task must exist before deletion.";

  // 🧩 Step 3 — Act: delete the task
  auto removeResult = repo->remove("T-004");
  ASSERT_TRUE(removeResult.isOk()) << "Expected successful deletion.";

  // 🧾 Assert — Verify it no longer exists
  auto lookupAfter = repo->getById("T-004");
  ASSERT_TRUE(lookupAfter.isErr()) << "Task should not exist after deletion.";
  EXPECT_EQ(lookupAfter.error().code, DomainErrorCode::NotFound);

  // 🧩 Step 4 — Negative case: deleting non-existent ID again
  auto failRemove = repo->remove("T-004");
  ASSERT_TRUE(failRemove.isErr()) << "Expected error when deleting non-existent ID.";
  EXPECT_EQ(failRemove.error().code, DomainErrorCode::NotFound);
}

// ================================================================
// 🧪 Test Case #5 — ListTasks_ReturnsFiltered (correct signature)
// ================================================================
TEST_F(InMemoryTaskRepoTest, ListTasks_ReturnsFiltered)
{
  // 🧱 Arrange
  auto now = std::chrono::system_clock::now();

  Task t1{"T-005",
          "Design Database Schema",
          "ERD and migrations",
          TaskStatus::Todo,
          TaskPriority::High,
          std::nullopt,
          now,
          now};

  Task t2{"T-006",
          "Implement Repository",
          "Postgres + InMemory",
          TaskStatus::Doing,
          TaskPriority::Normal,
          std::nullopt,
          now,
          now};

  Task t3{"T-007",
          "Write Unit Tests",
          "GTest CRUD coverage",
          TaskStatus::Todo,
          TaskPriority::Low,
          std::nullopt,
          now,
          now};

  ASSERT_TRUE(repo->create(t1).isOk());
  ASSERT_TRUE(repo->create(t2).isOk());
  ASSERT_TRUE(repo->create(t3).isOk());

  // 🧩 Step 1 — Retrieve all tasks (no filters)
  auto allResult = repo->list(std::nullopt, std::nullopt, 100, 0);
  ASSERT_TRUE(allResult.isOk()) << "Listing all tasks should succeed.";

  const auto& allTasks = allResult.value();
  EXPECT_EQ(allTasks.size(), 3) << "Expected 3 tasks in total.";

  // 🧩 Step 2 — Filter by status: Todo
  auto todoResult = repo->list(TaskStatus::Todo, std::nullopt, 100, 0);
  ASSERT_TRUE(todoResult.isOk()) << "Listing by status Todo should succeed.";

  const auto& todos = todoResult.value();
  EXPECT_EQ(todos.size(), 2) << "Expected 2 tasks with status Todo.";

  // 🧩 Step 3 — Filter by priority: Medium
  auto mediumResult = repo->list(std::nullopt, TaskPriority::Normal, 100, 0);
  ASSERT_TRUE(mediumResult.isOk()) << "Listing by priority Medium should succeed.";

  const auto& mediums = mediumResult.value();
  EXPECT_EQ(mediums.size(), 1) << "Expected 1 task with Medium priority.";

  // 🧩 Step 4 — Combined filter: Todo + High
  auto combinedResult = repo->list(TaskStatus::Todo, TaskPriority::High, 100, 0);
  ASSERT_TRUE(combinedResult.isOk()) << "Combined filter should succeed.";

  const auto& combined = combinedResult.value();
  EXPECT_EQ(combined.size(), 1) << "Expected 1 task with Todo status and High priority.";

  // 🧩 Step 5 — Pagination test: limit + offset
  auto pagedResult = repo->list(std::nullopt, std::nullopt, 1, 1);
  ASSERT_TRUE(pagedResult.isOk()) << "Paged listing should succeed.";
  const auto& paged = pagedResult.value();
  EXPECT_EQ(paged.size(), 1) << "Expected 1 task in paged result.";

  // 🧩 Step 6 — Empty repository case
  repo = std::make_unique<P1_S2_InMemoryTaskRepository>();
  auto emptyResult = repo->list(std::nullopt, std::nullopt, 100, 0);
  ASSERT_TRUE(emptyResult.isOk());
  EXPECT_TRUE(emptyResult.value().empty()) << "Expected empty list when repo is empty.";
}

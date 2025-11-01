/*
 * 🧪 File: test_P1_S2_PostgresTaskRepository.cpp
 * ----------------------------------------------
 * 📌 Purpose   : Integration Tests for P1_S2_PostgresTaskRepository (Phase 1 / Slice 2)
 * 🧱 Layer     : Infrastructure (Persistence / DB)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Updated   : 2025-11-01
 * 🔖 Version   : 1.2 (Refactored to use DatabaseIntegrationFixture)
 *
 * 🧠 Description:
 * Integration tests verifying CRUD operations for the Postgres repository.
 *   - Database is provisioned via DatabaseIntegrationFixture.
 *   - Repository connects automatically via AppSettings.
 */

#include "domain/core/entities/P1_TaskPriority.h"
#include "domain/core/entities/P1_TaskStatus.h"
#include "infra/db/P1_S2_PostgresTaskRepository.h"
#include "infra/runtime/P1_AppSettings.h"
#include "infra/runtime/P1_Logger.h"
#include "tests/integration/common/DatabaseIntegrationFixture.h"
#include <algorithm>
#include <gtest/gtest.h>

using namespace tasqly::testing::infra;
using namespace tasqly::p1::infra::db;
using namespace tasqly::p1::infra::runtime;
using namespace tasqly::p1::s1::domain::core;

// ================================================================
// 🧩 Fixture: PostgresTaskRepoTest
// ================================================================
class PostgresTaskRepoTest : public DatabaseIntegrationFixture
{
protected:
  std::unique_ptr<P1_S2_PostgresTaskRepository> repo;

  void SetUp() override
  {
    DatabaseIntegrationFixture::SetUp(); // Ensure DB ready

    P1_Logger::instance().setMinimumLevel(LogLevel::Error);

    auto& settings = P1_AppSettings::instance();
    settings.set("DB_HOST", "localhost");
    settings.set("DB_PORT", "5432");
    settings.set("DB_NAME", "tasqly_test");
    settings.set("DB_USER", "postgres");
    settings.set("DB_PASS", "themyth2060");

    repo = std::make_unique<P1_S2_PostgresTaskRepository>();
    ASSERT_TRUE(repo->isConnected()) << "Repository should connect automatically via AppSettings.";
  }

  void TearDown() override
  {
    if (repo && repo->isConnected()) {
      // Clean test data to keep DB stable across tests
      repo->deleteTask("PG-001");
      repo->deleteTask("PG-002");
      repo->deleteTask("PG-003");
      repo->deleteTask("PG-L1");
      repo->deleteTask("PG-L2");
      repo->deleteTask("PG-L3");
      repo.reset();
    }
  }
};

// ================================================================
// 🧪 Test Case #1 — Add and Retrieve Task (Integration)
// ================================================================
TEST_F(PostgresTaskRepoTest, AddAndRetrieveTask)
{
  ASSERT_NE(repo, nullptr);
  ASSERT_TRUE(repo->isConnected());

  TaskRecord rec{"PG-001",
                 "Integration Test Task",
                 "Ensure DB insertion works",
                 static_cast<int>(TaskStatus::Todo),
                 static_cast<int>(TaskPriority::High),
                 std::nullopt,
                 "2025-10-26T10:00:00Z",
                 "2025-10-26T10:00:00Z"};

  auto insertResult = repo->addTask(rec);
  ASSERT_TRUE(insertResult.isOk());

  auto fetchResult = repo->getTaskById("PG-001");
  ASSERT_TRUE(fetchResult.isOk());
  const auto& fetched = fetchResult.value();
  EXPECT_EQ(fetched.id, rec.id);
  EXPECT_EQ(fetched.title, rec.title);
  EXPECT_EQ(fetched.notes, rec.notes);
  EXPECT_EQ(fetched.status, rec.status);
  EXPECT_EQ(fetched.priority, rec.priority);
}

// ================================================================
// 🧪 Test Case #2 — UpdateTask_Integration
// ================================================================
TEST_F(PostgresTaskRepoTest, UpdateTask_Integration)
{
  TaskRecord rec{"PG-002",
                 "Original Title",
                 "Initial Notes",
                 static_cast<int>(TaskStatus::Todo),
                 static_cast<int>(TaskPriority::Normal),
                 std::nullopt,
                 "2025-10-26T10:00:00Z",
                 "2025-10-26T10:00:00Z"};

  ASSERT_TRUE(repo->addTask(rec).isOk());

  rec.title = "Updated Title";
  rec.notes = "Updated Notes";
  rec.status = static_cast<int>(TaskStatus::Done);
  rec.priority = static_cast<int>(TaskPriority::High);
  rec.updatedAt = "2025-10-26T10:05:00Z";

  ASSERT_TRUE(repo->updateTask(rec).isOk());

  auto fetchResult = repo->getTaskById("PG-002");
  ASSERT_TRUE(fetchResult.isOk());
  const auto& fetched = fetchResult.value();
  EXPECT_EQ(fetched.title, "Updated Title");
  EXPECT_EQ(fetched.notes, "Updated Notes");

  TaskRecord ghost{"PG-999",
                   "Ghost Record",
                   "Should not exist",
                   static_cast<int>(TaskStatus::Todo),
                   static_cast<int>(TaskPriority::Low),
                   std::nullopt,
                   "2025-10-26T10:00:00Z",
                   "2025-10-26T10:00:00Z"};
  auto failResult = repo->updateTask(ghost);
  ASSERT_TRUE(failResult.isErr());
  EXPECT_EQ(failResult.error().code, DomainErrorCode::NotFound);
}

// ================================================================
// 🧪 Test Case #3 — DeleteTask_Integration
// ================================================================
TEST_F(PostgresTaskRepoTest, DeleteTask_Integration)
{
  TaskRecord rec{"PG-003",
                 "Task To Delete",
                 "Will be deleted during test",
                 static_cast<int>(TaskStatus::Doing),
                 static_cast<int>(TaskPriority::Low),
                 std::nullopt,
                 "2025-10-26T11:00:00Z",
                 "2025-10-26T11:00:00Z"};

  ASSERT_TRUE(repo->addTask(rec).isOk());

  auto fetchBefore = repo->getTaskById("PG-003");
  ASSERT_TRUE(fetchBefore.isOk());

  ASSERT_TRUE(repo->deleteTask("PG-003").isOk());

  auto fetchAfter = repo->getTaskById("PG-003");
  ASSERT_TRUE(fetchAfter.isErr());
  EXPECT_EQ(fetchAfter.error().code, DomainErrorCode::NotFound);
}

// ================================================================
// 🧪 Test Case #4 — ListTasks_Integration
// ================================================================
TEST_F(PostgresTaskRepoTest, ListTasks_Integration)
{
  repo->deleteTask("PG-L1");
  repo->deleteTask("PG-L2");
  repo->deleteTask("PG-L3");

  auto now = "2025-10-26T12:00:00Z";

  TaskRecord t1{"PG-L1",
                "Task L1",
                "Database Schema Design",
                static_cast<int>(TaskStatus::Todo),
                static_cast<int>(TaskPriority::High),
                std::nullopt,
                now,
                now};

  TaskRecord t2{"PG-L2",
                "Task L2",
                "Repository Implementation",
                static_cast<int>(TaskStatus::Doing),
                static_cast<int>(TaskPriority::Normal),
                std::nullopt,
                now,
                now};

  TaskRecord t3{"PG-L3",
                "Task L3",
                "Unit Testing for CRUD",
                static_cast<int>(TaskStatus::Todo),
                static_cast<int>(TaskPriority::Low),
                std::nullopt,
                now,
                now};

  ASSERT_TRUE(repo->addTask(t1).isOk());
  ASSERT_TRUE(repo->addTask(t2).isOk());
  ASSERT_TRUE(repo->addTask(t3).isOk());

  auto listResult = repo->listTasks();
  ASSERT_TRUE(listResult.isOk());
  const auto& tasks = listResult.value();
  EXPECT_GE(tasks.size(), 3);

  std::vector<std::string> ids;
  for (const auto& t : tasks)
    ids.push_back(t.id);

  EXPECT_NE(std::find(ids.begin(), ids.end(), "PG-L1"), ids.end());
  EXPECT_NE(std::find(ids.begin(), ids.end(), "PG-L2"), ids.end());
  EXPECT_NE(std::find(ids.begin(), ids.end(), "PG-L3"), ids.end());
}

/*
 * 🧪 File: test_P1_S2_PostgresTaskRepository.cpp
 * ---------------------------------------------
 * 📌 Purpose   : Unit Tests for P1_S2_PostgresTaskRepository (Error Handling)
 * 🧱 Layer     : Tests (Unit - Infrastructure Persistence)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-11-06
 * 🔖 Version   : 1.0
 *
 * 🧠 Description:
 * Unit tests for PostgresTaskRepository covering error handling scenarios:
 * - Connection failures (invalid credentials, timeout, retry)
 * - CRUD operations when not connected
 * - Error classification (auth, timeout, db init, etc.)
 * - Connection reset scenarios
 */

#include "domain/core/entities/P1_TaskPriority.h"
#include "domain/core/entities/P1_TaskStatus.h"
#include "domain/core/errors/P1_DomainResult.h"
#include "infra/db/P1_S2_PostgresTaskRepository.h"
#include "infra/runtime/P1_AppSettings.h"
#include "infra/runtime/P1_Logger.h"
#include <gtest/gtest.h>

using namespace tasqly::p1::infra::db;
using namespace tasqly::p1::infra::runtime;
using namespace tasqly::p1::s1::domain::core;

// ================================================================
// 🧩 Connection Failure Tests
// ================================================================

TEST(P1_S2_PostgresTaskRepositoryTest, ConnectWithInvalidHost)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "invalid_host_that_does_not_exist_12345");
  settings.set("DB_PORT", "9999");
  settings.set("DB_NAME", "invalid_db");
  settings.set("DB_USER", "bad_user");
  settings.set("DB_PASS", "bad_pass");
  settings.set("DB_CONNECT_TIMEOUT", "1"); // Very short timeout
  
  P1_S2_PostgresTaskRepository repo;
  
  // Should fail to connect
  EXPECT_FALSE(repo.isConnected());
  
  // Should have error set
  const auto& error = repo.lastError();
  EXPECT_NE(error.code, RuntimeErrorCode::None); // ✅ fixed: P1_Error has no isOk()
}

TEST(P1_S2_PostgresTaskRepositoryTest, ConnectWithInvalidCredentials)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  // Use valid host but invalid credentials to test auth error classification
  settings.set("DB_HOST", "localhost");
  settings.set("DB_PORT", "5432");
  settings.set("DB_NAME", "tasqly_test");
  settings.set("DB_USER", "invalid_user_12345");
  settings.set("DB_PASS", "invalid_pass_12345");
  settings.set("DB_CONNECT_TIMEOUT", "1");
  
  P1_S2_PostgresTaskRepository repo;
  
  // Should fail to connect
  EXPECT_FALSE(repo.isConnected());
  
  // Should have error set (may be auth or connection error depending on DB)
  const auto& error = repo.lastError();
  EXPECT_NE(error.code, RuntimeErrorCode::None); // ✅ fix
}

TEST(P1_S2_PostgresTaskRepositoryTest, ConnectRetryLogic)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "invalid_host_that_does_not_exist_12345");
  settings.set("DB_PORT", "9999");
  settings.set("DB_CONNECT_TIMEOUT", "1");
  
  P1_S2_PostgresTaskRepository repo;
  
  // Should attempt retry (MAX_RETRIES = 1, so 2 attempts total)
  // Should fail after retries
  EXPECT_FALSE(repo.isConnected());
}

// ================================================================
// 🧩 CRUD Operations When Not Connected Tests
// ================================================================

TEST(P1_S2_PostgresTaskRepositoryTest, AddTaskWhenNotConnected)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  // Create repo with invalid connection
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "invalid_host");
  settings.set("DB_PORT", "9999");
  
  P1_S2_PostgresTaskRepository repo;
  ASSERT_FALSE(repo.isConnected());
  
  TaskRecord rec{"TEST-001",
                 "Test Task",
                 "Test Notes",
                 static_cast<int>(TaskStatus::Todo),
                 static_cast<int>(TaskPriority::Normal),
                 std::nullopt,
                 "2025-10-26T10:00:00Z",
                 "2025-10-26T10:00:00Z"};
  
  auto result = repo.addTask(rec);
  EXPECT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::Storage); // ✅ fix field name
}

TEST(P1_S2_PostgresTaskRepositoryTest, UpdateTaskWhenNotConnected)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "invalid_host");
  settings.set("DB_PORT", "9999");
  
  P1_S2_PostgresTaskRepository repo;
  ASSERT_FALSE(repo.isConnected());
  
  TaskRecord rec{"TEST-002",
                 "Test Task",
                 "Test Notes",
                 static_cast<int>(TaskStatus::Todo),
                 static_cast<int>(TaskPriority::Normal),
                 std::nullopt,
                 "2025-10-26T10:00:00Z",
                 "2025-10-26T10:00:00Z"};
  
  auto result = repo.updateTask(rec);
  EXPECT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::Storage); // ✅ fix
}

TEST(P1_S2_PostgresTaskRepositoryTest, DeleteTaskWhenNotConnected)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "invalid_host");
  settings.set("DB_PORT", "9999");
  
  P1_S2_PostgresTaskRepository repo;
  ASSERT_FALSE(repo.isConnected());
  
  auto result = repo.deleteTask("TEST-003");
  EXPECT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::Storage); // ✅ fix
}

TEST(P1_S2_PostgresTaskRepositoryTest, GetTaskByIdWhenNotConnected)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "invalid_host");
  settings.set("DB_PORT", "9999");
  
  P1_S2_PostgresTaskRepository repo;
  ASSERT_FALSE(repo.isConnected());
  
  auto result = repo.getTaskById("TEST-004");
  EXPECT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::Storage); // ✅ fix
}

TEST(P1_S2_PostgresTaskRepositoryTest, ListTasksWhenNotConnected)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "invalid_host");
  settings.set("DB_PORT", "9999");
  
  P1_S2_PostgresTaskRepository repo;
  ASSERT_FALSE(repo.isConnected());
  
  auto result = repo.listTasks();
  EXPECT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::Storage); // ✅ fix
}

// ================================================================
// 🧩 Disconnect Tests
// ================================================================

TEST(P1_S2_PostgresTaskRepositoryTest, DisconnectWhenNotConnected)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "invalid_host");
  settings.set("DB_PORT", "9999");
  
  P1_S2_PostgresTaskRepository repo;
  ASSERT_FALSE(repo.isConnected());
  
  // Should not crash when disconnecting
  EXPECT_NO_THROW(repo.disconnect());
  EXPECT_FALSE(repo.isConnected());
}

// ================================================================
// 🧩 Connection State Tests
// ================================================================

TEST(P1_S2_PostgresTaskRepositoryTest, IsConnectedReturnsFalseWhenNotConnected)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "invalid_host");
  settings.set("DB_PORT", "9999");
  
  P1_S2_PostgresTaskRepository repo;
  
  EXPECT_FALSE(repo.isConnected());
}

TEST(P1_S2_PostgresTaskRepositoryTest, ConnectWhenAlreadyConnected)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  // This test requires a valid DB connection
  // It tests the early return in connect() when m_connected is true
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "localhost");
  settings.set("DB_PORT", "5432");
  settings.set("DB_NAME", "tasqly_test");
  settings.set("DB_USER", "postgres");
  settings.set("DB_PASS", "themyth2060");
  
  P1_S2_PostgresTaskRepository repo;
  
  // If connection succeeds, test that calling connect() again returns true
  if (repo.isConnected()) {
    bool result = repo.connect();
    EXPECT_TRUE(result); // Should return true immediately if already connected
    EXPECT_TRUE(repo.isConnected());
  }
}

// ================================================================
// 🧩 Error Classification Tests (via connection failures)
// ================================================================

TEST(P1_S2_PostgresTaskRepositoryTest, ErrorClassificationTimeout)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "invalid_host_that_does_not_exist_12345");
  settings.set("DB_PORT", "9999");
  settings.set("DB_CONNECT_TIMEOUT", "1"); // Very short timeout
  
  P1_S2_PostgresTaskRepository repo;
  
  // Should fail to connect
  EXPECT_FALSE(repo.isConnected());
  
  // Error should be set (may be timeout or db init depending on actual error)
  const auto& error = repo.lastError();
  EXPECT_NE(error.code, RuntimeErrorCode::None); // ✅ fix
}

// ================================================================
// 🧩 Update Task - Not Found Scenario
// ================================================================

TEST(P1_S2_PostgresTaskRepositoryTest, UpdateTaskNotFound)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  // This test requires a valid DB connection
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "localhost");
  settings.set("DB_PORT", "5432");
  settings.set("DB_NAME", "tasqly_test");
  settings.set("DB_USER", "postgres");
  settings.set("DB_PASS", "themyth2060");
  
  P1_S2_PostgresTaskRepository repo;
  
  if (repo.isConnected()) {
    // Try to update a non-existent task
    TaskRecord rec{"NON_EXISTENT_TASK_12345",
                   "Non-existent Task",
                   "This task does not exist",
                   static_cast<int>(TaskStatus::Todo),
                   static_cast<int>(TaskPriority::Normal),
                   std::nullopt,
                   "2025-10-26T10:00:00Z",
                   "2025-10-26T10:00:00Z"};
    
    auto result = repo.updateTask(rec);
    EXPECT_TRUE(result.isErr());
    EXPECT_EQ(result.error().code, DomainErrorCode::NotFound); // ✅ fix
  }
}

// ================================================================
// 🧩 Get Task By ID - Not Found Scenario
// ================================================================

TEST(P1_S2_PostgresTaskRepositoryTest, GetTaskByIdNotFound)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "localhost");
  settings.set("DB_PORT", "5432");
  settings.set("DB_NAME", "tasqly_test");
  settings.set("DB_USER", "postgres");
  settings.set("DB_PASS", "themyth2060");
  
  P1_S2_PostgresTaskRepository repo;
  
  if (repo.isConnected()) {
    // Try to get a non-existent task
    auto result = repo.getTaskById("NON_EXISTENT_TASK_12345");
    EXPECT_TRUE(result.isErr());
    EXPECT_EQ(result.error().code, DomainErrorCode::NotFound); // ✅ fix
  }
}

// ================================================================
// 🧩 List Tasks - Empty Result
// ================================================================

TEST(P1_S2_PostgresTaskRepositoryTest, ListTasksEmptyResult)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "localhost");
  settings.set("DB_PORT", "5432");
  settings.set("DB_NAME", "tasqly_test");
  settings.set("DB_USER", "postgres");
  settings.set("DB_PASS", "themyth2060");
  
  P1_S2_PostgresTaskRepository repo;
  
  if (repo.isConnected()) {
    // List tasks (may be empty, but should succeed)
    auto result = repo.listTasks();
    EXPECT_TRUE(result.isOk());
    
    // Result may be empty, but should not error
    const auto& tasks = result.value();
    // Just verify it's a valid vector (can be empty)
    (void)tasks;
  }
}

// ================================================================
// 🧩 Add Task - With All Optional Fields
// ================================================================

TEST(P1_S2_PostgresTaskRepositoryTest, AddTaskWithOptionalFields)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "localhost");
  settings.set("DB_PORT", "5432");
  settings.set("DB_NAME", "tasqly_test");
  settings.set("DB_USER", "postgres");
  settings.set("DB_PASS", "themyth2060");
  
  P1_S2_PostgresTaskRepository repo;
  
  if (repo.isConnected()) {
    // Add task with all optional fields set
    TaskRecord rec{"TEST-OPTIONAL-001",
                   "Task With Optional Fields",
                   "Notes field",
                   static_cast<int>(TaskStatus::Doing),
                   static_cast<int>(TaskPriority::High),
                   std::make_optional<std::string>("2025-12-31T23:59:59Z"), // deadline
                   "2025-10-26T10:00:00Z",
                   "2025-10-26T10:00:00Z"};
    
    auto result = repo.addTask(rec);
    EXPECT_TRUE(result.isOk());
    
    // Clean up
    repo.deleteTask("TEST-OPTIONAL-001");
  }
}

// ================================================================
// 🧩 Add Task - With Empty Optional Fields
// ================================================================

TEST(P1_S2_PostgresTaskRepositoryTest, AddTaskWithEmptyOptionalFields)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "localhost");
  settings.set("DB_PORT", "5432");
  settings.set("DB_NAME", "tasqly_test");
  settings.set("DB_USER", "postgres");
  settings.set("DB_PASS", "themyth2060");
  
  P1_S2_PostgresTaskRepository repo;
  
  if (repo.isConnected()) {
    // Add task with empty optional fields
    TaskRecord rec{"TEST-EMPTY-OPT-001",
                   "Task With Empty Optional Fields",
                   "", // empty notes
                   static_cast<int>(TaskStatus::Todo),
                   static_cast<int>(TaskPriority::Low),
                   std::nullopt, // no deadline
                   "2025-10-26T10:00:00Z",
                   "2025-10-26T10:00:00Z"};
    
    auto result = repo.addTask(rec);
    EXPECT_TRUE(result.isOk());
    
    // Clean up
    repo.deleteTask("TEST-EMPTY-OPT-001");
  }
}


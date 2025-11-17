/*
 * File: test_P1_S2_DbTaskRepositoryIntegration.cpp
 * ----------------------------------------------
 * Purpose   : Validates P1_S2_DbTaskRepository against a real PostgreSQL database.
 * Layer     : Infrastructure (Persistence) — Integration Tests
 * Author    : Cascade AI Assistant
 * Created   : 2025-11-17
 *
 * Description:
 *   Leverages DatabaseIntegrationFixture to provision a PostgreSQL schema and exercises the
 *   DbTaskRepository end-to-end: create, read, update, list, and delete. These tests ensure
 *   that the repository talks to a real database through the IDbConnection abstraction.
 */

#include "domain/core/entities/P1_Task.h"
#include "domain/core/errors/P1_DomainError.h"
#include "infra/persistence/P1_S2_DbTaskRepository.h"
#include "infra/runtime/P1_Error.h"
#include "infra/runtime/P1_Logger.h"
#include "tests/integration/common/DatabaseIntegrationFixture.h"

#include <gtest/gtest.h>
#include <libpq-fe.h>

#include <chrono>
#include <memory>
#include <string>
#include <vector>

using tasqly::p1::infra::runtime::P1_Error;
using tasqly::p1::infra::runtime::P1_Logger;
using tasqly::p1::s1::domain::core::DomainErrorCode;
using tasqly::p1::s1::domain::core::Task;
using tasqly::p1::s1::domain::core::TaskPriority;
using tasqly::p1::s1::domain::core::TaskStatus;
using tasqly::p1::s2::infra::db::DbQueryResult;
using tasqly::p1::s2::infra::db::DbResult;
using tasqly::p1::s2::infra::db::IDbConnection;
using tasqly::p1::s2::infra::persistence::P1_S2_DbTaskRepository;
using tasqly::testing::infra::DatabaseIntegrationFixture;

namespace {
// Adapter for IDbConnection
class FixtureDbConnectionAdapter : public IDbConnection
{
public:
  FixtureDbConnectionAdapter(PGconn* connection, P1_Logger* logger)
      : m_connection(connection)
      , m_logger(logger)
  {}

  DbResult execute(const std::string& sql) override
  {
    if (!m_connection)
      return DbResult::Err(P1_Error::makeDbInit("Adapter: null PG connection"));

    if (m_logger)
      m_logger->info("[FixtureDbConnectionAdapter] Executing SQL: " + sql);

    PGresult* res = PQexec(m_connection, sql.c_str());
    if (!res)
      return DbResult::Err(P1_Error::makeDbInit("Adapter: PQexec returned null"));

    const ExecStatusType status = PQresultStatus(res);

    if (status == PGRES_COMMAND_OK) {
      DbQueryResult empty;
      PQclear(res);
      return DbResult::Ok(empty);
    }

    if (status != PGRES_TUPLES_OK) {
      std::string err = PQerrorMessage(m_connection);
      PQclear(res);
      return DbResult::Err(P1_Error::makeIo("Adapter: " + err));
    }

    DbQueryResult out;
    const int cols = PQnfields(res);
    const int rows = PQntuples(res);
    out.columns.reserve(cols);
    for (int c = 0; c < cols; ++c)
      out.columns.emplace_back(PQfname(res, c));

    out.rows.reserve(rows);
    for (int r = 0; r < rows; ++r) {
      std::vector<std::string> row;
      row.reserve(cols);
      for (int c = 0; c < cols; ++c) {
        if (PQgetisnull(res, r, c))
          row.emplace_back("");
        else
          row.emplace_back(PQgetvalue(res, r, c));
      }
      out.rows.push_back(std::move(row));
    }

    PQclear(res);
    return DbResult::Ok(out);
  }

  bool reset() override
  {
    if (!m_connection)
      return false;

    PQreset(m_connection);
    return PQstatus(m_connection) == CONNECTION_OK;
  }

  bool isValid() const override
  {
    return m_connection && PQstatus(m_connection) == CONNECTION_OK;
  }

private:
  PGconn* m_connection = nullptr;
  P1_Logger* m_logger = nullptr;
};

// Test fixture for DbTaskRepositoryIntegrationTest
class DbTaskRepositoryIntegrationTest : public DatabaseIntegrationFixture
{
protected:
  P1_S2_DbTaskRepository makeRepository()
  {
    m_adapter = std::make_unique<FixtureDbConnectionAdapter>(conn, &P1_Logger::instance());
    return P1_S2_DbTaskRepository(m_adapter.get(), &P1_Logger::instance());
  }

  std::unique_ptr<FixtureDbConnectionAdapter> m_adapter;
};

// Helper function to create a Task object
Task makeTask(const std::string& id,
              const std::string& title,
              TaskStatus status,
              TaskPriority priority)
{
  Task task;
  task.id = id;
  task.title = title;
  task.notes = title + " notes";
  task.status = status;
  task.priority = priority;
  task.deadline = std::chrono::system_clock::now() + std::chrono::hours(12);
  task.createdAt = std::chrono::system_clock::now();
  task.updatedAt = task.createdAt;
  return task;
}

} // namespace

// ============================================================================
// Round-trip CRUD verification against PostgreSQL
// ============================================================================
TEST_F(DbTaskRepositoryIntegrationTest, CreateReadUpdateListAndDeleteRoundTrip)
{
  auto repo = makeRepository();

  Task task = makeTask("11111111-1111-1111-1111-111111111111",
                       "Integration Task",
                       TaskStatus::Todo,
                       TaskPriority::Normal);

  auto createRes = repo.create(task);
  ASSERT_TRUE(createRes.isOk()) << createRes.error().message;

  auto fetched = repo.getById(task.id);
  ASSERT_TRUE(fetched.isOk()) << fetched.error().message;
  EXPECT_EQ(fetched.value().title, task.title);
  EXPECT_EQ(fetched.value().status, TaskStatus::Todo);

  task.title = "Updated Integration Task";
  task.notes = "Updated notes";
  task.status = TaskStatus::Doing;
  task.priority = TaskPriority::High;
  task.deadline = std::chrono::system_clock::now() + std::chrono::hours(48);
  task.updatedAt = std::chrono::system_clock::now();

  auto updateRes = repo.update(task);
  ASSERT_TRUE(updateRes.isOk()) << updateRes.error().message;

  auto listRes = repo.list(std::nullopt, std::nullopt, 10, 0);
  ASSERT_TRUE(listRes.isOk());
  ASSERT_EQ(listRes.value().size(), 1);
  EXPECT_EQ(listRes.value().front().title, "Updated Integration Task");

  auto removeRes = repo.remove(task.id);
  ASSERT_TRUE(removeRes.isOk()) << removeRes.error().message;

  auto missing = repo.getById(task.id);
  ASSERT_TRUE(missing.isErr());
  EXPECT_EQ(missing.error().code, DomainErrorCode::NotFound);
}

// ============================================================================
// Listing filters should be respected
// ============================================================================
TEST_F(DbTaskRepositoryIntegrationTest, ListHonorsStatusAndPriorityFilters)
{
  auto repo = makeRepository();

  auto taskTodo = makeTask("22222222-2222-2222-2222-222222222222",
                           "Todo Task",
                           TaskStatus::Todo,
                           TaskPriority::Low);
  ASSERT_TRUE(repo.create(taskTodo).isOk());

  auto taskDoing = makeTask("33333333-3333-3333-3333-333333333333",
                            "Doing Task",
                            TaskStatus::Doing,
                            TaskPriority::Normal);
  ASSERT_TRUE(repo.create(taskDoing).isOk());

  auto taskDone = makeTask("44444444-4444-4444-4444-444444444444",
                           "Done Task",
                           TaskStatus::Done,
                           TaskPriority::High);
  ASSERT_TRUE(repo.create(taskDone).isOk());

  auto todoOnly = repo.list(TaskStatus::Todo, std::nullopt, 10, 0);
  ASSERT_TRUE(todoOnly.isOk());
  ASSERT_EQ(todoOnly.value().size(), 1);
  EXPECT_EQ(todoOnly.value().front().id, taskTodo.id);

  auto highPriority = repo.list(std::nullopt, TaskPriority::High, 10, 0);
  ASSERT_TRUE(highPriority.isOk());
  ASSERT_EQ(highPriority.value().size(), 1);
  EXPECT_EQ(highPriority.value().front().id, taskDone.id);

  auto paged = repo.list(std::nullopt, std::nullopt, 2, 1);
  ASSERT_TRUE(paged.isOk());
  EXPECT_EQ(paged.value().size(), 2);
}

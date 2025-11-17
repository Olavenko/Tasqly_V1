/*
 * 🧪 Integration Test — P1_S2_PostgresConnection
 * ----------------------------------------------
 * Validates the real PostgreSQL connection implementation against the shared
 * DatabaseIntegrationFixture to ensure SQL commands and queries succeed
 * end-to-end using libpq.
 */

#include <gtest/gtest.h>

#include "infra/db/P1_S2_PostgresConnection.h"
#include "infra/runtime/P1_Logger.h"
#include "tests/integration/common/DatabaseIntegrationFixture.h"

#include <string>

using tasqly::p1::infra::runtime::P1_Logger;
using tasqly::p1::s2::infra::db::DbQueryResult;
using tasqly::p1::s2::infra::db::P1_S2_PostgresConnection;
using tasqly::testing::infra::DatabaseIntegrationFixture;

namespace {

class PostgresConnectionIntegrationTest : public DatabaseIntegrationFixture
{};

std::string makeInsertSql(const std::string& id)
{
  return "INSERT INTO tasks (id, title, description, status, priority, deadline, created_at, "
         "updated_at) VALUES ('"
         + id + "', 'Integration Task', 'notes', 0, 1, NULL, NOW(), NOW());";
}

} // namespace

// ------------------------------------------------------------------
// Executes INSERT + SELECT round-trip via the real connection.
// ------------------------------------------------------------------
TEST_F(PostgresConnectionIntegrationTest, InsertAndSelectRoundTrip)
{
  P1_S2_PostgresConnection pgconn(testDbConnectionString(), &P1_Logger::instance());
  ASSERT_TRUE(pgconn.isValid());

  const std::string taskId = "aaaaaaaa-bbbb-cccc-dddd-eeeeeeee0001";

  auto insertRes = pgconn.execute(makeInsertSql(taskId));
  ASSERT_TRUE(insertRes.isOk()) << insertRes.error().message;

  auto selectRes = pgconn.execute("SELECT id, title, status, priority FROM tasks WHERE id = '"
                                + taskId + "';");
  ASSERT_TRUE(selectRes.isOk()) << selectRes.error().message;

  const DbQueryResult& rows = selectRes.value();
  ASSERT_EQ(rows.columns.size(), 4u);
  EXPECT_EQ(rows.columns[0], "id");

  ASSERT_EQ(rows.rows.size(), 1u);
  const auto& row = rows.rows.front();
  ASSERT_EQ(row.size(), 4u);
  EXPECT_EQ(row[0], taskId);
  EXPECT_EQ(row[1], "Integration Task");
  EXPECT_EQ(row[2], "0"); // status enum stored as text
  EXPECT_EQ(row[3], "1"); // priority enum stored as text
}

// ------------------------------------------------------------------
// reset() should keep the real connection valid.
// ------------------------------------------------------------------
TEST_F(PostgresConnectionIntegrationTest, ResetKeepsConnectionHealthy)
{
  P1_S2_PostgresConnection pgconn(testDbConnectionString(), &P1_Logger::instance());
  ASSERT_TRUE(pgconn.isValid());

  EXPECT_TRUE(pgconn.reset());
  EXPECT_TRUE(pgconn.isValid());
}

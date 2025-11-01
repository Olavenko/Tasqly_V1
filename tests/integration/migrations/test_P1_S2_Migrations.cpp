/*
 * 🧪 File: test_P1_S2_Migrations.cpp
 * ----------------------------------------------
 * 📌 Purpose   : Verify that migration scripts can be applied and rolled back safely.
 * 🧱 Layer     : Infrastructure (Migrations)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-26
 * 🗓️ Updated   : 2025-11-01
 *
 * 🧠 Description:
 * Ensures that P1_S2_create_tasks.sql and P1_S2_drop_tasks.sql:
 *   - Apply schema correctly.
 *   - Can be rolled back cleanly.
 *   - Are idempotent (safe to re-run).
 */

#include "tests/integration/common/DatabaseIntegrationFixture.h"
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>

using namespace tasqly::testing::infra;

// ================================================================
// 🧪 Test Case — Migration Apply & Rollback Verification
// ================================================================
class MigrationIntegrationTest : public DatabaseIntegrationFixture
{
protected:
  // 📦 Utility: Load SQL file contents
  static std::string loadSqlFile(const std::string& path)
  {
    std::ifstream file(path);
    if (!file.is_open()) {
      ADD_FAILURE() << "Cannot open SQL file: " << path;
      return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  }

  // 📦 Utility: Check if a table exists
  bool tableExists(const std::string& table)
  {
    std::string query = "SELECT EXISTS (SELECT 1 FROM information_schema.tables "
                        "WHERE table_name = '"
                        + table + "');";
    PGresult* res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
      PQclear(res);
      return false;
    }
    bool exists = std::string(PQgetvalue(res, 0, 0)) == "t";
    PQclear(res);
    return exists;
  }
};

// ================================================================
// 🧪 Test Implementation
// ================================================================
TEST_F(MigrationIntegrationTest, ApplyAndRollbackMigrations)
{
  std::string baseDir = std::string(PROJECT_SOURCE_DIR) + "/src/infra/migrations/";
  const std::string createSql = loadSqlFile(baseDir + "P1_S2_create_tasks.sql");
  const std::string dropSql = loadSqlFile(baseDir + "P1_S2_drop_tasks.sql");

  ASSERT_FALSE(createSql.empty()) << "Missing create migration file!";
  ASSERT_FALSE(dropSql.empty()) << "Missing drop migration file!";

  // 🧩 Step 1 — Ensure clean state
  PQexec(conn, dropSql.c_str());

  // 🧩 Step 2 — Apply migration
  PGresult* res = PQexec(conn, createSql.c_str());
  ASSERT_EQ(PQresultStatus(res), PGRES_COMMAND_OK)
      << "Migration apply failed: " << PQerrorMessage(conn);
  PQclear(res);

  // 🧾 Verify table exists
  ASSERT_TRUE(tableExists("tasks")) << "Table 'tasks' must exist after migration.";

  // 🧩 Step 3 — Rollback migration
  res = PQexec(conn, dropSql.c_str());
  ASSERT_EQ(PQresultStatus(res), PGRES_COMMAND_OK)
      << "Migration rollback failed: " << PQerrorMessage(conn);
  PQclear(res);

  // 🧾 Verify table removed
  ASSERT_FALSE(tableExists("tasks")) << "Table 'tasks' must be removed after rollback.";

  // 🧩 Step 4 — Re-run apply (idempotence)
  res = PQexec(conn, createSql.c_str());
  ASSERT_EQ(PQresultStatus(res), PGRES_COMMAND_OK)
      << "Migration re-apply failed: " << PQerrorMessage(conn);
  PQclear(res);

  ASSERT_TRUE(tableExists("tasks")) << "Table should exist after re-apply.";

  // 🧩 Step 5 — Cleanup
  res = PQexec(conn, dropSql.c_str());
  PQclear(res);
}

/*
 * 🧪 File: test_P1_S2_TaskRepositoryFactory.cpp
 * ----------------------------------------------
 * 📌 Purpose   : Integration Test for Postgres → InMemory fallback logic
 * 🧱 Layer     : Infrastructure (Factory)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-26
 * 🔖 Version   : 1.1 (Aligned with factory behavior)
 *
 * 🧠 Description:
 * Verifies that the repository factory correctly:
 *   - Tries to initialize PostgreSQL repository first.
 *   - Falls back to InMemory when PostgreSQL initialization fails.
 *   - Returns nullptr when fallback is disabled.
 */

#include "infra/factories/P1_S2_TaskRepositoryFactory.h"
#include "infra/persistence/P1_S2_InMemoryTaskRepository.h"
#include "infra/runtime/P1_AppSettings.h"
#include "infra/runtime/P1_Logger.h"
#include <gtest/gtest.h>

using namespace tasqly::p1::infra::factories;
using namespace tasqly::p1::infra::runtime;
using namespace tasqly::p1::infra::db;
using namespace tasqly::p1::infra::persistence;

// ================================================================
// 🧩 Fixture: FactoryFallbackIntegrationTest
// ================================================================
class FactoryFallbackIntegrationTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // 🧱 Reduce log noise
    P1_Logger::instance().setMinimumLevel(LogLevel::Error);

    // 🧩 Inject invalid PostgreSQL credentials
    auto& settings = P1_AppSettings::instance();
    settings.set("DB_HOST", "invalid_host");
    settings.set("DB_PORT", "9999");
    settings.set("DB_NAME", "invalid_db");
    settings.set("DB_USER", "bad_user");
    settings.set("DB_PASS", "bad_pass");

    // Enable fallback feature flag
    settings.set("features.db.fallback_inmemory", "true");
    settings.set("features.inmemory.seed", "false");
  }

  void TearDown() override
  {
    // Reset fallback flag
    P1_AppSettings::instance().set("features.db.fallback_inmemory", "true");
  }
};

// ================================================================
// 🧪 Test Case #5 — FallbackToInMemoryWhenPostgresFails (Corrected)
// ================================================================
TEST_F(FactoryFallbackIntegrationTest, FallbackToInMemoryWhenPostgresFails)
{
  auto& factory = P1_S2_TaskRepositoryFactory::instance();

  // 🧩 Act — Attempt repository creation (Postgres expected to fail)
  auto repoVoid = factory.createRepository();

  // 🧾 Assert — Ensure repository was created and is InMemory
  ASSERT_NE(repoVoid, nullptr) << "Expected fallback repository to be created.";

  // 🧩 Cast and verify type
  auto memRepo = std::static_pointer_cast<P1_S2_InMemoryTaskRepository>(repoVoid);
  ASSERT_NE(memRepo, nullptr) << "Expected InMemory repository after Postgres failure.";

  // 🧩 Verify fallback mode
  EXPECT_EQ(factory.currentMode(), "InMemory (Fallback)");

  // 🧩 Verify InMemory CRUD works
  Task t{"FB-001",
         "Fallback Verification",
         "Should exist only in memory",
         TaskStatus::Todo,
         TaskPriority::High,
         std::nullopt,
         std::chrono::system_clock::now(),
         std::chrono::system_clock::now()};

  auto insertRes = memRepo->create(t);
  ASSERT_TRUE(insertRes.isOk()) << "Expected successful creation in memory.";

  auto fetchRes = memRepo->getById("FB-001");
  ASSERT_TRUE(fetchRes.isOk()) << "Expected to retrieve the same task from memory.";
  EXPECT_EQ(fetchRes.value().title, "Fallback Verification");

  // 🧩 Verify list contains the inserted task
  auto listRes = memRepo->list(std::nullopt, std::nullopt, 100, 0);
  ASSERT_TRUE(listRes.isOk());
  EXPECT_GE(listRes.value().size(), 1);
  EXPECT_EQ(listRes.value().front().id, "FB-001");
}

// ================================================================
// 🧪 Test Case #6 — ReturnsNullWhenFallbackDisabled (Corrected)
// ================================================================
TEST_F(FactoryFallbackIntegrationTest, ReturnsNullWhenFallbackDisabled)
{
  auto& factory = P1_S2_TaskRepositoryFactory::instance();

  // 🧱 Arrange — Disable fallback flag
  auto& settings = P1_AppSettings::instance();
  settings.set("features.db.fallback_inmemory", "false");

  // Inject invalid DB credentials to force Postgres failure
  settings.set("DB_HOST", "bad_host");
  settings.set("DB_PORT", "9999");
  settings.set("DB_NAME", "bad_db");
  settings.set("DB_USER", "bad_user");
  settings.set("DB_PASS", "bad_pass");

  // 🧩 Act — Attempt to create repository
  auto repoVoid = factory.createRepository();

  // 🧾 Assert — Should return nullptr (no fallback)
  EXPECT_EQ(repoVoid, nullptr) << "Expected nullptr when fallback is disabled.";

  // 🧩 Verify factory mode updated correctly
  EXPECT_EQ(factory.currentMode(), "Offline (No Repository)")
      << "Expected factory mode to indicate offline/no repo state.";
}

/*
 * 🧪 File: test_P1_S2_Migrations.cpp
 * ----------------------------------------------
 * 📌 Purpose   : Verify that migration scripts can be applied and rolled back safely.
 * 🧱 Layer     : Infrastructure (Migrations)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-26
 *
 * 🧠 Description:
 * Ensures that P1_S2_create_tasks.sql and P1_S2_drop_tasks.sql:
 *   - Apply schema correctly.
 *   - Can be rolled back cleanly.
 *   - Are idempotent (safe to re-run).
 */

#include "infra/runtime/P1_AppSettings.h"
#include "infra/runtime/P1_Logger.h"
#include <fstream>
#include <gtest/gtest.h>
#include <libpq-fe.h>
#include <sstream>

using namespace tasqly::p1::infra::runtime;

// ================================================================
// 🧩 Fixture: MigrationIntegrationTest
// ================================================================
class MigrationIntegrationTest : public ::testing::Test
{
protected:
  PGconn* conn = nullptr;

  void SetUp() override
  {
    auto& settings = P1_AppSettings::instance();

    // 🧱 Safely fetch each field using getString()
    const std::string host = settings.getString("DB_HOST").value_or("localhost");
    const std::string port = settings.getString("DB_PORT").value_or("5432");
    const std::string dbname = settings.getString("DB_NAME").value_or("tasqly_test");
    const std::string user = settings.getString("DB_USER").value_or("postgres");
    const std::string pass = settings.getString("DB_PASS").value_or("");

    // 🧩 Construct connection string
    const std::string conninfo = "host=" + host + " port=" + port + " dbname=" + dbname
                                 + " user=" + user + " password=" + pass;

    // 🧩 Connect to PostgreSQL
    conn = PQconnectdb(conninfo.c_str());
    ASSERT_EQ(PQstatus(conn), CONNECTION_OK)
        << "Failed to connect to PostgreSQL.\nConnection Info: " << conninfo
        << "\nError: " << PQerrorMessage(conn);
  }

  void TearDown() override
  {
    if (conn) {
      PQfinish(conn);
      conn = nullptr;
    }
  }

  static std::string loadSqlFile(const std::string& path)
  {
    std::ifstream file(path);

    if (!file.is_open()) {
      ADD_FAILURE() << "Cannot open SQL file: " << path;
      return std::string();
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  }

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
// 🧪 Test Case #7 — Migration Apply & Rollback Verification
// ================================================================
TEST_F(MigrationIntegrationTest, ApplyAndRollbackMigrations)
{
  // 🧱 Load SQL scripts
  const std::string createSql = loadSqlFile("infrastructure/migrations/P1_S2_create_tasks.sql");
  const std::string dropSql = loadSqlFile("infrastructure/migrations/P1_S2_drop_tasks.sql");

  // 🧩 Step 1 — Ensure clean state before test
  PQexec(conn, dropSql.c_str());

  // 🧩 Step 2 — Apply migration
  PGresult* res = PQexec(conn, createSql.c_str());
  ASSERT_EQ(PQresultStatus(res), PGRES_COMMAND_OK)
      << "Migration apply failed: " << PQerrorMessage(conn);
  PQclear(res);

  // 🧾 Verify table exists
  ASSERT_TRUE(tableExists("tasks")) << "Table 'tasks' must exist after migration.";

  // 🧩 Step 3 — Rollback migration (drop table)
  res = PQexec(conn, dropSql.c_str());
  ASSERT_EQ(PQresultStatus(res), PGRES_COMMAND_OK)
      << "Migration rollback failed: " << PQerrorMessage(conn);
  PQclear(res);

  // 🧾 Verify table no longer exists
  ASSERT_FALSE(tableExists("tasks")) << "Table 'tasks' must be removed after rollback.";

  // 🧩 Step 4 — Re-run apply for idempotence
  res = PQexec(conn, createSql.c_str());
  ASSERT_EQ(PQresultStatus(res), PGRES_COMMAND_OK) << "Migration re-apply failed.";
  PQclear(res);

  ASSERT_TRUE(tableExists("tasks")) << "Table should exist after re-apply.";

  // 🧩 Step 5 — Clean up again
  res = PQexec(conn, dropSql.c_str());
  PQclear(res);
}

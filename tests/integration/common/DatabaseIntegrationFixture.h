#pragma once
/*
 * 🧱 File: DatabaseIntegrationFixture.h
 * -------------------------------------
 * 📌 Purpose   : Provides a unified PostgreSQL test database environment for all integration tests.
 * 🧱 Layer     : Testing Infrastructure / Common
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-11-01
 * 🔖 Version   : 1.3
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * This fixture ensures all integration tests share one isolated database (tasqly_test)
 * that persists for the full test run.
 * It:
 *   - Creates the DB once.
 *   - Applies schema migrations.
 *   - Enables extensions (pgcrypto).
 *   - Provides a reusable connection handle.
 *   - Cleans up tables (not the whole DB) before each test.
 *
 * 🚀 Added Improvements:
 *   ✅ Detailed logging for each DB operation.
 *   ✅ Keeps DB alive (no DROP between tests).
 *   ✅ Truncates tables safely before each test to ensure isolation.
 */

#include <cstdlib>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <libpq-fe.h>
#include <sstream>
#include <string>

namespace tasqly::testing::infra {

class DatabaseIntegrationFixture : public ::testing::Test
{
protected:
  static PGconn* adminConn;
  static PGconn* conn;

  // ============================================================
  // 🧱 Global Setup (runs ONCE before all test suites)
  // ============================================================
  static void SetUpTestSuite()
  {
    const char* ci = std::getenv("CI");
    const std::string host = "localhost";
    const std::string port = "5432";
    const std::string user = "postgres";
    const std::string pass = "themyth2060";
    const std::string testDb = "tasqly_test";

#ifdef _WIN32
    if (ci && std::string(ci) == "true") {
      std::cout << "\n[DB FIXTURE]️ Detected Windows CI Environment.\n";
      std::cout << "[DB FIXTURE] Skipping PostgreSQL integration tests on Windows CI.\n";
      GTEST_SKIP() << "Skipping PostgreSQL integration tests on Windows CI (GitHub Actions).";
      return;
    } else {
      std::cout << "\n[DB FIXTURE] Running on Windows Local Environment. trying to connect to "
                   "PostgreSQL.\n";

      std::string adminConnStr = "host=" + host + " port=" + port + " dbname=postgres user=" + user
                                 + " password=" + pass;
      PGconn* testConn = PQconnectdb(adminConnStr.c_str());

      if (PQstatus(testConn) != CONNECTION_OK) {
        std::cout << "[DB FIXTURE] No local PostgreSQL server detected. skipping integration "
                     "tests.\n";
        PQfinish(testConn);
        GTEST_SKIP() << "PostgreSQL not running locally. Skipping integration tests.";
        return;
      } else {
        std::cout << "[DB FIXTURE] Local PostgreSQL detected. proceeding with integration tests.\n";
        PQfinish(testConn);
      }
    }
#endif

    std::cout << "\n[DB FIXTURE] ==============================================\n";
    std::cout << "[DB FIXTURE] Initializing PostgreSQL Test Environment...\n";

    // 🧩 Step 1 — Connect to the admin DB
    std::string adminConnStr = "host=" + host + " port=" + port + " dbname=postgres user=" + user
                               + " password=" + pass;
    adminConn = PQconnectdb(adminConnStr.c_str());
    ASSERT_EQ(PQstatus(adminConn), CONNECTION_OK)
        << " Cannot connect to Postgres admin DB: " << PQerrorMessage(adminConn);
    std::cout << "[DB FIXTURE] Connected to 'postgres' admin database.\n";

    // 🧩 Step 2 — Ensure test DB exists (create if missing)
    PGresult* res = PQexec(adminConn,
                           ("SELECT 1 FROM pg_database WHERE datname='" + testDb + "';").c_str());
    bool exists = (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0);
    PQclear(res);

    if (!exists) {
      std::cout << "[DB FIXTURE] Creating test database '" << testDb << "'...\n";
      res = PQexec(adminConn, ("CREATE DATABASE " + testDb + " OWNER " + user + ";").c_str());
      ASSERT_EQ(PQresultStatus(res), PGRES_COMMAND_OK)
          << " Failed to create test DB: " << PQerrorMessage(adminConn);
      PQclear(res);
      std::cout << "[DB FIXTURE] Test database created successfully.\n";
    } else {
      std::cout << "[DB FIXTURE] Test database '" << testDb << "' already exists.\n";
    }

    // 🧩 Step 3 — Connect to the test database
    std::string testConnStr = "host=" + host + " port=" + port + " dbname=" + testDb
                              + " user=" + user + " password=" + pass;
    conn = PQconnectdb(testConnStr.c_str());
    ASSERT_EQ(PQstatus(conn), CONNECTION_OK)
        << "Cannot connect to test DB: " << PQerrorMessage(conn);
    std::cout << "[DB FIXTURE] Connected to test DB successfully.\n";

    // 🧩 Step 4 — Enable pgcrypto
    res = PQexec(conn, "CREATE EXTENSION IF NOT EXISTS pgcrypto;");
    if (PQresultStatus(res) == PGRES_COMMAND_OK)
      std::cout << "[DB FIXTURE] pgcrypto extension ready.\n";
    else
      std::cout << "[DB FIXTURE] Failed to enable pgcrypto: " << PQerrorMessage(conn) << "\n";
    PQclear(res);

    // 🧩 Step 5 — Apply initial migration (schema)
    const std::string migrationPath = std::string(PROJECT_SOURCE_DIR)
                                      + "/src/infra/migrations/P1_S2_create_tasks.sql";
    std::cout << "[DB FIXTURE] Applying migration from: " << migrationPath << "\n";
    applyMigration(conn, migrationPath);
    std::cout << "[DB FIXTURE] Schema initialized successfully.\n";

    std::cout << "[DB FIXTURE] ==============================================\n\n";
  }

  // ============================================================
  // 🧱 Global Teardown (runs ONCE after all tests)
  // ============================================================
  static void TearDownTestSuite()
  {
    std::cout << "\n[DB FIXTURE] Shutting down PostgreSQL Test Environment...\n";

    if (conn) {
      std::cout << "[DB FIXTURE] Closing connection to test DB...\n";
      PQfinish(conn);
      conn = nullptr;
    }

    if (adminConn) {
      std::cout << "[DB FIXTURE] Keeping database 'tasqly_test' for inspection.\n";
      PQfinish(adminConn);
      adminConn = nullptr;
    }

    std::cout << "[DB FIXTURE] Environment cleanup complete.\n\n";
  }

  // ============================================================
  // 🧹 Per-test Setup (runs BEFORE each test)
  // ============================================================
  void SetUp() override
  {
    ASSERT_TRUE(conn != nullptr) << " Test DB connection not initialized!";
    std::cout << "[DB FIXTURE] Cleaning up data before test...\n";

    PGresult* clear = PQexec(conn, "TRUNCATE TABLE tasks RESTART IDENTITY;");
    if (PQresultStatus(clear) == PGRES_COMMAND_OK)
      std::cout << "[DB FIXTURE] tasks table truncated.\n";
    else
      std::cout << "[DB FIXTURE]️ Failed to truncate tasks: " << PQerrorMessage(conn) << "\n";
    PQclear(clear);
  }

  // ============================================================
  // 📦 Helper: Apply SQL migration file
  // ============================================================
  static void applyMigration(PGconn* connection, const std::string& path)
  {
    std::ifstream file(path);
    ASSERT_TRUE(file.is_open()) << " Cannot open migration file: " << path;

    std::stringstream sql;
    sql << file.rdbuf();
    PGresult* res = PQexec(connection, sql.str().c_str());

    ASSERT_EQ(PQresultStatus(res), PGRES_COMMAND_OK)
        << " Migration failed: " << PQerrorMessage(connection);
    PQclear(res);
  }
};

} // namespace tasqly::testing::infra

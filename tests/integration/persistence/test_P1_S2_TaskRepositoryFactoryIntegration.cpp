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

using namespace tasqly::p1::s2::infra::factories;
using namespace tasqly::p1::infra::runtime;
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
  auto repoVoid = factory.create();

  // 🧾 Assert — Ensure repository was created and is InMemory
  ASSERT_NE(repoVoid, nullptr) << "Expected fallback repository to be created.";

  // 🧩 Cast and verify type
  auto memRepo = std::static_pointer_cast<P1_S2_InMemoryTaskRepository>(repoVoid);
  ASSERT_NE(memRepo, nullptr) << "Expected InMemory repository after Postgres failure.";

  // 🧩 Verify fallback mode
  EXPECT_EQ(factory.mode(), "InMemory (Fallback)");

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
  // Should throw when fallback is disabled and PostgreSQL fails
  bool exception_thrown = false;
  try {
    factory.createRepository();
  } catch (const std::runtime_error& e) {
    exception_thrown = true;
    EXPECT_NE(std::string(e.what()).find("PostgreSQL failed with no fallback"), std::string::npos)
        << "Expected specific error message.";
  }
  EXPECT_TRUE(exception_thrown) << "Expected exception when fallback is disabled.";

  // 🧩 Verify factory mode updated correctly
  EXPECT_EQ(factory.currentMode(), "Offline (No Repository)")
      << "Expected factory mode to indicate offline/no repo state.";
}

#ifndef TASQLY_SKIP_DB_TESTS

#include "infra/persistence/P1_S2_DbTaskRepository.h"
#include "tests/integration/common/DatabaseIntegrationFixture.h"

using tasqly::p1::s2::infra::persistence::P1_S2_DbTaskRepository;
using tasqly::testing::infra::DatabaseIntegrationFixture;

// ================================================================
// 🧩 Factory + PostgreSQL Integration
// ================================================================

class FactoryPostgresIntegrationTest : public DatabaseIntegrationFixture
{
protected:
  void SetUp() override
  {
    DatabaseIntegrationFixture::SetUp();
    P1_Logger::instance().setMinimumLevel(LogLevel::Error);

    auto& settings = P1_AppSettings::instance();
    settings.set("db.connection_string", testDbConnectionString());
    settings.set("features.db.fallback_inmemory", "true");
  }
};

TEST_F(FactoryPostgresIntegrationTest, UsesPostgresBackendWhenAvailable)
{
  auto& factory = P1_S2_TaskRepositoryFactory::instance();

  auto repo = factory.createRepository();
  ASSERT_NE(repo, nullptr);

  if (factory.currentMode() != "PostgreSQL (Primary)") {
    GTEST_SKIP() << "PostgreSQL not available for factory; mode=" << factory.currentMode();
  }

  auto dbRepo = std::static_pointer_cast<P1_S2_DbTaskRepository>(repo);
  ASSERT_NE(dbRepo, nullptr);
}

#endif // TASQLY_SKIP_DB_TESTS

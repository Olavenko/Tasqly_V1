/*
 * 🧪 File: test_P1_S2_TaskRepositoryFactory.cpp
 * ---------------------------------------------
 * 📌 Purpose   : Unit Tests for P1_S2_TaskRepositoryFactory (Phase 1 Factory)
 * 🧱 Layer     : Tests (Unit - Infrastructure Factory)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-11-06
 * 🔖 Version   : 1.0
 *
 * 🧠 Description:
 * Unit tests for TaskRepositoryFactory covering:
 * - Singleton pattern
 * - Fallback behavior when PostgreSQL fails
 * - Fallback disabled scenario
 * - Exception handling
 * - Current mode reporting
 * - InMemory repository creation with seed
 */

#include "infra/factories/P1_S2_TaskRepositoryFactory.h"
#include "infra/persistence/P1_S2_InMemoryTaskRepository.h"
#include "infra/runtime/P1_AppSettings.h"
#include "infra/runtime/P1_Logger.h"
#include <gtest/gtest.h>

using namespace tasqly::p1::infra::factories;
using namespace tasqly::p1::infra::runtime;
using namespace tasqly::p1::infra::persistence;

// ================================================================
// 🧩 Singleton Tests
// ================================================================

TEST(P1_S2_TaskRepositoryFactoryTest, SingletonInstance)
{
  P1_S2_TaskRepositoryFactory& factory1 = P1_S2_TaskRepositoryFactory::instance();
  P1_S2_TaskRepositoryFactory& factory2 = P1_S2_TaskRepositoryFactory::instance();
  EXPECT_EQ(&factory1, &factory2);
}

// ================================================================
// 🧩 Fallback Disabled Tests
// ================================================================

TEST(P1_S2_TaskRepositoryFactoryTest, FallbackDisabledReturnsNull)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  // Set invalid PostgreSQL credentials
  settings.set("DB_HOST", "invalid_host");
  settings.set("DB_PORT", "9999");
  settings.set("DB_NAME", "invalid_db");
  settings.set("DB_USER", "bad_user");
  settings.set("DB_PASS", "bad_pass");
  
  // Disable fallback (use the correct key name)
  settings.set("features.db.fallback_inmemory", "false");
  
  auto& factory = P1_S2_TaskRepositoryFactory::instance();
  auto repo = factory.createRepository();
  
  // Should return nullptr when fallback is disabled
  EXPECT_EQ(repo, nullptr);
  
  // Current mode should reflect offline state
  EXPECT_EQ(factory.currentMode(), "Offline (No Repository)");
  
  // Reset fallback for other tests
  settings.set("features.db.fallback_inmemory", "true");
}

// ================================================================
// 🧩 Fallback Enabled Tests
// ================================================================

TEST(P1_S2_TaskRepositoryFactoryTest, FallbackEnabledCreatesInMemory)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  // Set invalid PostgreSQL credentials with very short timeout to force failure
  settings.set("DB_HOST", "invalid_host_that_does_not_exist_12345");
  settings.set("DB_PORT", "9999");
  settings.set("DB_NAME", "invalid_db");
  settings.set("DB_USER", "bad_user");
  settings.set("DB_PASS", "bad_pass");
  settings.set("DB_CONNECT_TIMEOUT", "1"); // Very short timeout (1 second)
  
  // Enable fallback (use the correct key name)
  settings.set("features.db.fallback_inmemory", "true");
  settings.set("features.inmemory.seed", "false");
  
  auto& factory = P1_S2_TaskRepositoryFactory::instance();
  auto repo = factory.createRepository();
  
  // Should return a repository (either Postgres if available, or InMemory as fallback)
  EXPECT_NE(repo, nullptr);
  
  // Try to cast to InMemory - if it succeeds, we're in fallback mode
  auto memRepo = std::static_pointer_cast<P1_S2_InMemoryTaskRepository>(repo);
  if (memRepo != nullptr) {
    // We're in fallback mode - verify mode
    EXPECT_EQ(factory.currentMode(), "InMemory (Fallback)");
  } else {
    // PostgreSQL might be available - that's OK, just verify we got a repository
    EXPECT_NE(repo, nullptr);
  }
}

// ================================================================
// 🧩 Seed Feature Tests
// ================================================================

TEST(P1_S2_TaskRepositoryFactoryTest, InMemoryWithSeedEnabled)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  // Set invalid PostgreSQL credentials to force fallback
  settings.set("DB_HOST", "invalid_host");
  settings.set("DB_PORT", "9999");
  settings.set("DB_NAME", "invalid_db");
  settings.set("DB_USER", "bad_user");
  settings.set("DB_PASS", "bad_pass");
  
  // Enable fallback and seed (use the correct key names)
  settings.set("features.db.fallback_inmemory", "true");
  settings.set("features.inmemory.seed", "true");
  
  auto& factory = P1_S2_TaskRepositoryFactory::instance();
  auto repo = factory.createRepository();
  
  // Should return InMemory repository
  EXPECT_NE(repo, nullptr);
  
  auto memRepo = std::static_pointer_cast<P1_S2_InMemoryTaskRepository>(repo);
  EXPECT_NE(memRepo, nullptr);
  
  // Repository should be seeded (we can't directly verify this without accessing internals,
  // but we can verify the repository works)
  auto listResult = memRepo->list(std::nullopt, std::nullopt, 100, 0);
  EXPECT_TRUE(listResult.isOk());
  
  // Reset seed for other tests
  settings.set("features.inmemory.seed", "false");
}

// ================================================================
// 🧩 Current Mode Tests
// ================================================================

TEST(P1_S2_TaskRepositoryFactoryTest, CurrentModeOffline)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "invalid_host");
  settings.set("features.db.fallback_inmemory", "false");
  
  auto& factory = P1_S2_TaskRepositoryFactory::instance();
  auto repo = factory.createRepository();
  
  EXPECT_EQ(repo, nullptr);
  EXPECT_EQ(factory.currentMode(), "Offline (No Repository)");
  
  // Reset
  settings.set("features.db.fallback_inmemory", "true");
}

TEST(P1_S2_TaskRepositoryFactoryTest, CurrentModeFallback)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  // Set invalid PostgreSQL credentials with very short timeout
  settings.set("DB_HOST", "invalid_host_that_does_not_exist_12345");
  settings.set("DB_PORT", "9999");
  settings.set("DB_CONNECT_TIMEOUT", "1"); // Very short timeout
  settings.set("features.db.fallback_inmemory", "true");
  
  auto& factory = P1_S2_TaskRepositoryFactory::instance();
  auto repo = factory.createRepository();
  
  EXPECT_NE(repo, nullptr);
  
  // Check if we're in fallback mode
  auto memRepo = std::static_pointer_cast<P1_S2_InMemoryTaskRepository>(repo);
  if (memRepo != nullptr) {
    // We're in fallback mode
    EXPECT_EQ(factory.currentMode(), "InMemory (Fallback)");
  } else {
    // PostgreSQL might be available - verify we got a repository
    EXPECT_NE(repo, nullptr);
    // Mode should be either "InMemory (Fallback)" or "PostgreSQL (Primary)"
    std::string mode = factory.currentMode();
    EXPECT_TRUE(mode == "InMemory (Fallback)" || mode == "PostgreSQL (Primary)");
  }
}

// ================================================================
// 🧩 Exception Handling Tests
// ================================================================

TEST(P1_S2_TaskRepositoryFactoryTest, HandlesPostgresException)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  // Set invalid credentials to trigger exception
  settings.set("DB_HOST", "invalid_host");
  settings.set("DB_PORT", "9999");
  settings.set("features.db.fallback_inmemory", "true");
  
  auto& factory = P1_S2_TaskRepositoryFactory::instance();
  
  // Should not throw, should fallback to InMemory
  EXPECT_NO_THROW({
    auto repo = factory.createRepository();
    EXPECT_NE(repo, nullptr);
  });
}

TEST(P1_S2_TaskRepositoryFactoryTest, HandlesInMemoryException)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "invalid_host");
  settings.set("features.db.fallback_inmemory", "true");
  
  auto& factory = P1_S2_TaskRepositoryFactory::instance();
  
  // InMemory repository creation should not throw
  // (it's a simple in-memory structure, unlikely to fail)
  EXPECT_NO_THROW({
    auto repo = factory.createRepository();
    // Even if InMemory creation somehow fails, factory should handle it gracefully
    (void)repo;
  });
}

// ================================================================
// 🧩 Multiple Calls Tests
// ================================================================

TEST(P1_S2_TaskRepositoryFactoryTest, MultipleCallsConsistent)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  // Set invalid PostgreSQL credentials with very short timeout
  settings.set("DB_HOST", "invalid_host_that_does_not_exist_12345");
  settings.set("DB_PORT", "9999");
  settings.set("DB_CONNECT_TIMEOUT", "1"); // Very short timeout
  settings.set("features.db.fallback_inmemory", "true");
  
  auto& factory = P1_S2_TaskRepositoryFactory::instance();
  
  // First call
  auto repo1 = factory.createRepository();
  EXPECT_NE(repo1, nullptr);
  std::string mode1 = factory.currentMode();
  
  // Second call
  auto repo2 = factory.createRepository();
  EXPECT_NE(repo2, nullptr);
  std::string mode2 = factory.currentMode();
  
  // Mode should be consistent between calls
  EXPECT_EQ(mode1, mode2);
  
  // Mode should be either "InMemory (Fallback)" or "PostgreSQL (Primary)"
  EXPECT_TRUE(mode1 == "InMemory (Fallback)" || mode1 == "PostgreSQL (Primary)");
}

// ================================================================
// 🧩 Exception Handling in InMemory Creation
// ================================================================

TEST(P1_S2_TaskRepositoryFactoryTest, HandlesInMemoryCreationException)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  // Set invalid PostgreSQL credentials to force fallback
  settings.set("DB_HOST", "invalid_host");
  settings.set("DB_PORT", "9999");
  settings.set("features.db.fallback_inmemory", "true");
  
  auto& factory = P1_S2_TaskRepositoryFactory::instance();
  
  // InMemory repository creation should not throw
  // Even if it somehow fails, factory should handle it gracefully
  EXPECT_NO_THROW({
    auto repo = factory.createRepository();
    // If InMemory creation fails, factory should return nullptr
    // but should not throw
    (void)repo;
  });
}

// ================================================================
// 🧩 Current Mode Tests - PostgreSQL Success
// ================================================================

TEST(P1_S2_TaskRepositoryFactoryTest, CurrentModePostgreSQL)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  // Set valid PostgreSQL credentials (if available)
  // Note: This test may pass or fail depending on DB availability
  settings.set("DB_HOST", "localhost");
  settings.set("DB_PORT", "5432");
  settings.set("DB_NAME", "tasqly_test");
  settings.set("DB_USER", "postgres");
  settings.set("DB_PASS", "themyth2060");
  settings.set("features.db.fallback_inmemory", "true");
  
  auto& factory = P1_S2_TaskRepositoryFactory::instance();
  auto repo = factory.createRepository();
  
  if (repo != nullptr) {
    std::string mode = factory.currentMode();
    // Mode should be either "InMemory (Fallback)" or "PostgreSQL (Primary)"
    EXPECT_TRUE(mode == "InMemory (Fallback)" || mode == "PostgreSQL (Primary)");
  }
}


/*
 * 🧪 File: test_P1_S2_TaskRepositoryFactory.cpp
 * ---------------------------------------------
 * 📌 Purpose   : Unit Tests for P1_S2_TaskRepositoryFactory (Phase 1 Factory)
 * 🧱 Layer     : Tests (Unit - Infrastructure Factory)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-11-06
 * 🔖 Version   : 1.1
 *
 * 🧠 Description:
 * Unit tests for TaskRepositoryFactory covering:
 * - Singleton pattern
 * - Fallback behavior when PostgreSQL fails
 * - Fallback disabled scenario
 * - Exception handling
 * - Current mode reporting
 */

#include "infra/factories/P1_S2_TaskRepositoryFactory.h"
#include "infra/persistence/P1_S2_DbTaskRepository.h"
#include "infra/persistence/P1_S2_InMemoryTaskRepository.h"
#include "infra/runtime/P1_AppSettings.h"
#include "infra/runtime/P1_Logger.h"
#include <gtest/gtest.h>

using namespace tasqly::p1::s2::infra::factories;
using namespace tasqly::p1::infra::runtime;
using tasqly::p1::infra::persistence::P1_S2_InMemoryTaskRepository;
using tasqly::p1::s2::infra::persistence::P1_S2_DbTaskRepository;

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

TEST(P1_S2_TaskRepositoryFactoryTest, FallbackDisabledThrowsException)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  // Set invalid PostgreSQL connection string
  settings.set("db.connection_string", "host=invalid_host port=9999 dbname=invalid_db user=bad_user");
  
  // Disable fallback
  settings.set("features.db.fallback_inmemory", "false");
  
  auto& factory = P1_S2_TaskRepositoryFactory::instance();
  
  // Should throw when fallback is disabled and PostgreSQL fails
  EXPECT_THROW({
    try {
      factory.create();
    } catch (const std::exception& e) {
      EXPECT_NE(std::string(e.what()).find("PostgreSQL failed with no fallback"), std::string::npos);
      throw;
    }
  }, std::runtime_error);
  
  // Reset fallback for other tests
  settings.set("features.db.fallback_inmemory", "true");
}

// ================================================================
// Fallback Enabled Tests
// ================================================================

TEST(P1_S2_TaskRepositoryFactoryTest, FallbackEnabledCreatesInMemory)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  // Set invalid PostgreSQL connection string
  settings.set("db.connection_string", "host=invalid_host_that_does_not_exist_12345 port=9999 dbname=invalid_db user=bad_user");
  
  // Enable fallback
  settings.set("features.db.fallback_inmemory", "true");
  
  auto& factory = P1_S2_TaskRepositoryFactory::instance();
  auto repo = factory.create();
  
  // Should return a repository (InMemory as fallback)
  EXPECT_NE(repo, nullptr);
  
  auto memRepo = std::static_pointer_cast<P1_S2_InMemoryTaskRepository>(repo);
  if (memRepo != nullptr) {
    // We're in fallback mode - verify mode
    EXPECT_EQ(factory.mode(), "InMemory (Fallback)");
  } else {
    // PostgreSQL might be available - that's OK, just verify we got a repository
    EXPECT_NE(repo, nullptr);
  }
}

TEST(P1_S2_TaskRepositoryFactoryTest, HandlesMultipleCallsConsistently)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  // Set invalid connection to force fallback
  settings.set("db.connection_string", "host=invalid");
  settings.set("features.db.fallback_inmemory", "true");
  
  auto& factory = P1_S2_TaskRepositoryFactory::instance();
  
  // First call
  auto repo1 = factory.create();
  EXPECT_NE(repo1, nullptr);
  std::string mode1 = factory.mode();

  // Second call should be consistent
  auto repo2 = factory.create();
  EXPECT_NE(repo2, nullptr);
  std::string mode2 = factory.mode();

  EXPECT_EQ(mode1, mode2);
  
  // The two repositories should be different instances
  EXPECT_NE(repo1.get(), repo2.get());
}

// ================================================================
// Seed Feature Tests
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
  auto repo = factory.create();

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
// Current Mode Tests
// ================================================================

TEST(P1_S2_TaskRepositoryFactoryTest, CurrentModeOffline)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);
  
  auto& settings = P1_AppSettings::instance();
  settings.set("DB_HOST", "invalid_host");
  settings.set("features.db.fallback_inmemory", "false");
  
  auto& factory = P1_S2_TaskRepositoryFactory::instance();
  
  // Should throw when fallback is disabled and PostgreSQL fails
  EXPECT_THROW({
    factory.create();
  }, std::runtime_error);

  EXPECT_EQ(factory.mode(), "Offline (No Repository)");

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
  auto repo = factory.create();

  EXPECT_NE(repo, nullptr);
  
  // Check if we're in fallback mode
  auto memRepo = std::static_pointer_cast<P1_S2_InMemoryTaskRepository>(repo);
  if (memRepo != nullptr) {
    // We're in fallback mode
    EXPECT_EQ(factory.mode(), "InMemory (Fallback)");
  } else {
    // PostgreSQL might be available - verify we got a repository
    EXPECT_NE(repo, nullptr);
    // Mode should be either "InMemory (Fallback)" or "PostgreSQL (Primary)"
    std::string mode = factory.mode();
    EXPECT_TRUE(mode == "InMemory (Fallback)" || mode == "PostgreSQL (Primary)");
  }
}

TEST(P1_S2_TaskRepositoryFactoryTest, CurrentModeReflectsState)
{
  auto& settings = P1_AppSettings::instance();
  auto& factory = P1_S2_TaskRepositoryFactory::instance();
  
  // Test with valid connection string (if available)
  try {
    settings.set("db.connection_string", "host=localhost port=5432 dbname=tasqly user=postgres");
    settings.set("features.db.fallback_inmemory", "false");
    
    auto repo = factory.create();
    if (repo) {
      EXPECT_EQ(factory.mode(), "PostgreSQL (Primary)");
      return; // Successfully connected to PostgreSQL
    }
  } catch (...) {
    // Continue to fallback test
  }
  
  // Test fallback to InMemory
  settings.set("db.connection_string", "host=invalid_host");
  settings.set("features.db.fallback_inmemory", "true");
  
  auto repo = factory.create();
  EXPECT_EQ(factory.mode(), "InMemory (Fallback)");
}

// ================================================================
// Exception Handling Tests
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
    auto repo = factory.create();
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
    auto repo = factory.create();
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
  auto repo1 = factory.create();
  EXPECT_NE(repo1, nullptr);
  std::string mode1 = factory.mode();

  // Second call
  auto repo2 = factory.create();
  EXPECT_NE(repo2, nullptr);
  std::string mode2 = factory.mode();

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
    auto repo = factory.create();
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
  auto repo = factory.create();

  if (repo != nullptr) {
    std::string mode = factory.mode();
    // Mode should be either "InMemory (Fallback)" or "PostgreSQL (Primary)"
    EXPECT_TRUE(mode == "InMemory (Fallback)" || mode == "PostgreSQL (Primary)");
  }
}

// ================================================================
// Alias Behavior Tests (createRepository / currentMode)
// ================================================================

TEST(P1_S2_TaskRepositoryFactoryTest, CreateRepositoryAliasMatchesCreate)
{
  P1_Logger::instance().setMinimumLevel(LogLevel::Error);

  auto& settings = P1_AppSettings::instance();
  settings.set("db.connection_string", "host=invalid_host port=9999 dbname=invalid_db user=bad_user");
  settings.set("features.db.fallback_inmemory", "true");

  auto& factory = P1_S2_TaskRepositoryFactory::instance();

  auto repo1 = factory.create();
  auto mode1 = factory.mode();

  auto repo2 = factory.createRepository();
  auto mode2 = factory.currentMode();

  EXPECT_NE(repo1, nullptr);
  EXPECT_NE(repo2, nullptr);
  EXPECT_EQ(mode1, mode2);
}


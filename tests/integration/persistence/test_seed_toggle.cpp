/*
 * 🧱 File: test_seed_toggle.cpp
 * -----------------------------
 * 📌 Purpose   : Integration tests for seeding behavior in InMemory repositories.
 * 🧱 Layer     : Tests (Integration - Persistence)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-08
 * 🗓️ Updated   : 2025-09-23
 *
 * 🧠 Description:
 * Verifies that InMemoryTaskRepository and InMemoryGoalRepository
 * honor the `seed` flag correctly:
 * - seed=false → repository is empty
 * - seed=true → repository is pre-populated
 *
 * 📌 Note:
 * These are integration-level checks of repository initialization only.
 */

#include "infra/persistence/InMemoryGoalRepository.h"
#include "infra/persistence/InMemoryTaskRepository.h"
#include <gtest/gtest.h>

using namespace tasqly::domain::core;

// 📝 Test 1: Task repo without seed is empty
TEST(SeedToggleTests, TaskRepositoryWithoutSeedIsEmpty)
{
  InMemoryTaskRepository repo(false);
  auto listResult = repo.list(10, 0).get();
  ASSERT_TRUE(listResult.isOk());
  EXPECT_TRUE(listResult.value().empty());
}

// 📝 Test 2: Task repo with seed has data
TEST(SeedToggleTests, TaskRepositoryWithSeedHasData)
{
  InMemoryTaskRepository repo(true);
  auto listResult = repo.list(10, 0).get();
  ASSERT_TRUE(listResult.isOk());
  EXPECT_FALSE(listResult.value().empty());
}

// 📝 Test 3: Goal repo without seed is empty
TEST(SeedToggleTests, GoalRepositoryWithoutSeedIsEmpty)
{
  InMemoryGoalRepository repo(false);
  auto listResult = repo.list(10, 0).get();
  ASSERT_TRUE(listResult.isOk());
  EXPECT_TRUE(listResult.value().empty());
}

// 📝 Test 4: Goal repo with seed has data
TEST(SeedToggleTests, GoalRepositoryWithSeedHasData)
{
  InMemoryGoalRepository repo(true);
  auto listResult = repo.list(10, 0).get();
  ASSERT_TRUE(listResult.isOk());
  EXPECT_FALSE(listResult.value().empty());
}

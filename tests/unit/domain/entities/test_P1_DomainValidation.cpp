/*
 * 🧱 File: test_P1_DomainValidation.cpp
 * ---------------------------------
 * 📌 Purpose   : Unit tests for DomainValidation helpers (title & deadline).
 * 🧱 Layer     : Tests (Unit / Domain)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-12
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Verifies core validation logic in DomainValidation:
 *  - Title validation (non-empty, ≤200 chars)
 *  - Deadline validation (≥ now or not set)
 * These tests ensure domain consistency rules hold before any persistence.
 *
 * 🔗 Depends On:
 *   - domain/core/entities/P1_DomainValidation.h
 * ✅ Tested In : TasqlyTestsRunner
 * ⚡ Performance: Negligible (pure CPU checks)
 */

#include "domain/core/entities/P1_DomainValidation.h"
#include <gtest/gtest.h>

using namespace tasqly::p1::s1::domain::core;

// 🧩 Fixture for DomainValidation tests
class DomainValidationTest : public ::testing::Test
{
protected:
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
};

// 🧪 Case 1: Empty title should fail
TEST_F(DomainValidationTest, EmptyTitle_ShouldReturnInvalid)
{
  auto result = DomainValidation::validateTitle("");
  EXPECT_FALSE(result.valid);
  EXPECT_EQ(result.message, "Title cannot be empty");
}

// 🧪 Case 2: Overly long title (>200 chars) should fail
TEST_F(DomainValidationTest, LongTitle_ShouldReturnInvalid)
{
  std::string longTitle(250, 'A');
  auto result = DomainValidation::validateTitle(longTitle);
  EXPECT_FALSE(result.valid);
  EXPECT_EQ(result.message, "Title exceeds 200 characters");
}

// 🧪 Case 3: Valid title should pass
TEST_F(DomainValidationTest, ValidTitle_ShouldReturnValid)
{
  std::string validTitle = "Write unit tests";
  auto result = DomainValidation::validateTitle(validTitle);
  EXPECT_TRUE(result.valid);
  EXPECT_TRUE(result.message.empty());
}

// 🧪 Case 4: Null deadline should be valid
TEST_F(DomainValidationTest, NullDeadline_ShouldReturnValid)
{
  auto result = DomainValidation::validateDeadline(std::nullopt);
  EXPECT_TRUE(result.valid);
  EXPECT_TRUE(result.message.empty());
}

// 🧪 Case 5: Past deadline should fail
TEST_F(DomainValidationTest, PastDeadline_ShouldReturnInvalid)
{
  auto past = now - std::chrono::hours(5);
  auto result = DomainValidation::validateDeadline(past);
  EXPECT_FALSE(result.valid);
  EXPECT_EQ(result.message, "Deadline cannot be in the past");
}

// 🧪 Case 6: Future deadline should pass
TEST_F(DomainValidationTest, FutureDeadline_ShouldReturnValid)
{
  auto future = now + std::chrono::hours(2);
  auto result = DomainValidation::validateDeadline(future);
  EXPECT_TRUE(result.valid);
  EXPECT_TRUE(result.message.empty());
}

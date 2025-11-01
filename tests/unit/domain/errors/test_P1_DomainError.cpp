/*
 * 🧱 File: test_P1_DomainError.cpp
 * -----------------------------
 * 📌 Purpose   : Unit tests for DomainError struct and factory helpers.
 * 🧱 Layer     : Domain (Core)
 * 🧪 Type      : Unit Test (GoogleTest)
 * 👤 Author    : Tasqly QA Bot
 * 🗓️ Created   : 2025-10-12
 * 🔖 Version   : 1.1 (Aligned with v1 API)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Verifies the correctness of DomainError initialization, factory helpers,
 * and equality comparison operators.
 */

#include "domain/core/errors/P1_DomainError.h"
#include <gtest/gtest.h>

using namespace tasqly::p1::s1::domain::core;

// 🧩 Test: Default constructor initializes with Unknown code
TEST(DomainErrorTest, DefaultConstructorSetsUnknown)
{
  DomainError err;
  EXPECT_EQ(err.code, DomainErrorCode::Unknown);
  EXPECT_TRUE(err.message.empty());
  EXPECT_TRUE(err.context.empty());
}

// 🧩 Test: Parameterized constructor sets fields correctly
TEST(DomainErrorTest, ParameterizedConstructorSetsValues)
{
  DomainError err(DomainErrorCode::Validation, "Invalid title");
  EXPECT_EQ(err.code, DomainErrorCode::Validation);
  EXPECT_EQ(err.message, "Invalid title");
  EXPECT_TRUE(err.context.empty());
}

// 🧩 Test: Context constructor stores key-value data
TEST(DomainErrorTest, ContextConstructorStoresContext)
{
  std::unordered_map<std::string, std::string> ctx{{"field", "title"}};
  DomainError err(DomainErrorCode::Conflict, "Duplicate task", ctx);
  EXPECT_EQ(err.code, DomainErrorCode::Conflict);
  EXPECT_EQ(err.message, "Duplicate task");
  EXPECT_EQ(err.context.at("field"), "title");
}

// 🧩 Test: Factory helpers create correct codes
TEST(DomainErrorTest, FactoryHelpersCreateCorrectCodes)
{
  auto v = DomainError::makeValidation("Missing title");
  auto nf = DomainError::makeNotFound("Task not found");
  auto c = DomainError::makeConflict("Duplicate");
  auto s = DomainError::makeStorage("IO error");
  auto u = DomainError::makeUnknown();

  EXPECT_EQ(v.code, DomainErrorCode::Validation);
  EXPECT_EQ(nf.code, DomainErrorCode::NotFound);
  EXPECT_EQ(c.code, DomainErrorCode::Conflict);
  EXPECT_EQ(s.code, DomainErrorCode::Storage);
  EXPECT_EQ(u.code, DomainErrorCode::Unknown);
}

// 🧩 Test: Equality and inequality operators work as expected
TEST(DomainErrorTest, EqualityOperators)
{
  DomainError a(DomainErrorCode::Validation, "Invalid");
  DomainError b(DomainErrorCode::Validation, "Invalid");
  DomainError c(DomainErrorCode::Conflict, "Conflict");

  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
  EXPECT_TRUE(a != c);
}

/*
 * 🧱 File: test_DomainResult.cpp
 * ------------------------------
 * 📌 Purpose   : Unit tests for DomainResult<T> and DomainResult<void>.
 * 🧱 Layer     : Domain (Core)
 * 🧪 Type      : Unit Test (GoogleTest)
 * 👤 Author    : Tasqly QA Bot
 * 🗓️ Created   : 2025-10-12
 * 🔖 Version   : 1.0
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Tests correct behavior of DomainResult<T> success/error handling,
 * specialization for void, and all factory and accessor methods.
 */

#include "domain/core/errors/P1_DomainResult.h"
#include <gtest/gtest.h>

using namespace tasqly::domain::core::v1;

// 🧩 Test: ok() factory creates success result
TEST(DomainResultTest, OkFactoryCreatesSuccess)
{
  auto result = DomainResult<int>::ok(42);
  EXPECT_TRUE(result.isOk());
  EXPECT_FALSE(result.isErr());
  EXPECT_TRUE(static_cast<bool>(result));
  EXPECT_EQ(result.value(), 42);
}

// 🧩 Test: err() factory creates error result
TEST(DomainResultTest, ErrFactoryCreatesError)
{
  auto err = DomainError::makeValidation("Invalid input");
  auto result = DomainResult<int>::err(err);

  EXPECT_TRUE(result.isErr());
  EXPECT_FALSE(result.isOk());
  EXPECT_FALSE(static_cast<bool>(result));

  EXPECT_EQ(result.error().code, DomainErrorCode::Validation);
  EXPECT_EQ(result.error().message, "Invalid input");
}

// 🧩 Test: value() accessor throws assertion when invalid (Debug builds)
TEST(DomainResultTest, AccessingValueOnErrorIsUnsafe)
{
#ifndef NDEBUG
  auto err = DomainError::makeNotFound("Missing ID");
  auto result = DomainResult<int>::err(err);
  EXPECT_DEATH((void) result.value(), ""); // death test for assert
#endif
}

// 🧩 Test: error() accessor throws assertion when invalid (Debug builds)
TEST(DomainResultTest, AccessingErrorOnOkIsUnsafe)
{
#ifndef NDEBUG
  auto result = DomainResult<int>::ok(77);
  EXPECT_DEATH((void) result.error(), ""); // death test for assert
#endif
}

// 🧩 Test: DomainResult<void> ok() specialization works
TEST(DomainResultVoidTest, OkSpecializationWorks)
{
  auto result = DomainResult<void>::ok();
  EXPECT_TRUE(result.isOk());
  EXPECT_FALSE(result.isErr());
  EXPECT_TRUE(static_cast<bool>(result));
}

// 🧩 Test: DomainResult<void> err() specialization works
TEST(DomainResultVoidTest, ErrSpecializationWorks)
{
  auto err = DomainError::makeStorage("Disk failure");
  auto result = DomainResult<void>::err(err);

  EXPECT_TRUE(result.isErr());
  EXPECT_FALSE(result.isOk());
  EXPECT_FALSE(static_cast<bool>(result));

  EXPECT_EQ(result.error().code, DomainErrorCode::Storage);
  EXPECT_EQ(result.error().message, "Disk failure");
}

// 🧩 Test: Accessing error on void::ok() should assert
TEST(DomainResultVoidTest, AccessingErrorOnOkIsUnsafe)
{
#ifndef NDEBUG
  auto result = DomainResult<void>::ok();
  EXPECT_DEATH((void) result.error(), "");
#endif
}

/*
 * 🧪 File: test_P1_S2_Result.cpp
 * --------------------------------
 * 📌 Purpose   : Unit Tests for tasqly::p1::s2::infra::runtime::Result<T>
 * 🧱 Layer     : Tests (Unit - Infrastructure Runtime)
 * 🗓️ Created   : 2025-11-17
 *
 * 🧠 Description:
 * Verifies the core semantics of the lightweight Phase1 Slice2 Result<T> type:
 * - Construction helpers (Ok / Err)
 * - State queries (isOk / isErr)
 * - Accessors (value() / error()) for const and non-const objects
 * - Copy and move semantics for both success and error states
 */

#include <gtest/gtest.h>

#include "infra/runtime/P1_Error.h"
#include "infra/runtime/P1_S2_Result.h"

using tasqly::p1::infra::runtime::P1_Error;
using tasqly::p1::infra::runtime::RuntimeErrorCode;
using tasqly::p1::s2::infra::runtime::Result;

// -------------------------------------------------------------
// 1) Ok() construction and basic accessors
// -------------------------------------------------------------
TEST(P1_S2_Result, OkConstructionStoresValue)
{
  auto r = Result<int>::Ok(42);

  EXPECT_TRUE(r.isOk());
  EXPECT_FALSE(r.isErr());
  EXPECT_EQ(r.value(), 42);
}

TEST(P1_S2_Result, OkConstructionWithNonTrivialType)
{
  auto r = Result<std::string>::Ok(std::string("hello"));

  EXPECT_TRUE(r.isOk());
  EXPECT_FALSE(r.isErr());
  EXPECT_EQ(r.value(), "hello");

  // Non-const accessor should allow mutation
  r.value() = "world";
  EXPECT_EQ(r.value(), "world");
}

TEST(P1_S2_Result, OkConstAccessUsesConstOverload)
{
  const auto r = Result<int>::Ok(7);

  EXPECT_TRUE(r.isOk());
  EXPECT_FALSE(r.isErr());
  EXPECT_EQ(r.value(), 7);
}

// -------------------------------------------------------------
// 2) Err() construction and error accessors
// -------------------------------------------------------------
TEST(P1_S2_Result, ErrConstructionStoresError)
{
  P1_Error err = P1_Error::makeDbInit("db init failed");

  auto r = Result<int>::Err(err);

  EXPECT_TRUE(r.isErr());
  EXPECT_FALSE(r.isOk());
  EXPECT_EQ(r.error().code, RuntimeErrorCode::DbInit);
  EXPECT_EQ(r.error().message, "db init failed");
}

// -------------------------------------------------------------
// 3) Copy semantics
// -------------------------------------------------------------
TEST(P1_S2_Result, CopyConstructionPreservesOkState)
{
  auto original = Result<int>::Ok(10);
  auto copy = original; // copy-construct

  EXPECT_TRUE(copy.isOk());
  EXPECT_FALSE(copy.isErr());
  EXPECT_EQ(copy.value(), 10);
}

TEST(P1_S2_Result, CopyConstructionPreservesErrState)
{
  P1_Error err = P1_Error::makeTimeout("timeout");
  auto original = Result<int>::Err(err);
  auto copy = original; // copy-construct

  EXPECT_TRUE(copy.isErr());
  EXPECT_FALSE(copy.isOk());
  EXPECT_EQ(copy.error().code, RuntimeErrorCode::Timeout);
  EXPECT_EQ(copy.error().message, "timeout");
}

// -------------------------------------------------------------
// 4) Move semantics
// -------------------------------------------------------------
TEST(P1_S2_Result, MoveConstructionPreservesOkState)
{
  auto makeResult = [] {
    return Result<std::string>::Ok(std::string("payload"));
  };

  auto moved = makeResult();

  EXPECT_TRUE(moved.isOk());
  EXPECT_FALSE(moved.isErr());
  EXPECT_EQ(moved.value(), "payload");
}

TEST(P1_S2_Result, MoveConstructionPreservesErrState)
{
  auto makeErrorResult = [] {
    P1_Error err = P1_Error::makeNetwork("net down");
    return Result<int>::Err(err);
  };

  auto moved = makeErrorResult();

  EXPECT_TRUE(moved.isErr());
  EXPECT_FALSE(moved.isOk());
  EXPECT_EQ(moved.error().code, RuntimeErrorCode::Network);
  EXPECT_EQ(moved.error().message, "net down");
}


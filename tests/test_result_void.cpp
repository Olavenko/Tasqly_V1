/*
 * 🧱 File: test_result_void.cpp
 * ----------------------------
 * 📌 Purpose   : Unit tests for Result<void, Error> helpers.
 * 🧱 Layer     : Tests (Unit)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-03
 *
 * 🧠 Description:
 * Verifies the core functionality of Result<void,Error>:
 * - Construction via ok()/err()
 * - State queries (isOk/isErr)
 * - match() branching
 */

#include "domain/core/Error.h"
#include "domain/core/Result.h"
#include <gtest/gtest.h>

using namespace tasqly::domain::core;

// ✅ Test: Construct Result<void> success
TEST(ResultVoidTests, OkConstruction)
{
  auto r = ok();
  EXPECT_TRUE(r.isOk());
  EXPECT_FALSE(r.isErr());
}

// ✅ Test: Construct Result<void> error
TEST(ResultVoidTests, ErrConstruction)
{
  Error e(42, "boom");
  auto r = err(e);
  EXPECT_TRUE(r.isErr());
  EXPECT_FALSE(r.isOk());
  EXPECT_EQ(r.error().code(), 42);
  EXPECT_EQ(r.error().message(), "boom");
}

// ✅ Test: match branching for void
TEST(ResultVoidTests, MatchOkAndErr)
{
  auto rOk = ok();
  auto resOk = rOk.match([] { return QString("ok"); },
                         [](const Error& e) { return QString("err:%1").arg(e.message()); });
  EXPECT_EQ(resOk, "ok");

  Error e(7, "bad");
  auto rErr = err(e);
  auto resErr = rErr.match([] { return QString("ok"); },
                           [](const Error& e) { return QString("err:%1").arg(e.message()); });
  EXPECT_EQ(resErr, "err:bad");
}

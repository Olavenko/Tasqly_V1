/*
 * 🧱 File: test_result.cpp
 * ------------------------
 * 📌 Purpose   : Unit tests for Result<T, Error> helpers.
 * 🧱 Layer     : Tests (Unit)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-03
 *
 * 🧠 Description:
 * Verifies the core functionality of Result<T,Error>:
 * - Construction (ok/err)
 * - State queries (isOk/isErr)
 * - Accessors (value/error/valueOr)
 * - Transformers (map/mapError/andThen/match)
 */

#include "domain/core/Error.h"
#include "domain/core/Result.h"
#include <gtest/gtest.h>

using namespace tasqly::domain::core;

// ✅ Test: Construct Result<T> success
TEST(ResultTests, OkConstruction)
{
  auto r = Result<int>::ok(42);
  EXPECT_TRUE(r.isOk());
  EXPECT_FALSE(r.isErr());
  EXPECT_EQ(r.value(), 42);
}

// ✅ Test: Construct Result<T> error
TEST(ResultTests, ErrConstruction)
{
  Error e(1001, "Something went wrong");
  auto r = Result<int>::err(e);
  EXPECT_TRUE(r.isErr());
  EXPECT_FALSE(r.isOk());
  EXPECT_EQ(r.error().code(), 1001);
  EXPECT_EQ(r.error().message(), "Something went wrong");
}

// ✅ Test: Empty message becomes "Unknown error"
TEST(ResultTests, EmptyMessageDefaults)
{
  Error e(2001, "");
  EXPECT_EQ(e.message(), "Unknown error");
}

// ✅ Test: valueOr returns fallback when error
TEST(ResultTests, ValueOrFallback)
{
  Error e(3001, "fail");
  auto r = Result<int>::err(e);
  EXPECT_EQ(r.valueOr(99), 99);
}

// ✅ Test: map applies function when ok
TEST(ResultTests, MapOnSuccess)
{
  auto r = Result<int>::ok(5);
  auto r2 = r.map([](int v) { return v * 2; });
  EXPECT_TRUE(r2.isOk());
  EXPECT_EQ(r2.value(), 10);
}

// ✅ Test: mapError applies function when error
TEST(ResultTests, MapErrorOnFailure)
{
  Error e(4001, "bad");
  auto r = Result<int>::err(e);
  auto r2 = r.mapError([](const Error& err) { return err.withContext({{"hint", "recoverable"}}); });
  EXPECT_TRUE(r2.isErr());
  EXPECT_EQ(r2.error().context().value("hint").toString(), "recoverable");
}

// ✅ Test: andThen chaining
TEST(ResultTests, AndThenSuccess)
{
  auto r = Result<int>::ok(2);
  auto r2 = r.andThen([](int v) { return Result<int>::ok(v * 3); });
  EXPECT_TRUE(r2.isOk());
  EXPECT_EQ(r2.value(), 6);
}

TEST(ResultTests, AndThenFailure)
{
  Error e(5001, "fail");
  auto r = Result<int>::err(e);
  auto r2 = r.andThen([](int v) { return Result<int>::ok(v * 3); });
  EXPECT_TRUE(r2.isErr());
  EXPECT_EQ(r2.error().code(), 5001);
}

// ✅ Test: match branching
TEST(ResultTests, MatchOkAndErr)
{
  auto r1 = Result<int>::ok(7);
  auto res1 = r1.match([](int v) { return QString("ok:%1").arg(v); },
                       [](const Error& e) { return QString("err:%1").arg(e.message()); });
  EXPECT_EQ(res1, "ok:7");

  Error e(6001, "boom");
  auto r2 = Result<int>::err(e);
  auto res2 = r2.match([](int v) { return QString("ok:%1").arg(v); },
                       [](const Error& e) { return QString("err:%1").arg(e.message()); });
  EXPECT_EQ(res2, "err:boom");
}

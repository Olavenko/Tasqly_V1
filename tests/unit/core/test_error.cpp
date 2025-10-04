/*
 * 🧱 File: test_error.cpp
 * -----------------------
 * 📌 Purpose   : Unit tests for Error class (core domain).
 * 🧱 Layer     : Tests (Unit - Core)
 * 👤 Author    : Tasqly Team
 * 🗓️ Created   : 2025-09-15
 *
 * 🧠 Description:
 * Covers edge cases and validates equality/comparison operators of the
 * tasqly::domain::core::Error class.
 *
 * 🔗 Depends On:
 *   - src/domain/core/Error.h
 * 🔗 Related   : Result<T, Error>, ErrorReporter, LogManager
 * ✅ Tested In : TasqlyTestsRunner
 * ⚠️ Errors    : None
 * ⚡ Performance: Non-critical
 * 🔒 Security  : N/A
 * 🕒 Last Major Change: 2025-09-15
 */

#include "domain/core/Error.h"

#include <gtest/gtest.h>
#include <QVariantMap>

using namespace tasqly::domain::core;

// 🧪 Test Suite: Error
// --------------------

// ✅ Test 1: Default constructed Error should have code=0, message="Unknown error", empty context
TEST(ErrorTest, DefaultConstructor)
{
  Error err;
  EXPECT_EQ(err.code(), Error::kUnknownCode);
  EXPECT_EQ(err.message(), "Unknown error");
  EXPECT_TRUE(err.context().isEmpty());
}

// ✅ Test 2: Constructing with empty/whitespace message should normalize to "Unknown error"
TEST(ErrorTest, EmptyMessageNormalizes)
{
  Error err1(42, "");
  EXPECT_EQ(err1.message(), "Unknown error");

  Error err2(7, "   ");
  EXPECT_EQ(err2.message(), "Unknown error");
}

// ✅ Test 3: Unknown() factory should produce an Error with code=0, message="Unknown error"
TEST(ErrorTest, UnknownFactory)
{
  Error err = Error::Unknown();
  EXPECT_EQ(err.code(), Error::kUnknownCode);
  EXPECT_EQ(err.message(), "Unknown error");
  EXPECT_TRUE(err.context().isEmpty());
}

// ✅ Test 4: withContext() should add and override keys in context
TEST(ErrorTest, WithContextMergesAndOverrides)
{
  QVariantMap ctx;
  ctx.insert("foo", "bar");
  Error base(100, "Base", ctx);

  QVariantMap extra;
  extra.insert("baz", 123);
  extra.insert("foo", "override");

  Error merged = base.withContext(extra);

  EXPECT_EQ(merged.code(), 100);
  EXPECT_EQ(merged.message(), "Base");
  EXPECT_EQ(merged.context().value("foo").toString(), "override");
  EXPECT_EQ(merged.context().value("baz").toInt(), 123);
}

// ✅ Test 5: Equality operator should return true for identical errors
TEST(ErrorTest, EqualityOperator)
{
  Error e1(1, "Same", {{"k", 1}});
  Error e2(1, "Same", {{"k", 1}});

  EXPECT_TRUE(e1 == e2);
  EXPECT_FALSE(e1 != e2);
}

// ✅ Test 6: Inequality should detect differences in code, message, or context
TEST(ErrorTest, InequalityOperator)
{
  Error base(1, "Msg", {{"x", 1}});

  Error diffCode(2, "Msg", {{"x", 1}});
  Error diffMsg(1, "Other", {{"x", 1}});
  Error diffCtx(1, "Msg", {{"x", 2}});

  EXPECT_TRUE(base != diffCode);
  EXPECT_TRUE(base != diffMsg);
  EXPECT_TRUE(base != diffCtx);
}

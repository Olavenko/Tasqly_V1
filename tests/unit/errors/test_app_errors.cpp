/*
 * 🧱 File: test_app_errors.cpp
 * ----------------------------
 * 📌 Purpose   : Unit tests for AppErrors (error codes + messages + context).
 * 🧱 Layer     : Testing (Unit)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-14
 *
 * 🧠 Description:
 * Verifies that AppErrors factory methods produce correct
 * error codes, messages, and context values.
 *
 * 🔗 Depends On:
 *   - app/errors/AppErrors.h
 *   - domain/core/Error.h
 * 🔗 Related   : ErrorReporter, LogManager
 * ✅ Tested In : TasqlyTestsRunner (GoogleTest)
 * ⚠️ Errors    : None (pure validation of return values)
 * 🕒 Last Major Change: 2025-09-14
 */

#include "app/errors/AppErrors.h"
#include "domain/core/Error.h"

#include <gtest/gtest.h>

using namespace tasqly::app::errors;
using namespace tasqly::domain::core;

// 🧪 Test fixture
class AppErrorsTest : public ::testing::Test
{
protected:
  AppErrors errors; // default instance
};

// ✅ Generic make()
TEST_F(AppErrorsTest, Make_GenericError)
{
  auto err = errors.make(1001, "Custom message");
  EXPECT_EQ(err.code(), 1001);
  EXPECT_EQ(err.message().toStdString(), "Custom message");
  EXPECT_TRUE(err.context().isEmpty());
}

// 🌐 NetworkUnavailable
TEST_F(AppErrorsTest, NetworkUnavailable)
{
  auto err = errors.networkUnavailable();
  EXPECT_EQ(err.code(), 2001);
  EXPECT_EQ(err.message().toStdString(), "Network unavailable");
}

// 🌐 HttpError adds status to context
TEST_F(AppErrorsTest, HttpErrorAddsStatus)
{
  auto err = errors.httpError(404);
  EXPECT_EQ(err.code(), 2002);
  EXPECT_EQ(err.message().toStdString(), "HTTP request failed");
  EXPECT_TRUE(err.context().contains("status"));
  EXPECT_EQ(err.context().value("status").toInt(), 404);
}

// ⏳ Timeout
TEST_F(AppErrorsTest, Timeout)
{
  auto err = errors.timeout();
  EXPECT_EQ(err.code(), 2003);
  EXPECT_EQ(err.message().toStdString(), "Operation timed out");
}

// 🧼 ValidationFailed with field + reason
TEST_F(AppErrorsTest, ValidationFailedIncludesFieldAndReason)
{
  auto err = errors.validationFailed("email", "invalid format");
  EXPECT_EQ(err.code(), 2004);
  EXPECT_EQ(err.message().toStdString(), "Validation failed");
  EXPECT_EQ(err.context().value("field").toString().toStdString(), "email");
  EXPECT_EQ(err.context().value("reason").toString().toStdString(), "invalid format");
}

// 🗄️ DbUnavailable
TEST_F(AppErrorsTest, DbUnavailable)
{
  auto err = errors.dbUnavailable();
  EXPECT_EQ(err.code(), 2005);
  EXPECT_EQ(err.message().toStdString(), "Database unavailable");
}

// ❓ Unknown fallback
TEST_F(AppErrorsTest, UnknownFallback)
{
  auto err = errors.unknown();
  EXPECT_EQ(err.code(), Error::kUnknownCode); // يجب أن يكون 0
  EXPECT_EQ(err.message().toStdString(), "Unknown error");
}

// 🧼 ValidationFailed بدون reason → context ما يحتويش "reason"
TEST_F(AppErrorsTest, ValidationFailedWithoutReason)
{
  auto err = errors.validationFailed("username", "");
  EXPECT_EQ(err.code(), 2004);
  EXPECT_EQ(err.message().toStdString(), "Validation failed");
  EXPECT_EQ(err.context().value("field").toString().toStdString(), "username");
  EXPECT_FALSE(err.context().contains("reason"));
}

// ✅ Generic make() with empty message → Error ctor should normalize
TEST_F(AppErrorsTest, Make_WithEmptyMessageIsNormalized)
{
  auto err = errors.make(1234, "");
  EXPECT_EQ(err.code(), 1234);
  EXPECT_FALSE(err.message().isEmpty()); // should not be empty
}

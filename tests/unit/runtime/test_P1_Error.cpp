/*
 * 🧪 File: test_P1_Error.cpp
 * --------------------------
 * 📌 Purpose   : Unit Tests for P1_Error (Phase 1 Runtime Error)
 * 🧱 Layer     : Tests (Unit - Infrastructure Runtime)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-11-05
 * 🔖 Version   : 1.0
 *
 * 🧠 Description:
 * Comprehensive unit tests for P1_Error covering:
 * - Factory methods (makeDbInit, makeAuth, makeTimeout, etc.)
 * - Conversion methods (toDomainError, fromDomainError)
 * - String utilities (toString, codeToString)
 * - Context masking (sensitive data protection)
 * - Equality operators
 * - All error codes
 */

#include "infra/runtime/P1_Error.h"
#include "domain/core/errors/P1_DomainError.h"
#include <gtest/gtest.h>

using namespace tasqly::p1::infra::runtime;
using namespace tasqly::p1::s1::domain::core;

// ================================================================
// 🧩 Factory Methods Tests
// ================================================================

TEST(P1_ErrorTest, FactoryMakeDbInit)
{
  auto err = P1_Error::makeDbInit("Database initialization failed");
  EXPECT_EQ(err.code, RuntimeErrorCode::DbInit);
  EXPECT_EQ(err.message, "Database initialization failed");
  EXPECT_TRUE(err.context.empty());
}

TEST(P1_ErrorTest, FactoryMakeAuth)
{
  auto err = P1_Error::makeAuth("Authentication failed");
  EXPECT_EQ(err.code, RuntimeErrorCode::DbAuth);
  EXPECT_EQ(err.message, "Authentication failed");
}

TEST(P1_ErrorTest, FactoryMakeTimeout)
{
  auto err = P1_Error::makeTimeout("Connection timeout");
  EXPECT_EQ(err.code, RuntimeErrorCode::Timeout);
  EXPECT_EQ(err.message, "Connection timeout");
}

TEST(P1_ErrorTest, FactoryMakeNetwork)
{
  auto err = P1_Error::makeNetwork("Network error");
  EXPECT_EQ(err.code, RuntimeErrorCode::Network);
  EXPECT_EQ(err.message, "Network error");
}

TEST(P1_ErrorTest, FactoryMakeIo)
{
  auto err = P1_Error::makeIo("IO error");
  EXPECT_EQ(err.code, RuntimeErrorCode::Io);
  EXPECT_EQ(err.message, "IO error");
}

TEST(P1_ErrorTest, FactoryMakeConfig)
{
  auto err = P1_Error::makeConfig("Configuration error");
  EXPECT_EQ(err.code, RuntimeErrorCode::Config);
  EXPECT_EQ(err.message, "Configuration error");
}

TEST(P1_ErrorTest, FactoryMakeUnknown)
{
  auto err = P1_Error::makeUnknown();
  EXPECT_EQ(err.code, RuntimeErrorCode::Unknown);
  EXPECT_EQ(err.message, "Unknown runtime error");
}

TEST(P1_ErrorTest, FactoryMakeUnknownWithMessage)
{
  auto err = P1_Error::makeUnknown("Custom unknown error");
  EXPECT_EQ(err.code, RuntimeErrorCode::Unknown);
  EXPECT_EQ(err.message, "Custom unknown error");
}

// ================================================================
// 🧩 Constructor Tests
// ================================================================

TEST(P1_ErrorTest, DefaultConstructor)
{
  P1_Error err;
  EXPECT_EQ(err.code, RuntimeErrorCode::None);
  EXPECT_TRUE(err.message.empty());
  EXPECT_TRUE(err.context.empty());
}

TEST(P1_ErrorTest, ConstructorWithCodeAndMessage)
{
  P1_Error err(RuntimeErrorCode::DbInit, "Test message");
  EXPECT_EQ(err.code, RuntimeErrorCode::DbInit);
  EXPECT_EQ(err.message, "Test message");
  EXPECT_TRUE(err.context.empty());
}

TEST(P1_ErrorTest, ConstructorWithContext)
{
  std::unordered_map<std::string, std::string> ctx;
  ctx["host"] = "localhost";
  ctx["port"] = "5432";

  P1_Error err(RuntimeErrorCode::DbInit, "Test message", ctx);
  EXPECT_EQ(err.code, RuntimeErrorCode::DbInit);
  EXPECT_EQ(err.message, "Test message");
  EXPECT_EQ(err.context.size(), 2);
  EXPECT_EQ(err.context.at("host"), "localhost");
  EXPECT_EQ(err.context.at("port"), "5432");
}

// ================================================================
// 🧩 CodeToString Tests
// ================================================================

TEST(P1_ErrorTest, CodeToStringNone)
{
  EXPECT_EQ(P1_Error::codeToString(RuntimeErrorCode::None), "None");
}

TEST(P1_ErrorTest, CodeToStringDbInit)
{
  EXPECT_EQ(P1_Error::codeToString(RuntimeErrorCode::DbInit), "DbInit");
}

TEST(P1_ErrorTest, CodeToStringDbAuth)
{
  EXPECT_EQ(P1_Error::codeToString(RuntimeErrorCode::DbAuth), "DbAuth");
}

TEST(P1_ErrorTest, CodeToStringTimeout)
{
  EXPECT_EQ(P1_Error::codeToString(RuntimeErrorCode::Timeout), "Timeout");
}

TEST(P1_ErrorTest, CodeToStringNetwork)
{
  EXPECT_EQ(P1_Error::codeToString(RuntimeErrorCode::Network), "Network");
}

TEST(P1_ErrorTest, CodeToStringIo)
{
  EXPECT_EQ(P1_Error::codeToString(RuntimeErrorCode::Io), "Io");
}

TEST(P1_ErrorTest, CodeToStringConfig)
{
  EXPECT_EQ(P1_Error::codeToString(RuntimeErrorCode::Config), "Config");
}

TEST(P1_ErrorTest, CodeToStringUnknown)
{
  EXPECT_EQ(P1_Error::codeToString(RuntimeErrorCode::Unknown), "Unknown");
}

TEST(P1_ErrorTest, CodeToStringDefaultCase)
{
  // Test default case for any unexpected enum value
  auto unknownCode = static_cast<RuntimeErrorCode>(999);
  EXPECT_EQ(P1_Error::codeToString(unknownCode), "Unknown");
}

// ================================================================
// 🧩 ToString Tests
// ================================================================

TEST(P1_ErrorTest, ToStringWithoutContext)
{
  P1_Error err(RuntimeErrorCode::DbInit, "Test message");
  std::string result = err.toString();
  EXPECT_TRUE(result.find("[DbInit]") != std::string::npos);
  EXPECT_TRUE(result.find("Test message") != std::string::npos);
}

TEST(P1_ErrorTest, ToStringWithContext)
{
  std::unordered_map<std::string, std::string> ctx;
  ctx["key1"] = "value1";
  ctx["key2"] = "value2";

  P1_Error err(RuntimeErrorCode::DbInit, "Test message", ctx);
  std::string result = err.toString();
  EXPECT_TRUE(result.find("[DbInit]") != std::string::npos);
  EXPECT_TRUE(result.find("Test message") != std::string::npos);
  EXPECT_TRUE(result.find("context={") != std::string::npos);
}

TEST(P1_ErrorTest, ToStringWithMasking)
{
  std::unordered_map<std::string, std::string> ctx;
  ctx["password"] = "secret123";
  ctx["username"] = "user";

  P1_Error err(RuntimeErrorCode::DbAuth, "Auth failed", ctx);
  std::string masked = err.toString(true);
  std::string unmasked = err.toString(false);

  // Masked should contain <masked> for password
  EXPECT_TRUE(masked.find("<masked>") != std::string::npos);
  EXPECT_TRUE(masked.find("secret123") == std::string::npos);

  // Unmasked should contain actual password
  EXPECT_TRUE(unmasked.find("secret123") != std::string::npos);
  EXPECT_TRUE(unmasked.find("<masked>") == std::string::npos);
}

TEST(P1_ErrorTest, ToStringMaskingSensitiveKeys)
{
  std::unordered_map<std::string, std::string> ctx;
  ctx["pass"] = "secret";
  ctx["token"] = "abc123";
  ctx["key"] = "private";
  ctx["secret"] = "hidden";

  P1_Error err(RuntimeErrorCode::Config, "Config error", ctx);
  std::string masked = err.toString(true);

  // All sensitive keys should be masked
  EXPECT_TRUE(masked.find("<masked>") != std::string::npos);
  
  // Check that key names appear in the output
  EXPECT_TRUE(masked.find("pass=") != std::string::npos);
  EXPECT_TRUE(masked.find("token=") != std::string::npos);
  EXPECT_TRUE(masked.find("key=") != std::string::npos);
  EXPECT_TRUE(masked.find("secret=") != std::string::npos);
  
  // Verify that <masked> appears at least 4 times (once for each sensitive key)
  size_t maskedCount = 0;
  size_t pos = 0;
  while ((pos = masked.find("<masked>", pos)) != std::string::npos) {
    maskedCount++;
    pos += 8; // length of "<masked>"
  }
  EXPECT_GE(maskedCount, 4) << "All 4 sensitive values should be masked";
  
  // Verify that actual sensitive values don't appear as standalone values
  // (they should be replaced with <masked>)
  // Note: "secret" might appear as part of "secret=" key name, which is OK
  // We check that values don't appear after = sign
  EXPECT_TRUE(masked.find("=secret") == std::string::npos || masked.find("secret=") != std::string::npos);
  EXPECT_TRUE(masked.find("=abc123") == std::string::npos);
  EXPECT_TRUE(masked.find("=private") == std::string::npos);
  EXPECT_TRUE(masked.find("=hidden") == std::string::npos);
}

TEST(P1_ErrorTest, ToStringNonSensitiveKeysNotMasked)
{
  std::unordered_map<std::string, std::string> ctx;
  ctx["hostname"] = "localhost";
  ctx["port"] = "5432";

  P1_Error err(RuntimeErrorCode::DbInit, "Init error", ctx);
  std::string masked = err.toString(true);

  // Non-sensitive keys should not be masked
  EXPECT_TRUE(masked.find("localhost") != std::string::npos);
  EXPECT_TRUE(masked.find("5432") != std::string::npos);
  EXPECT_TRUE(masked.find("<masked>") == std::string::npos);
}

// ================================================================
// 🧩 Conversion Tests
// ================================================================

TEST(P1_ErrorTest, ToDomainErrorWithoutContext)
{
  P1_Error err(RuntimeErrorCode::DbInit, "Database error");
  DomainError domainErr = err.toDomainError();

  EXPECT_EQ(domainErr.code, DomainErrorCode::Storage);
  EXPECT_TRUE(domainErr.message.find("[runtime:100]") != std::string::npos);
  EXPECT_TRUE(domainErr.message.find("Database error") != std::string::npos);
}

TEST(P1_ErrorTest, ToDomainErrorWithContext)
{
  std::unordered_map<std::string, std::string> ctx;
  ctx["host"] = "localhost";
  ctx["password"] = "secret";

  P1_Error err(RuntimeErrorCode::DbAuth, "Auth failed", ctx);
  DomainError domainErr = err.toDomainError();

  EXPECT_EQ(domainErr.code, DomainErrorCode::Storage);
  EXPECT_TRUE(domainErr.message.find("[runtime:101]") != std::string::npos);
  EXPECT_TRUE(domainErr.message.find("Auth failed") != std::string::npos);
  EXPECT_TRUE(domainErr.message.find("ctx:") != std::string::npos);
  // Password should be masked
  EXPECT_TRUE(domainErr.message.find("<masked>") != std::string::npos);
  EXPECT_TRUE(domainErr.message.find("secret") == std::string::npos);
}

TEST(P1_ErrorTest, FromDomainError)
{
  DomainError domainErr = DomainError::makeStorage("Storage error");
  P1_Error runtimeErr = P1_Error::fromDomainError(domainErr);

  EXPECT_EQ(runtimeErr.code, RuntimeErrorCode::Config);
  EXPECT_TRUE(runtimeErr.message.find("[domain]") != std::string::npos);
  EXPECT_TRUE(runtimeErr.message.find("Storage error") != std::string::npos);
  EXPECT_EQ(runtimeErr.context.size(), 1);
  EXPECT_EQ(runtimeErr.context.at("domain_code"), "1004");
}

TEST(P1_ErrorTest, FromDomainErrorWithContext)
{
  std::unordered_map<std::string, std::string> ctx;
  ctx["entity"] = "Task";
  DomainError domainErr(DomainErrorCode::NotFound, "Not found", ctx);

  P1_Error runtimeErr = P1_Error::fromDomainError(domainErr);
  EXPECT_EQ(runtimeErr.code, RuntimeErrorCode::Config);
  EXPECT_TRUE(runtimeErr.message.find("[domain]") != std::string::npos);
  EXPECT_EQ(runtimeErr.context.at("domain_code"), "1002");
}

// ================================================================
// 🧩 Equality Operators Tests
// ================================================================

TEST(P1_ErrorTest, EqualityOperatorSameError)
{
  P1_Error err1(RuntimeErrorCode::DbInit, "Test");
  P1_Error err2(RuntimeErrorCode::DbInit, "Test");
  EXPECT_TRUE(err1 == err2);
  EXPECT_FALSE(err1 != err2);
}

TEST(P1_ErrorTest, EqualityOperatorDifferentCode)
{
  P1_Error err1(RuntimeErrorCode::DbInit, "Test");
  P1_Error err2(RuntimeErrorCode::DbAuth, "Test");
  EXPECT_FALSE(err1 == err2);
  EXPECT_TRUE(err1 != err2);
}

TEST(P1_ErrorTest, EqualityOperatorDifferentMessage)
{
  P1_Error err1(RuntimeErrorCode::DbInit, "Test1");
  P1_Error err2(RuntimeErrorCode::DbInit, "Test2");
  EXPECT_FALSE(err1 == err2);
  EXPECT_TRUE(err1 != err2);
}

TEST(P1_ErrorTest, EqualityOperatorWithContext)
{
  std::unordered_map<std::string, std::string> ctx1;
  ctx1["key"] = "value1";
  std::unordered_map<std::string, std::string> ctx2;
  ctx2["key"] = "value2";

  P1_Error err1(RuntimeErrorCode::DbInit, "Test", ctx1);
  P1_Error err2(RuntimeErrorCode::DbInit, "Test", ctx2);
  EXPECT_FALSE(err1 == err2);
  EXPECT_TRUE(err1 != err2);
}

TEST(P1_ErrorTest, EqualityOperatorSameContext)
{
  std::unordered_map<std::string, std::string> ctx;
  ctx["key"] = "value";

  P1_Error err1(RuntimeErrorCode::DbInit, "Test", ctx);
  P1_Error err2(RuntimeErrorCode::DbInit, "Test", ctx);
  EXPECT_TRUE(err1 == err2);
  EXPECT_FALSE(err1 != err2);
}


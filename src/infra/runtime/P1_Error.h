/*
 * 🧱 File: P1_Error.h
 * -------------------
 * 📌 Purpose   : Defines a pure C++ runtime error model for infrastructure subsystems (DB, IO, Network, Config).
 * 🧱 Layer     : Infrastructure (Runtime)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-21
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * The P1_Error structure represents a system-level failure that occurs at runtime.
 * It provides strongly typed error codes (enum-based), descriptive messages,
 * and an optional context map for additional metadata (e.g., hostname, credentials path).
 *
 * This type is designed for use by Logger, Notifier, and Repository Factory subsystems.
 * It bridges between infrastructure-level and domain-level errors.
 *
 * 🔗 Depends On:
 *   - domain/core/v1/P1_DomainError.h (for conversion helpers)
 * 🔗 Related   : P1_Logger, P1_Notifier, P1_S2_TaskRepositoryFactory
 * ✅ Tested In : (planned) P1_ErrorTest.cpp
 * ⚠️ Errors    : None (self-contained data structure)
 * ⚡ Performance: O(1) creation and comparison
 * 🔒 Security  : Supports masking of sensitive context entries
 * 🔒 Thread Safety: Immutable after creation (safe for concurrent read)
 */

#pragma once

#include <string>
#include <unordered_map>
#include <utility>

#include "domain/core/errors/P1_DomainError.h"

namespace tasqly::p1::infra::runtime {

enum class RuntimeErrorCode {
  None = 0,
  DbInit = 100,
  DbAuth = 101,
  Timeout = 102,
  Network = 103,
  Io = 104,
  Config = 105,
  Unknown = 199
};

struct P1_Error
{
  RuntimeErrorCode code = RuntimeErrorCode::None;
  std::string message;
  std::unordered_map<std::string, std::string> context;

  P1_Error() = default;
  P1_Error(RuntimeErrorCode c, std::string m)
      : code(c)
      , message(std::move(m))
  {}
  P1_Error(RuntimeErrorCode c, std::string m, std::unordered_map<std::string, std::string> ctx)
      : code(c)
      , message(std::move(m))
      , context(std::move(ctx))
  {}

  // Factories
  static inline P1_Error makeDbInit(std::string msg)
  {
    return {RuntimeErrorCode::DbInit, std::move(msg)};
  }
  static inline P1_Error makeAuth(std::string msg)
  {
    return {RuntimeErrorCode::DbAuth, std::move(msg)};
  }
  static inline P1_Error makeTimeout(std::string msg)
  {
    return {RuntimeErrorCode::Timeout, std::move(msg)};
  }
  static inline P1_Error makeNetwork(std::string msg)
  {
    return {RuntimeErrorCode::Network, std::move(msg)};
  }
  static inline P1_Error makeIo(std::string msg) { return {RuntimeErrorCode::Io, std::move(msg)}; }
  static inline P1_Error makeConfig(std::string msg)
  {
    return {RuntimeErrorCode::Config, std::move(msg)};
  }
  static inline P1_Error makeUnknown(std::string msg = "Unknown runtime error")
  {
    return {RuntimeErrorCode::Unknown, std::move(msg)};
  }

  // Conversions
  tasqly::p1::s1::domain::core::DomainError toDomainError() const;
  static P1_Error fromDomainError(const tasqly::p1::s1::domain::core::DomainError& d);

  // Utilities
  std::string toString(bool masked = true) const;
  static std::string codeToString(RuntimeErrorCode c);

  // Operators
  bool operator==(const P1_Error& other) const noexcept
  {
    return code == other.code && message == other.message && context == other.context;
  }
  bool operator!=(const P1_Error& other) const noexcept { return !(*this == other); }
};

} // namespace tasqly::p1::infra::runtime

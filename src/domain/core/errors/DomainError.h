/*
 * 🧱 File: DomainError.h
 * ----------------------
 * 📌 Purpose   : Pure C++ domain error type (no Qt dependencies).
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-12
 * 🔖 Version   : 1.1 (Enum-based codes + factories)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Represents a domain-level error in a pure C++ context.
 * This error type contains a strongly-typed error code enum,
 * a descriptive message, and an optional key-value context map.
 * 
 * ⚙️ Characteristics:
 * - No Qt dependencies
 * - Fully portable
 * - Deterministic error codes (enum-based)
 * 
 * 🧩 Related:
 *   - DomainResult<T>
 *   - ITaskRepository
 *   - FakeTaskRepository
 */

#pragma once

#include <string>
#include <unordered_map>
#include <utility>

namespace tasqly::domain::core {

/// 🧱 Enumerates all domain-level error categories.
enum class DomainErrorCode {
  Unknown = 0,
  Validation = 1001,
  NotFound = 1002,
  Conflict = 1003,
  Storage = 1004
};

/// 🧩 DomainError — pure C++ representation of an error in domain logic.
struct DomainError
{
  DomainErrorCode code = DomainErrorCode::Unknown;
  std::string message;
  std::unordered_map<std::string, std::string> context;

  // 🧱 Constructors
  DomainError() = default;

  DomainError(DomainErrorCode c, std::string m)
      : code(c)
      , message(std::move(m))
  {}

  DomainError(DomainErrorCode c, std::string m, std::unordered_map<std::string, std::string> ctx)
      : code(c)
      , message(std::move(m))
      , context(std::move(ctx))
  {}

  // 🧩 Factory helpers
  static DomainError Validation(std::string msg)
  {
    return {DomainErrorCode::Validation, std::move(msg)};
  }

  static DomainError NotFound(std::string msg)
  {
    return {DomainErrorCode::NotFound, std::move(msg)};
  }

  static DomainError Conflict(std::string msg)
  {
    return {DomainErrorCode::Conflict, std::move(msg)};
  }

  static DomainError Storage(std::string msg) { return {DomainErrorCode::Storage, std::move(msg)}; }

  static DomainError Unknown(std::string msg = "Unknown error")
  {
    return {DomainErrorCode::Unknown, std::move(msg)};
  }

  // 🧩 Equality operators
  bool operator==(const DomainError& other) const noexcept
  {
    return code == other.code && message == other.message && context == other.context;
  }

  bool operator!=(const DomainError& other) const noexcept { return !(*this == other); }
};

} // namespace tasqly::domain::core

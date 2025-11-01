/*
 * 🧱 File: P1_DomainError.h
 * -------------------------
 * 📌 Purpose   : Pure C++ domain error type (no Qt dependencies).
 * 🧱 Layer     : Domain (Core) — Errors (Phase 1)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-12
 * 🔖 Version   : 1.2 (Namespace Isolation + Inline Factories)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Represents a domain-level error in a pure C++ context.
 * This error type contains a strongly-typed error code enum,
 * a descriptive message, and an optional key-value context map.
 *
 * ⚙️ Characteristics:
 * - No Qt dependencies
 * - Fully portable & serializable
 * - Deterministic error codes (enum-based)
 * - Factory helpers for readability
 *
 * 🔗 Related:
 *   - DomainResult<T>
 *   - ITaskRepository
 *   - FakeTaskRepository
 */

#pragma once

#include <string>
#include <unordered_map>
#include <utility>

namespace tasqly::p1::s1::domain::core {

/// @brief Enumerates all domain-level error categories.
enum class DomainErrorCode {
  Unknown = 0,       ///< Undefined or unexpected error
  Validation = 1001, ///< Entity validation failed
  NotFound = 1002,   ///< Entity not found
  Conflict = 1003,   ///< Conflict with existing entity (e.g., duplicate key)
  Storage = 1004     ///< Failure in repository or data persistence
};

/// @brief Represents a domain error with code, message, and optional context.
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

  // 🧩 Factory helpers (inline)
  static inline DomainError makeValidation(std::string msg)
  {
    return {DomainErrorCode::Validation, std::move(msg)};
  }

  static inline DomainError makeNotFound(std::string msg)
  {
    return {DomainErrorCode::NotFound, std::move(msg)};
  }

  static inline DomainError makeConflict(std::string msg)
  {
    return {DomainErrorCode::Conflict, std::move(msg)};
  }

  static inline DomainError makeStorage(std::string msg)
  {
    return {DomainErrorCode::Storage, std::move(msg)};
  }

  static inline DomainError makeUnknown(std::string msg = "Unknown error")
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

} // namespace tasqly::p1::s1::domain::core

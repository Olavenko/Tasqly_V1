/*
 * 🧱 File: P1_DomainValidation.h
 * ------------------------------
 * 📌 Purpose   : Provides validation helpers for Phase 1 domain entities.
 * 🧱 Layer     : Domain (Core) — Validation Utilities (Phase 1)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-08
 * 🔖 Version   : 1.1 (Namespace Isolation Revision)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * This header defines validation logic for Phase 1 domain entities such as
 * `P1_Task`. The validation functions are pure C++ utilities that enforce
 * basic domain rules and constraints without any dependency on Qt or
 * persistence layers.
 *
 * Rules implemented:
 *   - Title must not be empty or whitespace-only
 *   - Deadline (if provided) must not be in the past
 *
 * ✅ Safe for use in:
 *   - Unit tests
 *   - In-memory and persistent repositories
 *   - Domain service logic (Phase 1)
 */

#pragma once

#include <chrono>
#include <optional>
#include <string>

namespace tasqly::domain::core::v1 {

/// @brief Represents the result of a domain validation check.
struct ValidationResult
{
  bool valid = true;
  std::string message;
};

/// @brief Static utility class for validating domain entities.
class DomainValidation
{
public:
  /// @brief Validate that a task title is non-empty and not whitespace.
  /// @param title The task title to check.
  /// @return ValidationResult indicating validity and message.
  static ValidationResult validateTitle(const std::string& title);

  /// @brief Validate that a deadline (if provided) is not in the past.
  /// @param deadline Optional time_point (may be nullopt).
  /// @return ValidationResult indicating validity and message.
  static ValidationResult validateDeadline(
      const std::optional<std::chrono::system_clock::time_point>& deadline);
};

} // namespace tasqly::domain::core::v1

/*
 * 🧱 File: DomainValidation.cpp
 * -----------------------------
 * 📌 Purpose   : Implements domain validation logic for Task entity.
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-08
 * 🔖 Version   : 1.0 (Initial Implementation)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Contains pure C++ validation helpers ensuring domain consistency.
 * - Title must not be empty and ≤ 200 chars.
 * - Deadline (if set) must be >= current system time.
 * These rules ensure that invalid entities never cross the domain boundary.
 */

#include "domain/core/entities/DomainValidation.h"

namespace tasqly::domain::core {

// 🧩 Validate Task title
ValidationResult DomainValidation::validateTitle(const std::string& title)
{
  if (title.empty())
    return {false, "Title cannot be empty"};

  if (title.size() > 200)
    return {false, "Title exceeds 200 characters"};

  return {true, ""};
}

// 🧩 Validate deadline (must be >= now if defined)
ValidationResult DomainValidation::validateDeadline(
    const std::optional<std::chrono::system_clock::time_point>& deadline)
{
  if (!deadline.has_value())
    return {true, ""};

  const auto now = std::chrono::system_clock::now();
  if (deadline.value() < now)
    return {false, "Deadline cannot be in the past"};

  return {true, ""};
}

} // namespace tasqly::domain::core

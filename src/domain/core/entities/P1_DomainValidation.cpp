/*
 * 🧱 File: P1_DomainValidation.cpp
 * -----------------------------
 * 📌 Purpose   : Implements domain validation logic for Task entity.
 * 🧱 Layer     : Domain (Core) — Validation (Phase 1)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-08
 * 🔖 Version   : 1.1 (Namespace Isolation + Consistency)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Contains pure C++ validation helpers ensuring domain consistency.
 * - Title must not be empty and ≤ 200 chars.
 * - Deadline (if set) must be >= current system time.
 * These rules ensure that invalid entities never cross the domain boundary.
 */

#include "domain/core/entities/P1_DomainValidation.h"
#include <chrono>

namespace tasqly::p1::s1::domain::core {

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

} // namespace tasqly::p1::s1::domain::core

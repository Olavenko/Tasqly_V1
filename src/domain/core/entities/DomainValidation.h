/*
 * 🧱 File: DomainValidation.h
 * ----------------------------
 * 📌 Purpose   : Provides validation helpers for domain entities.
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-08
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 */

#pragma once
#include <chrono>
#include <string>

namespace tasqly::domain::core {

struct ValidationResult
{
  bool valid;
  std::string message;
};

class DomainValidation
{
public:
  // 🧩 Validate Task title
  static ValidationResult validateTitle(const std::string& title);

  // 🧩 Validate deadline (must be >= now)
  static ValidationResult validateDeadline(
      const std::optional<std::chrono::system_clock::time_point>& deadline);
};

} // namespace tasqly::domain::core

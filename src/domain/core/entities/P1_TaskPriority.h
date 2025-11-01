/*
 * 🧱 File: P1_TaskPriority.h
 * --------------------------
 * 📌 Purpose   : Defines priority levels for tasks (Phase 1 isolated version).
 * 🧱 Layer     : Domain (Core) — Entities (Phase 1)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-08
 * 🔖 Version   : 1.3 (Namespace Isolation + Safe Parsing)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Defines the priority hierarchy for Phase 1 domain tasks.
 * - Fully independent from Qt / legacy enums
 * - Case-insensitive string conversion
 * - Uses std::optional for safe parsing
 *
 * ⚙️ Example:
 *   auto pr = taskPriorityFromString("High");
 *   if (pr && *pr == TaskPriority::High) { ... }
 */

#pragma once
#include <optional>
#include <string>

namespace tasqly::p1::s1::domain::core {

/// @brief Represents a task's priority level.
enum class TaskPriority {
  Low = 0,    ///< Non-urgent
  Normal = 1, ///< Default level
  High = 2    ///< Requires immediate attention
};

/// @brief Convert TaskPriority enum → string (stable format)
inline std::string toString(TaskPriority p)
{
  switch (p) {
  case TaskPriority::Low:
    return "Low";
  case TaskPriority::Normal:
    return "Normal";
  case TaskPriority::High:
    return "High";
  default:
    return "Unknown";
  }
}

/// @brief Convert string → TaskPriority enum (case-insensitive)
inline std::optional<TaskPriority> taskPriorityFromString(const std::string& str)
{
  if (str == "Low" || str == "low")
    return TaskPriority::Low;
  if (str == "Normal" || str == "normal")
    return TaskPriority::Normal;
  if (str == "High" || str == "high")
    return TaskPriority::High;
  return std::nullopt;
}

} // namespace tasqly::p1::s1::domain::core

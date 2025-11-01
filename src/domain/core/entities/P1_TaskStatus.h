/*
 * 🧱 File: P1_TaskStatus.h
 * ------------------------
 * 📌 Purpose   : Defines task lifecycle states (Todo, Doing, Done) — Phase 1 isolated version.
 * 🧱 Layer     : Domain (Core) — Entities (Phase 1)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-08
 * 🔖 Version   : 1.3 (Namespace Isolation + Safe Parsing)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Represents the domain-level state machine for tasks.
 * Each task can be in one of three well-defined lifecycle stages:
 *   - Todo  → Not started yet
 *   - Doing → Currently in progress
 *   - Done  → Completed
 *
 * ✅ No Qt dependencies
 * ✅ Case-insensitive conversions
 * ✅ Compatible with all Phase 1 mappers and DTOs
 *
 * ⚙️ Example:
 *   auto status = taskStatusFromString("Doing");
 *   if (status && *status == TaskStatus::Doing) { ... }
 */

#pragma once

#include <optional>
#include <string>

namespace tasqly::p1::s1::domain::core {

/// @brief Defines task lifecycle states.
enum class TaskStatus {
  Todo = 0,  ///< Task not started
  Doing = 1, ///< Task in progress
  Done = 2   ///< Task completed
};

/// @brief Convert TaskStatus enum → string (stable, readable)
inline std::string toString(TaskStatus status)
{
  switch (status) {
  case TaskStatus::Todo:
    return "Todo";
  case TaskStatus::Doing:
    return "Doing";
  case TaskStatus::Done:
    return "Done";
  default:
    return "Unknown";
  }
}

/// @brief Convert string → TaskStatus enum (case-insensitive)
inline std::optional<TaskStatus> taskStatusFromString(const std::string& str)
{
  if (str == "Todo" || str == "todo")
    return TaskStatus::Todo;
  if (str == "Doing" || str == "doing")
    return TaskStatus::Doing;
  if (str == "Done" || str == "done")
    return TaskStatus::Done;
  return std::nullopt;
}

} // namespace tasqly::p1::s1::domain::core

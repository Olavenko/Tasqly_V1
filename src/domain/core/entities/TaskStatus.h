/*
 * 🧱 File: TaskStatus.h
 * ---------------------
 * 📌 Purpose   : Defines task lifecycle states (Todo, Doing, Done).
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-08
 * 🔖 Version   : 1.2 (Fixed name clash with TaskPriority)
 * 🛡️ Stability : Stable
 */

#pragma once
#include <optional>
#include <string>

namespace tasqly::domain::core {

enum class TaskStatus { Todo = 0, Doing = 1, Done = 2 };

// 🧩 Convert enum → string
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

// 🧩 Convert string → enum (safe)
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

} // namespace tasqly::domain::core

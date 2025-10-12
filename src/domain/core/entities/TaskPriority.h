/*
 * 🧱 File: TaskPriority.h
 * -----------------------
 * 📌 Purpose   : Defines priority levels for tasks.
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-08
 * 🔖 Version   : 1.2 (Fixed name clash with TaskStatus)
 * 🛡️ Stability : Stable
 */

#pragma once
#include <optional>
#include <string>

namespace tasqly::domain::core {

enum class TaskPriority { Low = 0, Normal = 1, High = 2 };

// 🧩 Convert enum → string
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

// 🧩 Convert string → enum (safe)
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

} // namespace tasqly::domain::core

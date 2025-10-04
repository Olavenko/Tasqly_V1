/*
 * 🧱 File: Task.h
 * ---------------------
 * 📌 Purpose   : Domain entity representing a single task.
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Represents a Task entity with unique identifier, title, description,
 * status, and timestamps. Bound to a Goal via goalId if applicable.
 */

#pragma once

#include <optional>
#include <QDateTime>
#include <QString>

namespace tasqly::domain::core {

// 🎯 Task Status enum
enum class TaskStatus { Pending, InProgress, Completed, Cancelled };

// 🎯 Task Priority enum
enum class TaskPriority { Low, Normal, High, Critical };

// 📝 Utility converters (for DB/UI interop)
inline QString toString(TaskStatus status)
{
  switch (status) {
  case TaskStatus::Pending:
    return "Pending";
  case TaskStatus::InProgress:
    return "InProgress";
  case TaskStatus::Completed:
    return "Completed";
  case TaskStatus::Cancelled:
    return "Cancelled";
  }
  return "Unknown";
}

inline TaskStatus taskStatusFromString(const QString& s)
{
  if (s == "Pending")
    return TaskStatus::Pending;
  if (s == "InProgress")
    return TaskStatus::InProgress;
  if (s == "Completed")
    return TaskStatus::Completed;
  if (s == "Cancelled")
    return TaskStatus::Cancelled;
  return TaskStatus::Pending; // default fallback
}

inline QString toString(TaskPriority priority)
{
  switch (priority) {
  case TaskPriority::Low:
    return "Low";
  case TaskPriority::Normal:
    return "Normal";
  case TaskPriority::High:
    return "High";
  case TaskPriority::Critical:
    return "Critical";
  }
  return "Unknown";
}

inline TaskPriority taskPriorityFromString(const QString& s)
{
  if (s == "Low")
    return TaskPriority::Low;
  if (s == "Normal")
    return TaskPriority::Normal;
  if (s == "High")
    return TaskPriority::High;
  if (s == "Critical")
    return TaskPriority::Critical;
  return TaskPriority::Normal; // default fallback
}

// 📝 Task entity
struct Task
{
  QString id;
  QString title;
  QString description;
  TaskStatus status;
  TaskPriority priority;
  QDateTime createdAt;
  QDateTime updatedAt;
  std::optional<QDateTime> dueDate;

  // 🆕 Default constructor
  Task() = default;

  // 🆕 Parameterized constructor
  Task(QString id,
       QString title,
       QString description,
       TaskStatus status,
       TaskPriority priority,
       QDateTime createdAt,
       QDateTime updatedAt,
       std::optional<QDateTime> dueDate = std::nullopt)
      : id(std::move(id))
      , title(std::move(title))
      , description(std::move(description))
      , status(status)
      , priority(priority)
      , createdAt(std::move(createdAt))
      , updatedAt(std::move(updatedAt))
      , dueDate(std::move(dueDate))
  {}

  // 🆕 Copy constructor / assignment
  Task(const Task&) = default;
  Task& operator=(const Task&) = default;

  // ⚡ Move constructor
  Task(Task&& other) noexcept
      : id(std::move(other.id))
      , title(std::move(other.title))
      , description(std::move(other.description))
      , status(other.status)
      , priority(other.priority)
      , createdAt(std::move(other.createdAt))
      , updatedAt(std::move(other.updatedAt))
      , dueDate(std::move(other.dueDate))
  {}

  // ⚡ Move assignment
  Task& operator=(Task&& other) noexcept
  {
    if (this != &other) {
      id = std::move(other.id);
      title = std::move(other.title);
      description = std::move(other.description);
      status = other.status;
      priority = other.priority;
      createdAt = std::move(other.createdAt);
      updatedAt = std::move(other.updatedAt);
      dueDate = std::move(other.dueDate);
    }
    return *this;
  }
};

} // namespace tasqly::domain::core

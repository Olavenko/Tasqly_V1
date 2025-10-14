/*
 * 🧱 File: P1_TaskDto.h
 * ---------------------
 * 📌 Purpose   : Defines lightweight data transfer object (DTO) for Task entity.
 * 🧱 Layer     : Domain (Core) — DTO / Mappers (Phase 1)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-09
 * 🔖 Version   : 1.1 (Namespace Isolation + Equality Operators)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * TaskDto is a plain data structure for transferring task data
 * across boundaries (e.g., repositories, mappers, serialization).
 * It mirrors P1_Task fields but uses primitive STL types only.
 *
 * ⚙️ Usage:
 * - Used by P1_TaskMapper for conversions Task <-> TaskDto
 * - Used by persistence and service layers for storage or transport
 */

#pragma once
#include <optional>
#include <string>

namespace tasqly::domain::core::v1 {

/// @brief Serializable representation of a Task entity (Phase 1).
struct TaskDto
{
  /// @brief Unique identifier (UUID string)
  std::string id;

  /// @brief Human-readable title of the task
  std::string title;

  /// @brief Optional notes or description
  std::optional<std::string> notes;

  /// @brief Current status as string ("Todo", "Doing", "Done")
  std::string status;

  /// @brief Current priority as string ("Low", "Normal", "High")
  std::string priority;

  /// @brief Optional ISO8601 deadline (e.g. "2025-10-15T12:30:00Z")
  std::optional<std::string> deadline;

  /// @brief Creation timestamp (ISO8601)
  std::string createdAt;

  /// @brief Last updated timestamp (ISO8601)
  std::string updatedAt;

  // ------------------------------------------------------------------
  // 🔁 Equality Operators — useful in tests & validation
  // ------------------------------------------------------------------
  bool operator==(const TaskDto& other) const noexcept
  {
    return id == other.id && title == other.title && notes == other.notes && status == other.status
           && priority == other.priority && deadline == other.deadline
           && createdAt == other.createdAt && updatedAt == other.updatedAt;
  }

  bool operator!=(const TaskDto& other) const noexcept { return !(*this == other); }
};

} // namespace tasqly::domain::core::v1

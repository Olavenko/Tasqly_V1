/*
 * 🧱 File: P1_TaskMapper.h
 * ------------------------
 * 📌 Purpose   : Defines deterministic conversions between Task and TaskDto.
 * 🧱 Layer     : Domain (Core) — Mappers (Phase 1)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-09
 * 🔖 Version   : 1.1 (Namespace Isolation + ISO helpers)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Provides bidirectional conversion between domain entity (Task)
 * and plain data transfer object (TaskDto).
 * Conversion logic is deterministic and avoids Qt or DB dependencies.
 *
 * 📄 ADR Reference:
 * docs/adr/phase1/task-mapper-strategy-manual.md
 *
 * ⚙️ Depends On:
 *   - domain/core/entities/P1_Task.h
 *   - domain/core/mappers/P1_TaskDto.h
 *   - domain/core/entities/P1_TaskPriority.h
 *   - domain/core/entities/P1_TaskStatus.h
 */

#pragma once
#include <chrono>
#include <string>

#include "domain/core/entities/P1_Task.h"
#include "domain/core/mappers/P1_TaskDto.h"

namespace tasqly::domain::core::v1 {

/// @brief Responsible for manual conversion between Task ↔ TaskDto.
class TaskMapper
{
public:
  friend class TaskMapperBenchAccess;
  // ------------------------------------------------------------------
  // 🔁 Convert Domain → DTO
  // ------------------------------------------------------------------
  /// @brief Converts a domain Task to a serializable TaskDto.
  /// @param task The domain entity to convert.
  /// @return Plain TaskDto ready for persistence or transmission.
  static TaskDto toDto(const Task& task);

  // ------------------------------------------------------------------
  // 🔁 Convert DTO → Domain
  // ------------------------------------------------------------------
  /// @brief Converts a TaskDto back into a validated domain Task entity.
  /// @param dto Serialized task data (from DB / JSON / network).
  /// @return Domain entity Task.
  static Task fromDto(const TaskDto& dto);

private:
  // ------------------------------------------------------------------
  // 🕒 ISO ↔ chrono conversion helpers
  // ------------------------------------------------------------------
  static std::string timePointToIso(const std::chrono::system_clock::time_point& tp);
  static std::chrono::system_clock::time_point isoToTimePoint(const std::string& iso);
};

} // namespace tasqly::domain::core::v1

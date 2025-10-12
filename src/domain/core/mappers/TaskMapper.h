/*
 * 🧱 File: TaskMapper.h
 * ---------------------
 * 📌 Purpose   : Defines manual conversion between Task and TaskDto.
 * 🧱 Layer     : Domain (Core) — Mappers
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-09
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Provides bidirectional conversion between domain entity (Task)
 * and pure data transfer object (TaskDto). Conversion is manual
 * (documented in ADR) to ensure deterministic, dependency-free behavior.
 *
 * 📄 ADR Reference:
 * docs/adr/phase1/task-mapper-strategy-manual.md
 */

#pragma once
#include "domain/core/entities/Task.h"
#include "domain/core/mappers/TaskDto.h"
#include <string>

namespace tasqly::domain::core {

/// @brief Responsible for manual conversion between Task <-> TaskDto
class TaskMapper
{
public:
  // 🧩 Convert Domain -> DTO
  static TaskDto toDto(const Task& task);

  // 🧩 Convert DTO -> Domain
  static Task fromDto(const TaskDto& dto);

private:
  // Internal helpers for time conversion (ISO ↔ chrono)
  static std::string timePointToIso(const std::chrono::system_clock::time_point& tp);
  static std::chrono::system_clock::time_point isoToTimePoint(const std::string& iso);
};

} // namespace tasqly::domain::core

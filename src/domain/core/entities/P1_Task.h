/*
 * 🧱 File: P1_Task.h
 * ------------------
 * 📌 Purpose   : Represents a domain Task entity (Phase 1 isolated version).
 * 🧱 Layer     : Domain (Core) — Entities (Phase 1)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-08
 * 🔖 Version   : 1.2 (Namespace Isolation Revision)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Core business entity representing a single user task.
 * - Contains no Qt or database dependencies.
 * - Generates UUIDs using pure C++ (portable and thread-safe).
 * - Fully isolated from legacy (Phase 0) Task.h that depends on Qt types.
 *
 * ⚙️ Features:
 * - Portable UUID generator
 * - Optional notes & deadline
 * - Timestamps for creation and updates
 *
 * 🔗 Dependencies:
 *   - P1_TaskPriority.h
 *   - P1_TaskStatus.h
 */

#pragma once

#include <chrono>
#include <cstdio>
#include <optional>
#include <random>
#include <string>

#include "domain/core/entities/P1_TaskPriority.h"
#include "domain/core/entities/P1_TaskStatus.h"

namespace tasqly::domain::core::v1 {

// 🧩 Utility — lightweight UUID generator (portable & deterministic)
inline std::string generateUuid()
{
  // ✅ Avoid static RNG — prevents crash on program exit (especially on Windows/MSVC)
  std::random_device rd;
  std::mt19937_64 rng(rd());
  std::uniform_int_distribution<uint64_t> dist;

  uint64_t high = dist(rng);
  uint64_t low = dist(rng);

  char buffer[33];
  std::snprintf(buffer,
                sizeof(buffer),
                "%016llx%016llx",
                static_cast<unsigned long long>(high),
                static_cast<unsigned long long>(low));

  return std::string(buffer);
}

// 🧱 Domain Entity — Task
struct Task
{
  std::string id = generateUuid();              ///< Unique identifier (UUIDv4-like)
  std::string title;                            ///< Non-empty title string
  std::optional<std::string> notes;             ///< Optional notes or description
  TaskStatus status = TaskStatus::Todo;         ///< Current status (Todo / Doing / Done)
  TaskPriority priority = TaskPriority::Normal; ///< Priority level
  std::optional<std::chrono::system_clock::time_point> deadline; ///< Optional due date
  std::chrono::system_clock::time_point createdAt
      = std::chrono::system_clock::now();                      ///< Creation timestamp
  std::chrono::system_clock::time_point updatedAt = createdAt; ///< Last modification timestamp
};

} // namespace tasqly::domain::core::v1

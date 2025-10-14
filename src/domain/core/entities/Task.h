/*
 * 🧱 File: Task.h
 * ----------------
 * 📌 Purpose   : Represents a domain Task entity.
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-08
 * 🔖 Version   : 1.1 (UUID portable fix)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Core business entity representing a single user task.
 * Contains no Qt or database dependencies.
 * Generates UUIDs using pure C++ (no OS-specific headers).
 */

#pragma once
#include <chrono>
#include <cstdio>
#include <optional>
#include <random>
#include <string>

#include "domain/core/entities/TaskPriority.h"
#include "domain/core/entities/TaskStatus.h"

namespace tasqly::domain::core {

// 🧩 Utility — lightweight UUID generator (portable)
inline std::string generateUuid()
{
  // ✅ Avoid static RNG — prevents crash on program exit (Windows/MSVC)
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
  std::string id = generateUuid();
  std::string title;
  std::optional<std::string> notes;
  TaskStatus status = TaskStatus::Todo;
  TaskPriority priority = TaskPriority::Normal;
  std::optional<std::chrono::system_clock::time_point> deadline;
  std::chrono::system_clock::time_point createdAt = std::chrono::system_clock::now();
  std::chrono::system_clock::time_point updatedAt = createdAt;
};

} // namespace tasqly::domain::core

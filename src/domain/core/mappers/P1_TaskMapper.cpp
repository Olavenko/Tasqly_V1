/*
 * 🧱 File: P1_TaskMapper.cpp
 * --------------------------
 * 📌 Purpose   : Manual conversion between Task and TaskDto.
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-14
 * 🔖 Version   : 1.2 (Stable, Clean Build)
 * 🛡️ Stability : Production Ready
 *
 * 🧠 Description:
 * Provides deterministic and dependency-free conversion between:
 *   - Domain entity (Task)
 *   - Data Transfer Object (TaskDto)
 *
 * Conversion ensures:
 *   - Optional fields handled safely
 *   - ISO8601 formatting for time values
 *   - No Qt, DB, or external dependencies
 */

#include "domain/core/mappers/P1_TaskMapper.h"
#include "domain/core/entities/P1_TaskPriority.h"
#include "domain/core/entities/P1_TaskStatus.h"

#include <chrono>
#include <ctime>
#include <format>
#include <string>
#include <string_view>

namespace tasqly::p1::s1::domain::core {

// =====================================================================
// ⚡ Optimized: Convert chrono::time_point -> ISO8601 string (C++20)
// =====================================================================
std::string TaskMapper::timePointToIso(const std::chrono::system_clock::time_point& tp)
{
  try {
    // 🧠 Faster direct formatting (no tm/gmtime conversion)
    // ISO8601 fixed-length: 20 chars → e.g. "2025-10-15T12:00:00Z"
    return std::format("{:%Y-%m-%dT%H:%M:%SZ}", std::chrono::floor<std::chrono::seconds>(tp));
  } catch (...) {
    // 🧯 In case of unsupported chrono::format (old compiler fallback)
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm tm{};
#if defined(_WIN32)
    gmtime_s(&tm, &t);
#else
    gmtime_r(&t, &tm);
#endif
    char buf[32] = {};
    if (std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tm))
      return std::string(buf);
    return "1970-01-01T00:00:00Z";
  }
}

// =====================================================================
// ⚡ Optimized: Convert ISO8601 string -> chrono::time_point
// =====================================================================
std::chrono::system_clock::time_point TaskMapper::isoToTimePoint(const std::string& iso)
{
  if (iso.size() < 20)
    return std::chrono::system_clock::from_time_t(0);

  // 🧠 Manual fixed-width parsing (no locale overhead)
  auto parseInt = [](std::string_view s, int pos, int len) noexcept -> int {
    int v = 0;
    for (int i = 0; i < len; ++i)
      v = v * 10 + (s[pos + i] - '0');
    return v;
  };

  std::tm tm{};
  tm.tm_year = parseInt(iso, 0, 4) - 1900;
  tm.tm_mon = parseInt(iso, 5, 2) - 1;
  tm.tm_mday = parseInt(iso, 8, 2);
  tm.tm_hour = parseInt(iso, 11, 2);
  tm.tm_min = parseInt(iso, 14, 2);
  tm.tm_sec = parseInt(iso, 17, 2);
  tm.tm_isdst = 0;

#if defined(_WIN32)
  std::time_t tt = _mkgmtime(&tm);
#else
  std::time_t tt = timegm(&tm);
#endif

  return std::chrono::system_clock::from_time_t(tt);
}

// =====================================================================
// 🧩 Convert Task -> TaskDto
// =====================================================================
TaskDto TaskMapper::toDto(const Task& task)
{
  TaskDto dto;
  dto.id = task.id;
  dto.title = task.title;
  dto.status = toString(task.status);
  dto.priority = toString(task.priority);

  if (task.notes.has_value() && !task.notes->empty())
    dto.notes = *task.notes;
  else
    dto.notes.reset();

  if (task.deadline.has_value())
    dto.deadline = timePointToIso(*task.deadline);
  else
    dto.deadline.reset();

  dto.createdAt = timePointToIso(task.createdAt);
  dto.updatedAt = timePointToIso(task.updatedAt);

  return dto;
}

// =====================================================================
// ⚡ Optimized: Convert TaskDto -> Task (fast lookup + move semantics)
// =====================================================================
Task TaskMapper::fromDto(const TaskDto& dto)
{
  Task task;

  // ⚡ move strings to avoid redundant heap allocations
  task.id = std::move(dto.id);
  task.title = std::move(dto.title);

  // 🧩 optional notes helper
  auto hasValue = [](const std::optional<std::string>& s) noexcept -> bool {
    return s && !s->empty();
  };

  if (hasValue(dto.notes))
    task.notes = *dto.notes;
  else
    task.notes.reset();

  // ==========================================================
  // ⚡ Fast lookup: Status + Priority (constexpr table lookup)
  // ==========================================================
  static constexpr std::pair<std::string_view, TaskStatus> STATUS_TABLE[]
      = {{"Todo", TaskStatus::Todo}, {"Doing", TaskStatus::Doing}, {"Done", TaskStatus::Done}};

  static constexpr std::pair<std::string_view, TaskPriority> PRIORITY_TABLE[]
      = {{"Low", TaskPriority::Low}, {"Normal", TaskPriority::Normal}, {"High", TaskPriority::High}};

  // 🔍 tiny lookup lambdas (no dynamic allocations)
  auto fastStatus = [](const std::string& s) noexcept -> TaskStatus {
    for (auto&& [key, val] : STATUS_TABLE)
      if (s == key)
        return val;
    return TaskStatus::Todo;
  };

  auto fastPriority = [](const std::string& s) noexcept -> TaskPriority {
    for (auto&& [key, val] : PRIORITY_TABLE)
      if (s == key)
        return val;
    return TaskPriority::Normal;
  };

  task.status = fastStatus(dto.status);
  task.priority = fastPriority(dto.priority);

  // ==========================================================
  // 🕒 Use optimized isoToTimePoint() (already improved)
  // ==========================================================
  if (hasValue(dto.deadline))
    task.deadline = isoToTimePoint(*dto.deadline);
  else
    task.deadline.reset();

  task.createdAt = isoToTimePoint(dto.createdAt);
  task.updatedAt = isoToTimePoint(dto.updatedAt);

  return task;
}

} // namespace tasqly::p1::s1::domain::core

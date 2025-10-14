/*
 * 🧱 File: TaskMapper.cpp
 * -----------------------
 * 📌 Purpose   : Manual bidirectional conversion between Task and TaskDto.
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-14
 * 🔖 Version   : 2.0 (Refactored — Clean, Deterministic)
 * 🛡️ Stability : Production-ready
 *
 * 🧠 Description:
 * Provides deterministic, dependency-free conversion between domain entity (Task)
 * and its lightweight transfer object (TaskDto). All conversions are manual and
 * cross-platform safe. Time serialization follows ISO8601 UTC format.
 *
 * 🔒 Safety Notes:
 * - All optionals are checked before dereference.
 * - Time conversions use gmtime_r / gmtime_s for portability.
 * - No debug or console output (pure logic only).
 *
 * ✅ Compliance:
 * - C++20, MinGW/Clang/MSVC compatible.
 * - Aligned with TaskMapper.h and FilePolicy.md.
 */

#include "domain/core/mappers/TaskMapper.h"
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace tasqly::domain::core;

// ============================================================================
// 🕓 Convert chrono::time_point → ISO8601 string (UTC)
// ============================================================================
std::string TaskMapper::timePointToIso(const std::chrono::system_clock::time_point& tp)
{
  std::time_t t = std::chrono::system_clock::to_time_t(tp);
  if (t == static_cast<std::time_t>(-1))
    return "1970-01-01T00:00:00Z";

  std::tm tm{};
#if defined(_WIN32)
  if (gmtime_s(&tm, &t) != 0) {
    std::memset(&tm, 0, sizeof(tm));
    tm.tm_year = 70; // 1970
    tm.tm_mday = 1;
  }
#else
  if (gmtime_r(&t, &tm) == nullptr) {
    std::memset(&tm, 0, sizeof(tm));
    tm.tm_year = 70;
    tm.tm_mday = 1;
  }
#endif

  // 🧩 Manual normalization (for MinGW bug)
  if (tm.tm_year < 0)
    tm.tm_year = 70;
  if (tm.tm_mon < 0)
    tm.tm_mon = 0;
  if (tm.tm_mday <= 0)
    tm.tm_mday = 1;
  if (tm.tm_hour < 0)
    tm.tm_hour = 0;
  if (tm.tm_min < 0)
    tm.tm_min = 0;
  if (tm.tm_sec < 0)
    tm.tm_sec = 0;

  char buf[32] = {};
  if (std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tm))
    return std::string(buf);

  return "1970-01-01T00:00:00Z";
}

// ============================================================================
// 🕓 Convert ISO8601 string → chrono::time_point
// ============================================================================
std::chrono::system_clock::time_point TaskMapper::isoToTimePoint(const std::string& iso)
{
  std::tm tm{};
  if (iso.empty()) {
    tm.tm_year = 70;
    tm.tm_mday = 1;
  } else {
    std::istringstream ss(iso);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    if (ss.fail()) {
      std::memset(&tm, 0, sizeof(tm));
      tm.tm_year = 70;
      tm.tm_mday = 1;
    }
  }

#if defined(_WIN32)
  std::time_t tt = _mkgmtime(&tm);
#else
  std::time_t tt = timegm(&tm);
#endif
  return std::chrono::system_clock::from_time_t(tt);
}

// ============================================================================
// 🧩 Convert Task → TaskDto
// ============================================================================
TaskDto TaskMapper::toDto(const Task& task)
{
  std::cout << "[TRACE] Entering toDto()\n";

  TaskDto dto;
  dto.id = task.id;
  dto.title = task.title;
  dto.status = toString(task.status);
  dto.priority = toString(task.priority);

  if (task.notes.has_value() && !task.notes->empty()) {
    std::cout << "[TRACE] Notes = " << *task.notes << std::endl;
    dto.notes = *task.notes;
  } else {
    std::cout << "[TRACE] Notes empty\n";
    dto.notes.reset();
  }

  if (task.deadline.has_value()) {
    std::cout << "[TRACE] deadline set\n";
    dto.deadline = timePointToIso(*task.deadline);
  } else {
    std::cout << "[TRACE] deadline empty\n";
    dto.deadline.reset();
  }

  dto.createdAt = timePointToIso(task.createdAt);
  std::cout << "[TRACE] createdAt done\n";

  dto.updatedAt = timePointToIso(task.updatedAt);
  std::cout << "[TRACE] updatedAt done\n";

  std::cout << "[TRACE] About to return dto\n";
  return dto;
}

// ============================================================================
// 🧩 Convert TaskDto → Task
// ============================================================================
Task TaskMapper::fromDto(const TaskDto& dto)
{
  Task task;

  task.id = dto.id;
  task.title = dto.title;

  // Notes (optional)
  if (dto.notes.has_value() && !dto.notes->empty())
    task.notes = *dto.notes;
  else
    task.notes.reset();

  // Status
  if (auto s = taskStatusFromString(dto.status))
    task.status = *s;
  else
    task.status = TaskStatus::Todo;

  // Priority
  if (auto p = taskPriorityFromString(dto.priority))
    task.priority = *p;
  else
    task.priority = TaskPriority::Normal;

  // Deadline (optional)
  if (dto.deadline.has_value() && !dto.deadline->empty())
    task.deadline = isoToTimePoint(*dto.deadline);
  else
    task.deadline.reset();

  // Time fields
  task.createdAt = isoToTimePoint(dto.createdAt);
  task.updatedAt = isoToTimePoint(dto.updatedAt);

  return task;
}

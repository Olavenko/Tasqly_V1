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
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace tasqly::domain::core::v1 {

// =====================================================================
// 🕓 Convert chrono::time_point -> ISO8601 string
// =====================================================================
std::string TaskMapper::timePointToIso(const std::chrono::system_clock::time_point& tp)
{
  std::time_t t = std::chrono::system_clock::to_time_t(tp);
  if (t < 0)
    t = 0;

  std::tm tm{};
#if defined(_WIN32)
  if (gmtime_s(&tm, &t) != 0) {
    std::memset(&tm, 0, sizeof(tm));
    tm.tm_year = 70;
    tm.tm_mday = 1;
  }
#else
  if (gmtime_r(&t, &tm) == nullptr) {
    std::memset(&tm, 0, sizeof(tm));
    tm.tm_year = 70;
    tm.tm_mday = 1;
  }
#endif

  char buf[32] = {};
  if (std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tm))
    return std::string(buf);

  return "1970-01-01T00:00:00Z";
}

// =====================================================================
// 🕓 Convert ISO8601 string -> chrono::time_point
// =====================================================================
std::chrono::system_clock::time_point TaskMapper::isoToTimePoint(const std::string& iso)
{
  std::tm tm{};
  if (!iso.empty()) {
    std::istringstream ss(iso);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
  } else {
    tm.tm_year = 70;
    tm.tm_mday = 1;
  }

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
// 🧩 Convert TaskDto -> Task
// =====================================================================
Task TaskMapper::fromDto(const TaskDto& dto)
{
  Task task;
  task.id = dto.id;
  task.title = dto.title;

  if (dto.notes.has_value() && !dto.notes->empty())
    task.notes = *dto.notes;
  else
    task.notes.reset();

  task.status = taskStatusFromString(dto.status).value_or(TaskStatus::Todo);
  task.priority = taskPriorityFromString(dto.priority).value_or(TaskPriority::Normal);

  if (dto.deadline.has_value() && !dto.deadline->empty())
    task.deadline = isoToTimePoint(*dto.deadline);
  else
    task.deadline.reset();

  task.createdAt = isoToTimePoint(dto.createdAt);
  task.updatedAt = isoToTimePoint(dto.updatedAt);

  return task;
}

} // namespace tasqly::domain::core::v1

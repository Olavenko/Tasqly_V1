/*
 * 🧱 File: TaskMapper.cpp
 * -----------------------
 * 📌 Purpose   : Implements manual conversion logic between Task and TaskDto.
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-09
 * 🔖 Version   : 1.0
 * 🛡️ Stability : Stable
 */

#include "domain/core/mappers/TaskMapper.h"
#include <iomanip>
#include <sstream>

namespace tasqly::domain::core {

// 🧩 Convert Task -> TaskDto
TaskDto TaskMapper::toDto(const Task& task)
{
  TaskDto dto;
  dto.id = task.id;
  dto.title = task.title;
  dto.notes = task.notes;
  dto.status = toString(task.status);
  dto.priority = toString(task.priority);
  if (task.deadline.has_value())
    dto.deadline = timePointToIso(task.deadline.value());
  dto.createdAt = timePointToIso(task.createdAt);
  dto.updatedAt = timePointToIso(task.updatedAt);
  return dto;
}

// 🧩 Convert TaskDto -> Task
Task TaskMapper::fromDto(const TaskDto& dto)
{
  Task task;
  task.id = dto.id;
  task.title = dto.title;
  task.notes = dto.notes;

  // Enums parsing
  auto s = taskStatusFromString(dto.status);
  task.status = s.has_value() ? s.value() : TaskStatus::Todo;

  auto p = taskPriorityFromString(dto.priority);
  task.priority = p.has_value() ? p.value() : TaskPriority::Normal;

  // Time parsing
  if (dto.deadline.has_value())
    task.deadline = isoToTimePoint(dto.deadline.value());

  task.createdAt = isoToTimePoint(dto.createdAt);
  task.updatedAt = isoToTimePoint(dto.updatedAt);

  return task;
}

// 🕓 Convert chrono::time_point -> ISO8601 string
std::string TaskMapper::timePointToIso(const std::chrono::system_clock::time_point& tp)
{
  std::time_t t = std::chrono::system_clock::to_time_t(tp);
  std::tm tm{};
#if defined(_WIN32)
  gmtime_s(&tm, &t); // safer on Windows
#else
  gmtime_r(&t, &tm);
#endif
  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
  return oss.str();
}

// 🕓 Convert ISO8601 string -> chrono::time_point
std::chrono::system_clock::time_point TaskMapper::isoToTimePoint(const std::string& iso)
{
  if (iso.empty())
    return std::chrono::system_clock::now(); // fallback safety

  std::tm tm{};
  std::istringstream ss(iso);
  ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
  if (ss.fail())
    return std::chrono::system_clock::now(); // fallback

#if defined(_WIN32)
  time_t tt = _mkgmtime(&tm); // safe UTC conversion on Windows
#else
  time_t tt = timegm(&tm);
#endif
  return std::chrono::system_clock::from_time_t(tt);
}

} // namespace tasqly::domain::core

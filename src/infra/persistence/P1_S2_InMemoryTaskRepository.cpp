/*
 * 🧱 File: P1_S2_InMemoryTaskRepository.cpp
 * ----------------------------------------
 * 📌 Purpose   : Implements thread-safe in-memory Task repository for Phase 1 / Slice 2.
 * 🧱 Layer     : Infrastructure (Persistence)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-19
 *
 * 🧠 Notes:
 * Implements ITaskRepository with STL containers and no external dependencies.
 * Provides consistent error handling via DomainResult / DomainError.
 */

#include "infra/persistence/P1_S2_InMemoryTaskRepository.h"
#include "infra/runtime/P1_Logger.h"
#include <algorithm>
#include <chrono>

using namespace tasqly::p1::s1::domain::core;
using namespace tasqly::p1::infra::persistence;

// 📝 Create a new task
DomainResult<Task> P1_S2_InMemoryTaskRepository::create(const Task& task)
{
  std::scoped_lock lock(m_mutex);

  if (m_tasks.find(task.id) != m_tasks.end()) {
    return DomainResult<Task>::err(DomainError::makeConflict("Task with same ID already exists"));
  }

  m_tasks.emplace(task.id, task);
  return DomainResult<Task>::ok(task);
}

// 🔎 Retrieve a task by ID
DomainResult<Task> P1_S2_InMemoryTaskRepository::getById(const std::string& id) const
{
  std::scoped_lock lock(m_mutex);

  auto it = m_tasks.find(id);
  if (it == m_tasks.end()) {
    return DomainResult<Task>::err(DomainError::makeNotFound("Task not found: " + id));
  }

  return DomainResult<Task>::ok(it->second);
}

// ✏️ Update existing task
DomainResult<Task> P1_S2_InMemoryTaskRepository::update(const Task& task)
{
  std::scoped_lock lock(m_mutex);

  auto it = m_tasks.find(task.id);
  if (it == m_tasks.end()) {
    return DomainResult<Task>::err(
        DomainError::makeNotFound("Task not found for update: " + task.id));
  }

  auto updated = task;
  updated.updatedAt = std::chrono::system_clock::now();
  it->second = updated;

  return DomainResult<Task>::ok(updated);
}

// 🗑️ Remove task by ID
DomainResult<void> P1_S2_InMemoryTaskRepository::remove(const std::string& id)
{
  std::scoped_lock lock(m_mutex);

  auto it = m_tasks.find(id);
  if (it == m_tasks.end()) {
    return DomainResult<void>::err(
        DomainError::makeNotFound("Cannot remove non-existing task: " + id));
  }

  m_tasks.erase(it);
  return DomainResult<void>::ok();
}

// 📋 List tasks with optional filters + pagination
DomainResult<std::vector<Task>> P1_S2_InMemoryTaskRepository::list(
    std::optional<TaskStatus> status,
    std::optional<TaskPriority> priority,
    std::size_t limit,
    std::size_t offset) const
{
  std::scoped_lock lock(m_mutex);

  std::vector<Task> result;
  result.reserve(m_tasks.size());

  // Filter tasks
  for (const auto& [_, task] : m_tasks) {
    if (status && task.status != *status)
      continue;
    if (priority && task.priority != *priority)
      continue;

    result.push_back(task);
  }

  // Apply pagination
  if (offset >= result.size()) {
    return DomainResult<std::vector<Task>>::ok({});
  }

  const auto end = std::min(result.size(), offset + limit);
  std::vector<Task> paged(result.begin() + static_cast<long>(offset),
                          result.begin() + static_cast<long>(end));

  return DomainResult<std::vector<Task>>::ok(std::move(paged));
}

// 🧩 Seed demo data (development/testing parity)
void P1_S2_InMemoryTaskRepository::seedDemoData()
{
  std::scoped_lock lock(m_mutex);

  runtime::P1_Logger::instance().info("[InMemoryRepo] Seeding demo tasks...");

  auto now = std::chrono::system_clock::now();

  std::vector<Task> demoTasks = {Task{"1",
                                      "Implement login page",
                                      "UI + validation",
                                      TaskStatus::Doing,
                                      TaskPriority::High,
                                      std::nullopt,
                                      now,
                                      now},
                                 Task{"2",
                                      "Refactor repository factory",
                                      "Add retry + fallback",
                                      TaskStatus::Todo,
                                      TaskPriority::Normal,
                                      std::nullopt,
                                      now,
                                      now},
                                 Task{"3",
                                      "Integrate Postgres migrations",
                                      "Verify indices + rollback",
                                      TaskStatus::Done,
                                      TaskPriority::Low,
                                      std::nullopt,
                                      now,
                                      now}};

  for (const auto& task : demoTasks) {
    m_tasks.emplace(task.id, task);
  }

  runtime::P1_Logger::instance().info("[InMemoryRepo] Seeded " + std::to_string(demoTasks.size())
                                      + " tasks.");
}

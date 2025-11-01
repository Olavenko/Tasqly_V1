/*
 * 🧱 File: FakeTaskRepository.cpp
 * -------------------------------
 * 📌 Purpose   : Implements FakeTaskRepository logic for testing.
 * 🧱 Layer     : Domain (Fakes / Testing)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-12
 * 🔖 Version   : 1.2 (Safe Copy Return + Memory Hygiene Fix)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Implements CRUD logic using an in-memory vector<Task>.
 * Fully isolated from DB and Qt dependencies.
 * 
 * ✅ Fixes:
 *   - Prevents SegFaults caused by dangling references
 *   - Ensures DomainResult<Task> always holds a copy (value semantics)
 *   - Adds thread-safe vector cleanup with shrink_to_fit()
 */

#include "FakeTaskRepository.h"
#include <algorithm>

namespace tasqly::p1::s1::domain::core {

// 📝 Create — add a new Task to the in-memory store
DomainResult<Task> FakeTaskRepository::create(const Task& task)
{
  std::scoped_lock lock(_mutex);

  // 🔍 Check for duplicate IDs
  auto exists = std::any_of(_tasks.begin(), _tasks.end(), [&](const Task& t) {
    return t.id == task.id;
  });

  if (exists)
    return DomainResult<Task>::err(DomainError::makeConflict("Task with same ID already exists"));

  // ✅ Push copy (not reference) into vector
  _tasks.push_back(task);

  // ✅ Return copy of stored object (avoid dangling references)
  return DomainResult<Task>::ok(Task(_tasks.back()));
}

// 🔎 Retrieve task by ID
DomainResult<Task> FakeTaskRepository::getById(const std::string& id) const
{
  std::scoped_lock lock(_mutex);

  auto it = std::find_if(_tasks.begin(), _tasks.end(), [&](const Task& t) { return t.id == id; });

  if (it == _tasks.end())
    return DomainResult<Task>::err(DomainError::makeNotFound("Task not found"));

  // ✅ Return copy instead of reference
  return DomainResult<Task>::ok(Task(*it));
}

// ✏️ Update existing task
DomainResult<Task> FakeTaskRepository::update(const Task& task)
{
  std::scoped_lock lock(_mutex);

  for (auto& t : _tasks) {
    if (t.id == task.id) {
      t = task;
      t.updatedAt = std::chrono::system_clock::now();
      return DomainResult<Task>::ok(Task(t)); // ✅ return copy
    }
  }

  return DomainResult<Task>::err(DomainError::makeNotFound("Cannot update non-existent task"));
}

// 🗑️ Remove by ID
DomainResult<void> FakeTaskRepository::remove(const std::string& id)
{
  std::scoped_lock lock(_mutex);

  auto it = std::remove_if(_tasks.begin(), _tasks.end(), [&](const Task& t) { return t.id == id; });

  if (it == _tasks.end())
    return DomainResult<void>::err(DomainError::makeNotFound("Task not found"));

  _tasks.erase(it, _tasks.end());
  return DomainResult<void>::ok();
}

// 📋 List tasks with optional filters and pagination
DomainResult<std::vector<Task>> FakeTaskRepository::list(std::optional<TaskStatus> status,
                                                         std::optional<TaskPriority> priority,
                                                         std::size_t limit,
                                                         std::size_t offset) const
{
  std::scoped_lock lock(_mutex);

  std::vector<Task> result;
  result.reserve(_tasks.size());

  for (const auto& t : _tasks) {
    if (status && t.status != *status)
      continue;
    if (priority && t.priority != *priority)
      continue;
    result.push_back(t);
  }

  if (offset >= result.size())
    return DomainResult<std::vector<Task>>::ok({});

  const std::size_t end = std::min(result.size(), offset + limit);
  std::vector<Task> page(result.begin() + offset, result.begin() + end);

  return DomainResult<std::vector<Task>>::ok(std::move(page));
}

// 🧹 Clear all stored tasks (safe & thread-safe)
void FakeTaskRepository::clear()
{
  std::scoped_lock lock(_mutex);

  // ✅ ensure safe destruction of all elements
  _tasks.clear();

  // ✅ release allocated memory to avoid residual capacity
  _tasks.shrink_to_fit();
}

} // namespace tasqly::p1::s1::domain::core

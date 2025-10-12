/*
 * 🧱 File: FakeTaskRepository.cpp
 * -------------------------------
 * 📌 Purpose   : Implements FakeTaskRepository logic for testing.
 * 🧱 Layer     : Domain (Fakes / Testing)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-12
 * 🔖 Version   : 1.0
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Implements CRUD logic using an in-memory vector<Task>.
 * Suitable for integration testing without DB or Qt dependencies.
 */

#include "FakeTaskRepository.h"

namespace tasqly::domain::core {

// 📝 Create
DomainResult<Task> FakeTaskRepository::create(const Task& task)
{
  std::scoped_lock lock(_mutex);

  auto exists = std::any_of(_tasks.begin(), _tasks.end(), [&](const Task& t) {
    return t.id == task.id;
  });
  if (exists)
    return DomainResult<Task>::err(DomainError::Conflict("Task with same ID already exists"));

  _tasks.push_back(task);
  Task created = _tasks.back();
  return DomainResult<Task>::ok(std::move(created)); // ✅ move-safe
}

// 🔎 GetById
DomainResult<Task> FakeTaskRepository::getById(const std::string& id) const
{
  std::scoped_lock lock(_mutex);

  auto it = std::find_if(_tasks.begin(), _tasks.end(), [&](const Task& t) { return t.id == id; });
  if (it == _tasks.end())
    return DomainResult<Task>::err(DomainError::NotFound("Task not found"));

  Task found = *it;
  return DomainResult<Task>::ok(std::move(found)); // ✅ move-safe
}

// ✏️ Update
DomainResult<Task> FakeTaskRepository::update(const Task& task)
{
  std::scoped_lock lock(_mutex);

  for (auto& t : _tasks) {
    if (t.id == task.id) {
      t = task;
      t.updatedAt = std::chrono::system_clock::now();
      Task updated = t;
      return DomainResult<Task>::ok(std::move(updated)); // ✅ move-safe
    }
  }

  return DomainResult<Task>::err(DomainError::NotFound("Cannot update non-existent task"));
}

// 🗑️ Remove
DomainResult<void> FakeTaskRepository::remove(const std::string& id)
{
  std::scoped_lock lock(_mutex);

  auto it = std::remove_if(_tasks.begin(), _tasks.end(), [&](const Task& t) { return t.id == id; });

  if (it == _tasks.end())
    return DomainResult<void>::err(DomainError::NotFound("Task not found"));

  _tasks.erase(it, _tasks.end());
  return DomainResult<void>::ok();
}

// 📋 List & Filter
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

  std::size_t end = std::min(result.size(), offset + limit);
  std::vector<Task> page(result.begin() + offset, result.begin() + end);

  return DomainResult<std::vector<Task>>::ok(std::move(page));
}

// 🧹 Clear all
void FakeTaskRepository::clear()
{
  std::scoped_lock lock(_mutex);
  _tasks.clear();
}

} // namespace tasqly::domain::core

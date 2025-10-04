/*
 * 🧱 File: InMemoryTaskRepository.cpp
 * ----------------------------------
 * 📌 Purpose   : Implementation of InMemoryTaskRepository methods.
 * 🧱 Layer     : Infrastructure (Persistence)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 * 🗓️ Updated   : 2025-09-10
 * 🔖 Version   : 1.1 (Fixed + Template aligned)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Provides a thread-safe in-memory repository for Task entities.
 * Implements synchronous helpers (*Sync) and wraps them with
 * asynchronous std::future APIs for integration with use cases.
 *
 * 🔗 Depends On:
 *   - domain/core/Task.h
 *   - domain/core/ITaskRepository.h
 *   - domain/core/Result.h
 * 🔗 Related   : InMemoryTaskRepository.h
 * ✅ Tested In : tests/unit/persistence/test_inmemory_task_repository.cpp
 * ⚠️ Errors    : Returns Result with RepoErrorCode::NotFound / AlreadyExists
 * ⚡ Performance: O(1) average access (unordered_map)
 * 🔒 Security  : Thread-safety ensured by std::scoped_lock
 * 🕒 Last Major Change: 2025-09-10
 */

#include "infra/persistence/InMemoryTaskRepository.h"
#include <algorithm>

using namespace tasqly::domain::core;

// 🏗️ Constructor with optional seeding
InMemoryTaskRepository::InMemoryTaskRepository(bool seed, int reserveHint)
{
  m_tasks.reserve(reserveHint > 0 ? reserveHint : 10240); // ⚡ default ~10k

  if (seed) {
    Task t1{"1",
            "Seed Task 1",
            "Demo task",
            tasqly::domain::core::TaskStatus::Pending,  // ✅ Enum
            tasqly::domain::core::TaskPriority::Normal, // ✅ Enum
            QDateTime::currentDateTime(),
            QDateTime::currentDateTime(),
            std::nullopt};

    Task t2{"2",
            "Seed Task 2",
            "Demo task",
            tasqly::domain::core::TaskStatus::InProgress, // ✅ Enum
            tasqly::domain::core::TaskPriority::Normal,   // ✅ Enum
            QDateTime::currentDateTime(),
            QDateTime::currentDateTime(),
            std::nullopt};

    m_tasks.try_emplace(t1.id.toStdString(), std::move(t1));
    m_tasks.try_emplace(t2.id.toStdString(), std::move(t2));
  }
}

// 🆕 Internal synchronous method: add a task (copy)
Result<Task> InMemoryTaskRepository::addSync(const Task& task)
{
  std::scoped_lock lock(m_mutex);
  auto [it, inserted] = m_tasks.try_emplace(task.id.toStdString(), task);
  if (!inserted) {
    return err<Task>(Error{static_cast<int>(RepoErrorCode::AlreadyExists),
                           "Task already exists",
                           {{"id", task.id}}});
  }
  return ok(it->second);
}

// 🆕 Internal synchronous method: add a task (move)
Result<Task> InMemoryTaskRepository::addSync(Task&& task)
{
  std::scoped_lock lock(m_mutex);
  auto [it, inserted] = m_tasks.try_emplace(task.id.toStdString(), std::move(task));
  if (!inserted) {
    return err<Task>(Error{static_cast<int>(RepoErrorCode::AlreadyExists),
                           "Task already exists",
                           {{"id", task.id}}});
  }
  return ok(it->second);
}

// ➕ Public API: add (const ref → forward to copy-sync)
std::future<Result<Task>> InMemoryTaskRepository::add(const Task& task)
{
  std::promise<Result<Task>> p;
  auto fut = p.get_future();
  p.set_value(addSync(task));
  return fut;
}

// ➕ Public API: add (move overload)
std::future<Result<Task>> InMemoryTaskRepository::add(Task&& task)
{
  std::promise<Result<Task>> p;
  auto fut = p.get_future();
  p.set_value(addSync(std::move(task)));
  return fut;
}

// 🆕 Internal synchronous method: update (copy)
Result<Task> InMemoryTaskRepository::updateSync(const Task& task)
{
  std::scoped_lock lock(m_mutex);
  auto it = m_tasks.find(task.id.toStdString());
  if (it == m_tasks.end()) {
    return err<Task>(
        Error{static_cast<int>(RepoErrorCode::NotFound), "Task not found", {{"id", task.id}}});
  }
  it->second = task; // copy
  return ok(it->second);
}

// 🆕 Internal synchronous method: update (move)
Result<Task> InMemoryTaskRepository::updateSync(Task&& task)
{
  std::scoped_lock lock(m_mutex);
  auto it = m_tasks.find(task.id.toStdString());
  if (it == m_tasks.end()) {
    return err<Task>(
        Error{static_cast<int>(RepoErrorCode::NotFound), "Task not found", {{"id", task.id}}});
  }
  it->second = std::move(task); // move
  return ok(it->second);
}

// 🔄 Public API: update (const ref → copy)
std::future<Result<Task>> InMemoryTaskRepository::update(const Task& task)
{
  std::promise<Result<Task>> p;
  auto fut = p.get_future();
  p.set_value(updateSync(task));
  return fut;
}

// 🔄 Public API: update (move overload)
std::future<Result<Task>> InMemoryTaskRepository::update(Task&& task)
{
  std::promise<Result<Task>> p;
  auto fut = p.get_future();
  p.set_value(updateSync(std::move(task)));
  return fut;
}

// 🆕 Internal synchronous method: remove a task
Result<void> InMemoryTaskRepository::removeSync(const QString& id)
{
  std::scoped_lock lock(m_mutex);

  auto it = m_tasks.find(id.toStdString());
  if (it == m_tasks.end()) {
    return err<void>(
        Error{static_cast<int>(RepoErrorCode::NotFound), "Task not found", {{"id", id}}});
  }
  m_tasks.erase(it);
  return ok();
}

// 🗑️ Public API: remove
std::future<Result<void>> InMemoryTaskRepository::remove(const QString& id)
{
  std::promise<Result<void>> p;
  auto fut = p.get_future();
  p.set_value(removeSync(id));
  return fut;
}

// 🆕 Internal synchronous method: retrieve a task by ID
Result<Task> InMemoryTaskRepository::getByIdSync(const QString& id)
{
  std::scoped_lock lock(m_mutex);

  auto it = m_tasks.find(id.toStdString());
  if (it == m_tasks.end()) {
    return err<Task>(
        Error{static_cast<int>(RepoErrorCode::NotFound), "Task not found", {{"id", id}}});
  }
  return ok(it->second);
}

// 🔎 Public API: getById
std::future<Result<Task>> InMemoryTaskRepository::getById(const QString& id)
{
  std::promise<Result<Task>> p;
  auto fut = p.get_future();
  p.set_value(getByIdSync(id));
  return fut;
}

// 🆕 Internal synchronous method: list tasks with pagination
Result<std::vector<Task>> InMemoryTaskRepository::listSync(int offset, int limit)
{
  std::scoped_lock lock(m_mutex);

  std::vector<Task> result;
  result.reserve(std::min(limit, static_cast<int>(m_tasks.size())));

  int skipped = 0;
  for (auto& [_, task] : m_tasks) {
    if (skipped++ < offset)
      continue;
    result.emplace_back(task); // copy لكل Task
    if ((int) result.size() >= limit)
      break;
  }

  return ok(std::move(result)); // move-friendly return
}

// 📋 Public API: list (futures are synchronous-wrapped, not true async)
std::future<Result<std::vector<Task>>> InMemoryTaskRepository::list(int limit, int offset)
{
  std::promise<Result<std::vector<Task>>> p;
  auto fut = p.get_future();
  p.set_value(listSync(offset, limit));
  return fut;
}

// ⚡ Bulk Insert: add multiple tasks at once with single lock
void InMemoryTaskRepository::addBulk(std::vector<Task>&& tasks)
{
  std::scoped_lock lock(m_mutex);
  for (auto& t : tasks) {
    std::string key = t.id.toStdString();
    if (!m_tasks.contains(key)) {
      m_tasks.emplace(std::move(key), std::move(t));
    }
    // ⚠️ If task already exists, skip silently (for benchmark seeding)
  }
}

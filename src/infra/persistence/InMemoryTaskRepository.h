/*
 * 🧱 File: InMemoryTaskRepository.h
 * ---------------------------------
 * 📌 Purpose   : In-memory implementation of ITaskRepository.
 * 🧱 Layer     : Infrastructure (Persistence)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Provides a thread-safe in-memory repository for Task entities.
 * Supports CRUD operations with optional seed data for testing/demo.
 */

#pragma once

#include "domain/core/Error.h"
#include "domain/core/ITaskRepository.h"
#include "domain/core/Result.h"
#include "domain/core/Task.h"

#include <future>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace tasqly::domain::core {

class InMemoryTaskRepository : public ITaskRepository
{
public:
  explicit InMemoryTaskRepository(bool seed = false, int reserveHint = 0);

  // ➕ Adds a new task to the repository
  std::future<tasqly::domain::core::Result<Task>> add(const Task& task) override;
  // 🆕 move overload
  std::future<Result<Task>> add(Task&& task);

  // 🔄 Updates an existing task
  std::future<tasqly::domain::core::Result<Task>> update(const Task& task) override;
  // 🆕 move overload
  std::future<Result<Task>> update(Task&& task);

  // ❌ Removes a task by ID
  std::future<tasqly::domain::core::Result<void>> remove(const QString& id) override;

  // 🔎 Retrieves a task by ID
  std::future<tasqly::domain::core::Result<Task>> getById(const QString& id) override;

  // 📋 Lists tasks with pagination
  std::future<tasqly::domain::core::Result<std::vector<Task>>> list(int limit, int offset) override;

  // ➕ Adds a batch of new tasks to the repository (bulk insert, perf optimization)
  void addBulk(std::vector<Task>&& tasks);

private:
  // 🛑 Tasks stored by value (copy). Can be optimized later with shared_ptr if needed.
  std::unordered_map<std::string, Task> m_tasks;
  mutable std::mutex m_mutex;

  // 👉 Internal synchronous helpers used by async wrappers
  Result<Task> addSync(const Task& task);
  Result<Task> addSync(Task&& task); // 🆕 move overload
  Result<Task> updateSync(const Task& task);
  Result<Task> updateSync(Task&& task); // 🆕 move overload

  Result<void> removeSync(const QString& id);
  Result<Task> getByIdSync(const QString& id);
  Result<std::vector<Task>> listSync(int offset, int limit);
};

} // namespace tasqly::domain::core

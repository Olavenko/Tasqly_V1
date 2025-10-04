/*
 * 🧱 File: ITaskRepository.h
 * ---------------------------
 * 📌 Purpose   : Abstraction for persistence operations on Task entities.
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Defines the contract for Task persistence. Provides async-ready
 * CRUD operations returning Result<T, Error>. Implementations may
 * be InMemory or database-backed.
 */

#pragma once

#include "domain/core/Result.h"
#include "domain/core/Task.h"

#include <future>
#include <vector>

namespace tasqly::domain::core {

class ITaskRepository
{
public:
  virtual ~ITaskRepository() = default;

  // ➕ Adds a new task to the repository
  virtual std::future<Result<Task>> add(const Task& task) = 0;

  // 🔄 Updates an existing task
  virtual std::future<Result<Task>> update(const Task& task) = 0;

  // ❌ Removes a task by ID
  virtual std::future<Result<void>> remove(const QString& id) = 0;

  // 🔎 Retrieves a task by ID
  virtual std::future<Result<Task>> getById(const QString& id) = 0;

  // 📋 Lists tasks with pagination
  virtual std::future<Result<std::vector<Task>>> list(int limit, int offset) = 0;
};

} // namespace tasqly::domain::core

/*
 * 🧱 File: IGoalRepository.h
 * ---------------------------
 * 📌 Purpose   : Abstraction for persistence operations on Goal entities.
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Defines the contract for Goal persistence. Provides async-ready
 * CRUD operations returning Result<T, Error>. Implementations may
 * be InMemory or database-backed.
 */

#pragma once

#include "Goal.h"
#include "Result.h"

#include <future>
#include <vector>

namespace tasqly::domain::core {

class IGoalRepository
{
public:
  virtual ~IGoalRepository() = default;

  // ➕ Adds a new goal to the repository
  virtual std::future<Result<Goal>> add(const Goal& goal) = 0;

  // 🔄 Updates an existing goal
  virtual std::future<Result<Goal>> update(const Goal& goal) = 0;

  // ❌ Removes a goal by ID
  virtual std::future<Result<void>> remove(const QString& id) = 0;

  // 🔎 Retrieves a goal by ID
  virtual std::future<Result<Goal>> getById(const QString& id) = 0;

  // 📋 Lists goals with pagination
  virtual std::future<Result<std::vector<Goal>>> list(int limit, int offset) = 0;
};

} // namespace tasqly::domain::core

/*
 * 🧱 File: InMemoryGoalRepository.h
 * ---------------------------------
 * 📌 Purpose   : In-memory implementation of IGoalRepository.
 * 🧱 Layer     : Infrastructure (Persistence)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Provides a thread-safe in-memory repository for Goal entities.
 * Supports CRUD operations with optional seed data for testing/demo.
 */

#pragma once

#include "domain/core/Goal.h"
#include "domain/core/IGoalRepository.h"
#include "domain/core/Result.h"

#include <future>
#include <mutex>
#include <vector>

namespace tasqly::domain::core {

class InMemoryGoalRepository : public IGoalRepository
{
public:
  explicit InMemoryGoalRepository(bool seed = false);

  // ➕ Adds a new goal to the repository
  std::future<Result<Goal>> add(const Goal& goal) override; // interface contract
  // 🆕 move overload
  std::future<Result<Goal>> add(Goal&& goal);

  // 🔄 Updates an existing goal
  std::future<Result<Goal>> update(const Goal& goal) override; // interface contract
  // 🆕 move overload
  std::future<Result<Goal>> update(Goal&& goal);

  // ❌ Removes a goal by ID
  std::future<Result<void>> remove(const QString& id) override;

  // 🔎 Retrieves a goal by ID
  std::future<Result<Goal>> getById(const QString& id) override;

  // 📋 Lists goals with pagination
  std::future<Result<std::vector<Goal>>> list(int limit, int offset) override;

  // ➕ Adds a batch of new goals to the repository (bulk insert, perf optimization)
  void addBulk(std::vector<Goal>&& goals);

private:
  // ⚡ Store goals keyed by QString (avoids costly QString→std::string conversion)
  std::unordered_map<std::string, Goal> m_goals;

  mutable std::mutex m_mutex;

  // 👉 Internal synchronous helpers used by async wrappers
  Result<Goal> addSync(const Goal& goal);
  Result<Goal> addSync(Goal&& goal); // 🆕 move overload
  Result<Goal> updateSync(const Goal& goal);
  Result<Goal> updateSync(Goal&& goal); // 🆕 move overload

  Result<void> removeSync(const QString& id);
  Result<Goal> getByIdSync(const QString& id);
  Result<std::vector<Goal>> listSync(int offset, int limit);
};

} // namespace tasqly::domain::core

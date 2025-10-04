/*
 * 🧱 File: InMemoryGoalRepository.cpp
 * ----------------------------------
 * 📌 Purpose   : Implementation of InMemoryGoalRepository methods.
 * 🧱 Layer     : Infrastructure (Persistence)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 * 🗓️ Updated   : 2025-09-23
 * 🔖 Version   : 1.2 (Unified with unordered_map)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Provides a thread-safe in-memory repository for Goal entities.
 * Implements synchronous helpers (*Sync) and wraps them with
 * asynchronous std::future APIs for integration with use cases.
 *
 * 🔗 Depends On:
 *   - domain/core/Goal.h
 *   - domain/core/IGoalRepository.h
 *   - domain/core/Result.h
 * 🔗 Related   : InMemoryGoalRepository.h
 * ✅ Tested In : tests/unit/persistence/test_inmemory_goal_repository.cpp
 * ⚠️ Errors    : Returns Result with RepoErrorCode::NotFound / AlreadyExists
 * ⚡ Performance: O(1) average access (unordered_map + reserved buckets)
 * 🔒 Security  : Thread-safety ensured by std::scoped_lock
 * 🕒 Last Major Change: 2025-09-23
 */

#include "infra/persistence/InMemoryGoalRepository.h"
#include <algorithm>

using namespace tasqly::domain::core;

// 🏗️ Constructor with optional seeding
InMemoryGoalRepository::InMemoryGoalRepository(bool seed)
{
  m_goals.reserve(50000); // ⚡ preallocate ~50k entries (optimize Stress benchmark)
  if (seed) {
    Goal g1{"1",
            "Seed Goal 1",
            "Demo goal",
            QDateTime::currentDateTime(),
            QDateTime::currentDateTime(),
            std::nullopt};

    Goal g2{"2",
            "Seed Goal 2",
            "Demo goal",
            QDateTime::currentDateTime(),
            QDateTime::currentDateTime(),
            std::nullopt};

    m_goals.try_emplace(g1.id.toStdString(), std::move(g1));
    m_goals.try_emplace(g2.id.toStdString(), std::move(g2));
  }
}

// 🆕 Internal synchronous method: add a goal (copy)
Result<Goal> InMemoryGoalRepository::addSync(const Goal& goal)
{
  std::scoped_lock lock(m_mutex);
  auto [it, inserted] = m_goals.try_emplace(goal.id.toStdString(), goal);
  if (!inserted) {
    return err<Goal>(Error{static_cast<int>(RepoErrorCode::AlreadyExists),
                           "Goal already exists",
                           {{"id", goal.id}}});
  }
  return ok(it->second);
}

// 🆕 Internal synchronous method: add a goal (move)
Result<Goal> InMemoryGoalRepository::addSync(Goal&& goal)
{
  std::scoped_lock lock(m_mutex);
  auto [it, inserted] = m_goals.try_emplace(goal.id.toStdString(), std::move(goal));
  if (!inserted) {
    return err<Goal>(Error{static_cast<int>(RepoErrorCode::AlreadyExists),
                           "Goal already exists",
                           {{"id", goal.id}}});
  }
  return ok(it->second);
}

// ➕ Public API: add (const ref → copy)
std::future<Result<Goal>> InMemoryGoalRepository::add(const Goal& goal)
{
  std::promise<Result<Goal>> p;
  auto fut = p.get_future();
  p.set_value(addSync(goal));
  return fut;
}

// ➕ Public API: add (move overload)
std::future<Result<Goal>> InMemoryGoalRepository::add(Goal&& goal)
{
  std::promise<Result<Goal>> p;
  auto fut = p.get_future();
  p.set_value(addSync(std::move(goal)));
  return fut;
}

// 🆕 Internal synchronous method: update (copy)
Result<Goal> InMemoryGoalRepository::updateSync(const Goal& goal)
{
  std::scoped_lock lock(m_mutex);
  auto it = m_goals.find(goal.id.toStdString());
  if (it == m_goals.end()) {
    return err<Goal>(
        Error{static_cast<int>(RepoErrorCode::NotFound), "Goal not found", {{"id", goal.id}}});
  }
  it->second = goal; // copy
  return ok(it->second);
}

// 🆕 Internal synchronous method: update (move)
Result<Goal> InMemoryGoalRepository::updateSync(Goal&& goal)
{
  std::scoped_lock lock(m_mutex);
  auto it = m_goals.find(goal.id.toStdString());
  if (it == m_goals.end()) {
    return err<Goal>(
        Error{static_cast<int>(RepoErrorCode::NotFound), "Goal not found", {{"id", goal.id}}});
  }
  it->second = std::move(goal);
  return ok(it->second);
}

// 🔄 Public API: update (const ref → copy)
std::future<Result<Goal>> InMemoryGoalRepository::update(const Goal& goal)
{
  std::promise<Result<Goal>> p;
  auto fut = p.get_future();
  p.set_value(updateSync(goal));
  return fut;
}

// 🔄 Public API: update (move overload)
std::future<Result<Goal>> InMemoryGoalRepository::update(Goal&& goal)
{
  std::promise<Result<Goal>> p;
  auto fut = p.get_future();
  p.set_value(updateSync(std::move(goal)));
  return fut;
}

// 🆕 Internal synchronous method: remove a goal
Result<void> InMemoryGoalRepository::removeSync(const QString& id)
{
  std::scoped_lock lock(m_mutex);

  auto it = m_goals.find(id.toStdString());
  if (it == m_goals.end()) {
    return err<void>(
        Error{static_cast<int>(RepoErrorCode::NotFound), "Goal not found", {{"id", id}}});
  }
  m_goals.erase(it);
  return ok();
}

// ❌ Public API: remove
std::future<Result<void>> InMemoryGoalRepository::remove(const QString& id)
{
  std::promise<Result<void>> p;
  auto fut = p.get_future();
  p.set_value(removeSync(id));
  return fut;
}

// 🆕 Internal synchronous method: retrieve a goal by ID
Result<Goal> InMemoryGoalRepository::getByIdSync(const QString& id)
{
  std::scoped_lock lock(m_mutex);

  auto it = m_goals.find(id.toStdString());
  if (it == m_goals.end()) {
    return err<Goal>(
        Error{static_cast<int>(RepoErrorCode::NotFound), "Goal not found", {{"id", id}}});
  }
  return ok(it->second);
}

// 🔎 Public API: getById
std::future<Result<Goal>> InMemoryGoalRepository::getById(const QString& id)
{
  std::promise<Result<Goal>> p;
  auto fut = p.get_future();
  p.set_value(getByIdSync(id));
  return fut;
}

// 🆕 Internal synchronous method: list goals with pagination
Result<std::vector<Goal>> InMemoryGoalRepository::listSync(int offset, int limit)
{
  std::scoped_lock lock(m_mutex);

  std::vector<Goal> result;
  result.reserve(std::min(limit, static_cast<int>(m_goals.size())));

  int skipped = 0;
  for (auto it = m_goals.begin(); it != m_goals.end(); ++it) {
    if (skipped++ < offset)
      continue;
    result.emplace_back(it->second); // copy
    if ((int) result.size() >= limit)
      break;
  }

  return ok(std::move(result));
}

// 📋 Public API: list
std::future<Result<std::vector<Goal>>> InMemoryGoalRepository::list(int limit, int offset)
{
  std::promise<Result<std::vector<Goal>>> p;
  auto fut = p.get_future();
  p.set_value(listSync(offset, limit));
  return fut;
}

// ⚡ Bulk Insert: add multiple goals at once with single lock
void InMemoryGoalRepository::addBulk(std::vector<Goal>&& goals)
{
  std::scoped_lock lock(m_mutex);
  for (auto& g : goals) {
    auto key = g.id.toStdString();
    if (m_goals.find(key) == m_goals.end()) {
      m_goals.emplace(std::move(key), std::move(g));
    }
    // ⚠️ If goal already exists, we skip silently (bench seeding case)
  }
}

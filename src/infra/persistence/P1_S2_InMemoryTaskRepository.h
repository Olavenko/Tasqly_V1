#pragma once
/*
 * 🧱 File: P1_S2_InMemoryTaskRepository.h
 * --------------------------------------
 * 📌 Purpose   : In-memory implementation of ITaskRepository for Phase 1 / Slice 2.
 * 🧱 Layer     : Infrastructure (Persistence)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-19
 * 🔖 Version   : 1.1 (Namespace + Naming Fix)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Provides a thread-safe, purely C++ in-memory repository for domain Task entities.
 * Implements the `ITaskRepository` contract defined in the Phase 1 domain layer.
 *
 * - STL-based
 * - Thread-safe (std::mutex)
 * - Deterministic behavior (no async)
 * - Used as fallback or testing repository
 *
 * 🔗 Depends On:
 *   - domain/core/entities/P1_Task.h
 *   - domain/core/contracts/P1_ITaskRepository.h
 *   - domain/core/errors/P1_DomainResult.h
 */

#include "domain/core/contracts/P1_ITaskRepository.h"
#include <mutex>
#include <unordered_map>

using namespace tasqly::p1::s1::domain::core;

namespace tasqly::p1::infra::persistence { // ✅ fixed namespace (removed ::s2)

class P1_S2_InMemoryTaskRepository final : public ITaskRepository // ✅ fixed class name
{
public:
  P1_S2_InMemoryTaskRepository() noexcept = default;
  ~P1_S2_InMemoryTaskRepository() override = default;

  // ------------------------------------------------------------------------
  // 🧩 CRUD Implementation
  // ------------------------------------------------------------------------
  DomainResult<Task> create(const Task& task) override;
  DomainResult<Task> getById(const std::string& id) const override;
  DomainResult<Task> update(const Task& task) override;
  DomainResult<void> remove(const std::string& id) override;
  DomainResult<std::vector<Task>> list(std::optional<TaskStatus> status,
                                       std::optional<TaskPriority> priority,
                                       std::size_t limit,
                                       std::size_t offset) const override;

  // 🧩 Seed deterministic demo data (for dev/test)
  void seedDemoData();

private:
  mutable std::mutex m_mutex;
  std::unordered_map<std::string, Task> m_tasks;
};

} // namespace tasqly::p1::infra::persistence

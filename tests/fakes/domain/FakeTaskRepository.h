/*
 * 🧱 File: FakeTaskRepository.h
 * -----------------------------
 * 📌 Purpose   : In-memory fake implementation of ITaskRepository for testing.
 * 🧱 Layer     : Domain (Fakes / Testing)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-12
 * 🔖 Version   : 1.1 (Namespace Alignment with v1)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Provides an in-memory Task repository for integration and domain tests.
 * Implements all CRUD operations from ITaskRepository using std::vector<Task>.
 * Thread-safe and pure C++ (no Qt, no DB).
 */

#pragma once

#include "domain/core/contracts/P1_ITaskRepository.h"
#include "domain/core/entities/P1_Task.h"
#include "domain/core/entities/P1_TaskPriority.h"
#include "domain/core/entities/P1_TaskStatus.h"
#include "domain/core/errors/P1_DomainResult.h"

#include <mutex>
#include <optional>
#include <vector>

namespace tasqly::domain::core::v1 {

class FakeTaskRepository final : public ITaskRepository
{
public:
  FakeTaskRepository() = default;
  ~FakeTaskRepository() override = default;

  // 📝 Create new task
  DomainResult<Task> create(const Task& task) override;

  // 🔎 Retrieve by ID
  DomainResult<Task> getById(const std::string& id) const override;

  // ✏️ Update existing task
  DomainResult<Task> update(const Task& task) override;

  // 🗑️ Delete by ID
  DomainResult<void> remove(const std::string& id) override;

  // 📋 List & Filter
  DomainResult<std::vector<Task>> list(std::optional<TaskStatus> status,
                                       std::optional<TaskPriority> priority,
                                       std::size_t limit,
                                       std::size_t offset) const override;

  // 🧹 Utility: clear all (for testing)
  void clear();

private:
  mutable std::mutex _mutex;
  std::vector<Task> _tasks;
};

} // namespace tasqly::domain::core::v1

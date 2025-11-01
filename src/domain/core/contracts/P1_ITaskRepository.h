/*
 * 🧱 File: P1_ITaskRepository.h
 * -----------------------------
 * 📌 Purpose   : Domain repository interface for Task CRUD + filtered listing.
 * 🧱 Layer     : Domain (Core) — Contracts (Phase 1)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-09
 * 🔖 Version   : 1.1 (Namespace Isolation Revision)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Defines the *Phase 1* domain repository interface for `P1_Task` entities.
 * This version isolates the Phase 1 domain model from legacy Qt-based entities
 * used in Phase 0 (located in domain/core/Task.h).
 *
 * - Fully decoupled from Qt
 * - Uses modern C++ STL containers and std::optional
 * - Employs DomainResult<T> for consistent error propagation
 * - Designed for both in-memory and persistent implementations
 *
 * 🔗 Depends On:
 *   - domain/core/entities/P1_Task.h
 *   - domain/core/errors/P1_DomainResult.h
 *
 * ✅ Tested In:
 *   - TasqlyTestsRunner (unit/domain/mappers/)
 */

#pragma once

#include <cstddef>  // std::size_t
#include <optional> // std::optional
#include <string>   // std::string
#include <vector>   // std::vector

#include "domain/core/entities/P1_Task.h"
#include "domain/core/errors/P1_DomainResult.h"

//
// 🧭 Namespace Layout
// -------------------
// Legacy code:      tasqly::domain::core
// Phase 1 entities: tasqly::domain::core::v1
//
// All new domain abstractions (repositories, mappers, entities)
// must reside in tasqly::domain::core::v1 to ensure full isolation.
//

namespace tasqly::p1::s1::domain::core {

/// @brief Pure domain repository contract for managing `P1_Task` aggregates.
class ITaskRepository
{
public:
  virtual ~ITaskRepository() = default;

  // ------------------------------------------------------------------------
  // 📝 Create
  // ------------------------------------------------------------------------
  /// @brief Persist a new task (id can be pre-generated in the entity).
  /// @param task Validated P1_Task entity.
  /// @return Result with the created Task (as stored) or Error.
  virtual DomainResult<Task> create(const Task& task) = 0;

  // ------------------------------------------------------------------------
  // 🔎 Read
  // ------------------------------------------------------------------------
  /// @brief Fetch a task by its id.
  /// @param id Task identifier (UUID string).
  /// @return Result with Task if found, otherwise Error (e.g., NOT_FOUND).
  virtual DomainResult<Task> getById(const std::string& id) const = 0;

  // ------------------------------------------------------------------------
  // ✏️ Update
  // ------------------------------------------------------------------------
  /// @brief Update an existing task (matched by task.id).
  /// @param task Task with updated fields; updatedAt is expected to be set by repo.
  /// @return Result with the updated Task or Error.
  virtual DomainResult<Task> update(const Task& task) = 0;

  // ------------------------------------------------------------------------
  // 🗑️ Delete
  // ------------------------------------------------------------------------
  /// @brief Delete a task by id.
  /// @param id Task identifier.
  /// @return Result<void, Error> → success or failure.
  virtual DomainResult<void> remove(const std::string& id) = 0; // (delete is a keyword)

  // ------------------------------------------------------------------------
  // 📋 List / Filter
  // ------------------------------------------------------------------------
  /// @brief List tasks with optional filters + pagination.
  /// @param status   Optional status filter.
  /// @param priority Optional priority filter.
  /// @param limit    Max items to return (server-side cap may apply).
  /// @param offset   Zero-based offset for pagination.
  /// @return Result<vector<Task>, Error> containing the page slice.
  virtual DomainResult<std::vector<Task>> list(std::optional<TaskStatus> status,
                                               std::optional<TaskPriority> priority,
                                               std::size_t limit,
                                               std::size_t offset) const
      = 0;
};

} // namespace tasqly::p1::s1::domain::core

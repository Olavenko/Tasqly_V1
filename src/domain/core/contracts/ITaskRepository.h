/*
 * 🧱 File: ITaskRepository.h
 * --------------------------
 * 📌 Purpose   : Domain repository interface for Task CRUD + filtered listing.
 * 🧱 Layer     : Domain (Core) — Contracts
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-09
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Pure domain contract that defines how Tasks are persisted/retrieved.
 * - No Qt/DB types (pure C++).
 * - Result<T, Error> for all operations to propagate domain-level errors.
 * - Filtered listing by status/priority with pagination (limit, offset).
 *
 * 🔗 Depends On:
 *   - domain/core/entities/Task.h (Task, TaskStatus, TaskPriority)
 * 🔗 Related   : TaskDto, TaskMapper, InMemoryTaskRepository, PostgresTaskRepository
 * ✅ Tested In : TasqlyTestsRunner (Unit & Integration)
 * ⚠️ Errors    : Validation errors, Not Found, Storage failures (via Error)
 * ⚡ Performance: list() should support pagination; O(limit) materialization
 */

#pragma once

#include <cstddef> // std::size_t
#include <optional>
#include <string>
#include <vector>

#include "domain/core/entities/Task.h"
#include "domain/core/errors/DomainResult.h"

// Forward declarations to avoid heavy includes (Result/Error defined in Phase 0)
namespace tasqly::domain::core {
struct Error; // domain error descriptor
template<typename T, typename E>
class Result; // generic result wrapper (e.g., Result<T, Error>)
} // namespace tasqly::domain::core

namespace tasqly::domain::core {

/// @brief Repository contract for Task aggregates (pure domain).
class ITaskRepository
{
public:
  virtual ~ITaskRepository() = default;

  // 📝 Create ---------------------------------------------------------------
  /// @brief Persist a new task (id can be pre-generated in entity).
  /// @param task Validated Task entity.
  /// @return Result with the created Task (as stored) or Error.
  virtual DomainResult<Task> create(const Task& task) = 0;

  // 🔎 Read ---------------------------------------------------------------
  /// @brief Fetch a task by its id.
  /// @param id Task identifier (UUID string).
  /// @return Result with Task if found, otherwise Error (e.g., NOT_FOUND).
  virtual DomainResult<Task> getById(const std::string& id) const = 0;

  // ✏️ Update --------------------------------------------------------------
  /// @brief Update an existing task (matched by task.id).
  /// @param task Task with updated fields; updatedAt is expected to be set by repo.
  /// @return Result with the updated Task or Error.
  virtual DomainResult<Task> update(const Task& task) = 0;

  // 🗑️ Delete --------------------------------------------------------------
  /// @brief Delete a task by id.
  /// @param id Task identifier.
  /// @return Result<bool, Error> → true if a row was deleted, false if none (or Error).
  virtual DomainResult<void> remove(const std::string& id) = 0; // (delete is a keyword)

  // 📋 List / Filter --------------------------------------------------------
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

} // namespace tasqly::domain::core

#pragma once
/*
 * 🧱 File: P1_S2_PostgresTaskRepository.h
 * ---------------------------------------
 * 📌 Purpose   : Implements a PostgreSQL-based repository for Tasks using libpq (Phase 1, Slice 2)
 * 🧱 Layer     : Infrastructure (Persistence / DB)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-21
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Provides a production-ready PostgreSQL implementation of the Task Repository contract.
 * This repository uses parameterized queries exclusively to avoid SQL injection.
 * It integrates with the new runtime subsystems: P1_Logger, P1_Error, and P1_AppSettings.
 *
 * ⚙️ Responsibilities:
 *   - Connect to PostgreSQL via libpq
 *   - Perform CRUD operations on `tasks` table
 *   - Handle errors gracefully (retry once on transient failure)
 *   - Return DomainResult<T> with proper DomainError codes
 *   - Respect runtime settings (from P1_AppSettings)
 *
 * 🔗 Depends On:
 *   - domain/core/v1/P1_DomainResult.h
 *   - infra/runtime/P1_Error.h
 *   - infra/runtime/P1_Logger.h
 *   - infra/runtime/P1_AppSettings.h
 *   - libpq (PostgreSQL C API)
 *
 * 🔒 Security:
 *   - All queries parameterized
 *   - Credentials read from environment (via P1_AppSettings)
 *   - Sensitive values masked in logs
 */

#include <libpq-fe.h>
#include <optional>
#include <string>
#include <vector>

#include "domain/core/errors/P1_DomainResult.h"
#include "infra/runtime/P1_Error.h"

namespace tasqly::p1::infra::db {

// 🧩 Task Entity (simplified version for infra-level mapping)
struct TaskRecord
{
  std::string id;
  std::string title;
  std::string notes;
  int status = 0;
  int priority = 0;
  std::optional<std::string> deadline;
  std::string createdAt;
  std::string updatedAt;
};

// 🧱 PostgreSQL-backed Task Repository
class P1_S2_PostgresTaskRepository final
{
public:
  P1_S2_PostgresTaskRepository();
  ~P1_S2_PostgresTaskRepository();

  // 🔹 Connection lifecycle
  bool connect();
  void disconnect();
  bool isConnected() const noexcept;

  // 🔹 CRUD Operations
  tasqly::p1::s1::domain::core::DomainResult<void> addTask(const TaskRecord& task);
  tasqly::p1::s1::domain::core::DomainResult<void> updateTask(const TaskRecord& task);
  tasqly::p1::s1::domain::core::DomainResult<void> deleteTask(const std::string& id);
  tasqly::p1::s1::domain::core::DomainResult<TaskRecord> getTaskById(const std::string& id);
  tasqly::p1::s1::domain::core::DomainResult<std::vector<TaskRecord>> listTasks();

  // 🧩 Retrieve last runtime error (if any)
  const tasqly::p1::infra::runtime::P1_Error& lastError() const noexcept { return lastError_; }

private:
  // 🧩 Helpers
  tasqly::p1::s1::domain::core::DomainResult<void> execCommand(
      const std::string& sql, const std::vector<std::string>& params);
  tasqly::p1::s1::domain::core::DomainResult<std::vector<TaskRecord>> queryTasks(
      const std::string& sql, const std::vector<std::string>& params);

  // 🔒 Connection handle
  PGconn* m_conn = nullptr;

  // 🧠 Internal state
  bool m_connected = false;

  // 🧩 Utility
  runtime::P1_Error handlePgError(const std::string& context);
  void logQuery(const std::string& sql, const std::vector<std::string>& params) const;

  // 🧩 Tracks last runtime error for fallback or diagnostics
  tasqly::p1::infra::runtime::P1_Error lastError_;

  // 🧩 Execute SQL safely with retry and structured logging
  bool executeQuery(const std::string& sql, const std::vector<std::string>& params);
};

} // namespace tasqly::p1::infra::db

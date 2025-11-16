#pragma once

/*
 * 🧱 File: P1_S2_DbTaskRepository.h
 * ---------------------------------
 * 📌 Purpose   : Database-backed implementation of ITaskRepository (uses IDbConnection)
 * 🧱 Layer     : Infrastructure → Persistence
 * 🎯 Slice     : Phase 1 — Slice 2.5
 */

#include <optional>
#include <string>
#include <vector>

#include "domain/core/contracts/P1_ITaskRepository.h"
#include "domain/core/entities/P1_Task.h"
#include "domain/core/errors/P1_DomainResult.h"

#include "infra/db/P1_S2_IDbConnection.h"
#include "infra/runtime/P1_Logger.h"

namespace tasqly::p1::s2::infra::persistence {

using tasqly::p1::s1::domain::core::DomainError;
using tasqly::p1::s1::domain::core::DomainResult;
using tasqly::p1::s1::domain::core::Task;
using tasqly::p1::s1::domain::core::TaskPriority;
using tasqly::p1::s1::domain::core::TaskStatus;

using tasqly::p1::s2::infra::db::DbQueryResult;
using tasqly::p1::s2::infra::db::DbResult;
using tasqly::p1::s2::infra::db::IDbConnection;

using tasqly::p1::infra::runtime::P1_Logger;

class P1_S2_DbTaskRepository : public tasqly::p1::s1::domain::core::ITaskRepository
{
public:
  explicit P1_S2_DbTaskRepository(IDbConnection* conn, P1_Logger* logger);

  // ---------------------------------------------------------
  // CRUD (matching EXACT domain contract)
  // ---------------------------------------------------------

  // CREATE
  DomainResult<Task> create(const Task& task) override;

  // READ
  DomainResult<Task> getById(const std::string& id) const override;

  // UPDATE
  DomainResult<Task> update(const Task& task) override;

  // DELETE
  DomainResult<void> remove(const std::string& id) override;

  // LIST
  DomainResult<std::vector<Task>> list(std::optional<TaskStatus> status,
                                       std::optional<TaskPriority> priority,
                                       std::size_t limit,
                                       std::size_t offset) const override;

private:
  // Helpers
  DomainError mapDbError(const DbResult& db) const;

  DomainResult<Task> mapSingleRow(const DbQueryResult& qr) const;
  DomainResult<std::vector<Task>> mapRows(const DbQueryResult& qr) const;

private:
  IDbConnection* m_conn = nullptr;
  P1_Logger* m_logger = nullptr;
};

} // namespace tasqly::p1::s2::infra::persistence

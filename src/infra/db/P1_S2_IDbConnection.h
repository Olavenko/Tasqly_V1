#pragma once
#pragma once

/*
 * 🧱 File: P1_S2_IDbConnection.h
 * ------------------------------
 * 📌 Purpose   : Abstract database connection interface for Slice 2.5.
 * 🧱 Layer     : Infrastructure (DB)
 * 🎯 Slice     : Phase 1 — Slice 2.5 (DB Abstraction)
 *
 * 📄 Notes:
 * - This interface defines the minimal DB access contract.
 * - Implemented by PostgresConnection + FakeDbConnection.
 * - Uses Result<T> (header-only) for safe value-or-error communication.
 */

#include <string>
#include <vector>

#include "infra/runtime/P1_S2_Result.h"

namespace tasqly::p1::s2::infra::db {

// ---------------------------------------------------------
//  📦 DbQueryResult — unified DB result for all backends
// ---------------------------------------------------------
struct DbQueryResult
{
  std::vector<std::string> columns;
  std::vector<std::vector<std::string>> rows;
};

// Alias for convenience
using DbResult = tasqly::p1::s2::infra::runtime::Result<DbQueryResult>;

// ---------------------------------------------------------
//  🔌 IDbConnection — minimal DB abstraction
// ---------------------------------------------------------
class IDbConnection
{
public:
  virtual ~IDbConnection() = default;

  // 📝 Execute SQL (parameterized or raw)
  virtual DbResult execute(const std::string& sql) = 0;

  // 🔄 Attempt to recover connection (retry / PQreset / etc.)
  virtual bool reset() = 0;

  // 🧪 Health check (is connection ready?)
  virtual bool isValid() const = 0;
};

} // namespace tasqly::p1::s2::infra::db

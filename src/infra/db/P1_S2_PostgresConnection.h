#pragma once

/*
 * 🧱 File: P1_S2_PostgresConnection.h
 * -----------------------------------
 * 📌 Purpose   : Concrete implementation of IDbConnection using libpq.
 * 🧱 Layer     : Infrastructure (DB)
 * 🎯 Slice     : Phase 1 — Slice 2.5 (DB Abstraction)
 *
 * 📄 Notes:
 * - Provides safe RAII for PGconn.
 * - Supports retry policy: fail → reset → retry once.
 * - Masks sensitive data in logs (password, etc.).
 * - Maps PGresult to DbQueryResult (columns + rows).
 */

#include "infra/db/P1_S2_IDbConnection.h"
#include "infra/runtime/P1_Logger.h"
#include <string>

namespace tasqly::p1::s2::infra::db {

using Logger = tasqly::p1::infra::runtime::P1_Logger;

// ---------------------------------------------------------
//  libpq abstraction — allows deterministic unit testing.
// ---------------------------------------------------------
class ILibpqApi
{
public:
  virtual ~ILibpqApi() = default;

  virtual void* connect(const std::string& connectionString) = 0;
  virtual void finish(void* conn) = 0;
  virtual int status(void* conn) = 0;
  virtual void reset(void* conn) = 0;

  virtual void* exec(void* conn, const std::string& sql) = 0;
  virtual int resultStatus(void* res) = 0;
  virtual const char* resultErrorMessage(void* res) = 0;

  virtual int nfields(void* res) = 0;
  virtual const char* fname(void* res, int column) = 0;
  virtual int ntuples(void* res) = 0;
  virtual bool getisnull(void* res, int row, int column) = 0;
  virtual const char* getvalue(void* res, int row, int column) = 0;

  virtual void clear(void* res) = 0;
};

class P1_S2_PostgresConnection : public IDbConnection
{
public:
  explicit P1_S2_PostgresConnection(const std::string& connectionString,
                                    Logger* logger,
                                    ILibpqApi* api = nullptr);

  ~P1_S2_PostgresConnection() override;

  DbResult execute(const std::string& sql) override;
  bool reset() override;
  bool isValid() const override;

private:
  std::string maskConnectionString(const std::string& raw) const;

  void logInfo(const std::string& msg) const;
  void logWarn(const std::string& msg) const;
  void logError(const std::string& msg) const;

  DbResult mapResult(void* res);
  DbResult executeWithRetry(const std::string& sql);

private:
  std::string m_rawConnStr;
  std::string m_maskedConnStr;

  void* m_conn = nullptr; // REAL TYPE ONLY IN CPP
  Logger* m_logger = nullptr;
  ILibpqApi* m_pgApi = nullptr; // non-owning
};

} // namespace tasqly::p1::s2::infra::db

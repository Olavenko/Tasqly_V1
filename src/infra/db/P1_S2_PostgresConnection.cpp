/*
 * 🧱 File: P1_S2_PostgresConnection.cpp
 * -------------------------------------
 * 📌 Purpose   : Concrete implementation of IDbConnection using libpq.
 * 🧱 Layer     : Infrastructure (DB)
 * 🎯 Slice     : Phase 1 — Slice 2.5 (DB Abstraction)
 */

#include "P1_S2_PostgresConnection.h"
#include <libpq-fe.h>

namespace tasqly::p1::s2::infra::db {

using Logger = tasqly::p1::infra::runtime::P1_Logger;
using InfraError = tasqly::p1::infra::runtime::P1_Error;

// ---------------------------------------------------------
//  Cast helpers (void* <-> PGconn / PGresult)
// ---------------------------------------------------------
static inline PGconn* asConn(void* p)
{
  return reinterpret_cast<PGconn*>(p);
}

static inline PGresult* asResult(void* p)
{
  return reinterpret_cast<PGresult*>(p);
}

static inline void* toVoid(PGconn* p)
{
  return reinterpret_cast<void*>(p);
}

// ---------------------------------------------------------
//  Constructor
// ---------------------------------------------------------
P1_S2_PostgresConnection::P1_S2_PostgresConnection(const std::string& connectionString,
                                                   Logger* logger)
    : m_rawConnStr(connectionString)
    , m_logger(logger)
{
  m_maskedConnStr = maskConnectionString(m_rawConnStr);

  logInfo("PostgresConnection: initializing connection");

  PGconn* real = ::PQconnectdb(m_rawConnStr.c_str());
  m_conn = toVoid(real);

  if (!real || ::PQstatus(real) != CONNECTION_OK) {
    logError("PostgresConnection: initial connection failed");
  } else {
    logInfo("PostgresConnection: initial connection established");
  }
}

// ---------------------------------------------------------
//  Destructor (RAII)
// ---------------------------------------------------------
P1_S2_PostgresConnection::~P1_S2_PostgresConnection()
{
  if (m_conn) {
    PGconn* real = asConn(m_conn);
    ::PQfinish(real);
    m_conn = nullptr;
  }
}

// ---------------------------------------------------------
//  IDbConnection::execute()
// ---------------------------------------------------------
DbResult P1_S2_PostgresConnection::execute(const std::string& sql)
{
  return executeWithRetry(sql);
}

// ---------------------------------------------------------
//  IDbConnection::reset()
// ---------------------------------------------------------
bool P1_S2_PostgresConnection::reset()
{
  if (!m_conn) {
    logWarn("PostgresConnection: reset called but connection is null");
    return false;
  }

  PGconn* real = asConn(m_conn);

  logWarn("PostgresConnection: attempting PQreset()");
  ::PQreset(real);

  if (::PQstatus(real) == CONNECTION_OK) {
    logInfo("PostgresConnection: reset succeeded");
    return true;
  }

  logError("PostgresConnection: reset failed");
  return false;
}

// ---------------------------------------------------------
//  IDbConnection::isValid()
// ---------------------------------------------------------
bool P1_S2_PostgresConnection::isValid() const
{
  if (!m_conn)
    return false;
  const PGconn* real = reinterpret_cast<const PGconn*>(m_conn);
  return ::PQstatus(real) == CONNECTION_OK;
}

// ---------------------------------------------------------
//  Mask sensitive values
// ---------------------------------------------------------
std::string P1_S2_PostgresConnection::maskConnectionString(const std::string& raw) const
{
  std::string masked = raw;

  const std::string key = "password=";
  std::size_t pos = masked.find(key);
  if (pos != std::string::npos) {
    std::size_t end = masked.find(' ', pos);
    if (end == std::string::npos)
      end = masked.size();
    masked.replace(pos + key.size(), end - (pos + key.size()), "******");
  }
  return masked;
}

// ---------------------------------------------------------
//  Logging helpers
// ---------------------------------------------------------
void P1_S2_PostgresConnection::logInfo(const std::string& msg) const
{
  if (m_logger)
    m_logger->info(msg);
}

void P1_S2_PostgresConnection::logWarn(const std::string& msg) const
{
  if (m_logger)
    m_logger->warn(msg);
}

void P1_S2_PostgresConnection::logError(const std::string& msg) const
{
  if (m_logger)
    m_logger->error(msg);
}

// ---------------------------------------------------------
//  Mapping PGresult → DbQueryResult
// ---------------------------------------------------------
DbResult P1_S2_PostgresConnection::mapResult(void* resOpaque)
{
  PGresult* res = asResult(resOpaque);

  if (!res) {
    return DbResult::Err(InfraError::makeIo("Postgres: null PGresult"));
  }

  ExecStatusType status = ::PQresultStatus(res);

  if (status == PGRES_COMMAND_OK) {
    DbQueryResult empty;
    return DbResult::Ok(empty);
  }

  if (status != PGRES_TUPLES_OK) {
    std::string err = ::PQresultErrorMessage(res);
    return DbResult::Err(InfraError::makeIo("Postgres: " + err));
  }

  int cols = ::PQnfields(res);
  int rows = ::PQntuples(res);

  DbQueryResult out;
  out.columns.reserve(cols);

  for (int c = 0; c < cols; ++c) {
    out.columns.emplace_back(::PQfname(res, c));
  }

  out.rows.reserve(rows);
  for (int r = 0; r < rows; ++r) {
    std::vector<std::string> row;
    row.reserve(cols);
    for (int c = 0; c < cols; ++c) {
      if (::PQgetisnull(res, r, c))
        row.emplace_back("");
      else
        row.emplace_back(::PQgetvalue(res, r, c));
    }
    out.rows.push_back(std::move(row));
  }

  return DbResult::Ok(out);
}

// ---------------------------------------------------------
//  Retry Logic
// ---------------------------------------------------------
DbResult P1_S2_PostgresConnection::executeWithRetry(const std::string& sql)
{
  if (!m_conn) {
    logError("PostgresConnection: execute called with null connection");
    return DbResult::Err(InfraError::makeDbInit("Postgres: null connection"));
  }

  logInfo("PostgresConnection: executing SQL");

  PGconn* real = asConn(m_conn);

  PGresult* res = ::PQexec(real, sql.c_str());
  bool fatal = (!res || ::PQresultStatus(res) == PGRES_FATAL_ERROR);

  if (fatal) {
    if (res)
      ::PQclear(res);

    logWarn("PostgresConnection: execute failed, attempting retry");

    if (!reset()) {
      logError("PostgresConnection: retry abandoned (reset failed)");
      return DbResult::Err(InfraError::makeDbInit("Postgres: retry reset failed"));
    }

    real = asConn(m_conn);
    res = ::PQexec(real, sql.c_str());

    if (!res || ::PQresultStatus(res) == PGRES_FATAL_ERROR) {
      if (res)
        ::PQclear(res);
      return DbResult::Err(InfraError::makeIo("Postgres: query failed after retry"));
    }
  }

  DbResult mapped = mapResult(res);
  ::PQclear(res);
  return mapped;
}

} // namespace tasqly::p1::s2::infra::db

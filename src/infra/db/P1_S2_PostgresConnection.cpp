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

namespace {

// ---------------------------------------------------------
//  Cast helpers (void* <-> PGconn / PGresult)
// ---------------------------------------------------------
inline PGconn* asConn(void* p)
{
  return reinterpret_cast<PGconn*>(p);
}

inline PGresult* asResult(void* p)
{
  return reinterpret_cast<PGresult*>(p);
}

inline void* toVoid(PGconn* p)
{
  return reinterpret_cast<void*>(p);
}

inline void* toVoid(PGresult* p)
{
  return reinterpret_cast<void*>(p);
}

// ---------------------------------------------------------
//  Real libpq bridge
// ---------------------------------------------------------
class RealLibpqApi : public ILibpqApi
{
public:
  void* connect(const std::string& connectionString) override
  {
    return toVoid(::PQconnectdb(connectionString.c_str()));
  }

  void finish(void* conn) override
  {
    if (auto* real = asConn(conn))
      ::PQfinish(real);
  }

  int status(void* conn) override
  {
    if (auto* real = asConn(conn))
      return static_cast<int>(::PQstatus(real));
    return CONNECTION_BAD;
  }

  void reset(void* conn) override
  {
    if (auto* real = asConn(conn))
      ::PQreset(real);
  }

  void* exec(void* conn, const std::string& sql) override
  {
    if (auto* real = asConn(conn))
      return toVoid(::PQexec(real, sql.c_str()));
    return nullptr;
  }

  int resultStatus(void* res) override
  {
    if (auto* real = asResult(res))
      return static_cast<int>(::PQresultStatus(real));
    return PGRES_FATAL_ERROR;
  }

  const char* resultErrorMessage(void* res) override
  {
    if (auto* real = asResult(res))
      return ::PQresultErrorMessage(real);
    return "";
  }

  int nfields(void* res) override
  {
    if (auto* real = asResult(res))
      return ::PQnfields(real);
    return 0;
  }

  const char* fname(void* res, int column) override
  {
    if (auto* real = asResult(res))
      return ::PQfname(real, column);
    return "";
  }

  int ntuples(void* res) override
  {
    if (auto* real = asResult(res))
      return ::PQntuples(real);
    return 0;
  }

  bool getisnull(void* res, int row, int column) override
  {
    if (auto* real = asResult(res))
      return ::PQgetisnull(real, row, column) != 0;
    return true;
  }

  const char* getvalue(void* res, int row, int column) override
  {
    if (auto* real = asResult(res))
      return ::PQgetvalue(real, row, column);
    return "";
  }

  void clear(void* res) override
  {
    if (auto* real = asResult(res))
      ::PQclear(real);
  }
};

ILibpqApi* resolveLibpqApi(ILibpqApi* provided)
{
  static RealLibpqApi api;
  return provided ? provided : &api;
}

} // namespace

// ---------------------------------------------------------
//  Constructor
// ---------------------------------------------------------
P1_S2_PostgresConnection::P1_S2_PostgresConnection(const std::string& connectionString,
                                                   Logger* logger,
                                                   ILibpqApi* api)
    : m_rawConnStr(connectionString)
    , m_logger(logger)
    , m_pgApi(resolveLibpqApi(api))
{
  m_maskedConnStr = maskConnectionString(m_rawConnStr);

  logInfo("PostgresConnection: initializing connection");

  m_conn = m_pgApi->connect(m_rawConnStr);

  if (!m_conn || m_pgApi->status(m_conn) != CONNECTION_OK) {
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
    m_pgApi->finish(m_conn);
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

  logWarn("PostgresConnection: attempting PQreset()");
  m_pgApi->reset(m_conn);

  if (m_pgApi->status(m_conn) == CONNECTION_OK) {
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
  return m_pgApi->status(m_conn) == CONNECTION_OK;
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
  if (!resOpaque) {
    return DbResult::Err(InfraError::makeIo("Postgres: null PGresult"));
  }

  ExecStatusType status = static_cast<ExecStatusType>(m_pgApi->resultStatus(resOpaque));

  if (status == PGRES_COMMAND_OK) {
    DbQueryResult empty;
    return DbResult::Ok(empty);
  }

  if (status != PGRES_TUPLES_OK) {
    const char* errMsg = m_pgApi->resultErrorMessage(resOpaque);
    std::string err = errMsg ? errMsg : "";
    return DbResult::Err(InfraError::makeIo("Postgres: " + err));
  }

  int cols = m_pgApi->nfields(resOpaque);
  int rows = m_pgApi->ntuples(resOpaque);

  DbQueryResult out;
  out.columns.reserve(cols);

  for (int c = 0; c < cols; ++c) {
    const char* columnName = m_pgApi->fname(resOpaque, c);
    out.columns.emplace_back(columnName ? columnName : "");
  }

  out.rows.reserve(rows);
  for (int r = 0; r < rows; ++r) {
    std::vector<std::string> row;
    row.reserve(cols);
    for (int c = 0; c < cols; ++c) {
      if (m_pgApi->getisnull(resOpaque, r, c))
        row.emplace_back("");
      else
        row.emplace_back(m_pgApi->getvalue(resOpaque, r, c));
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

  void* res = m_pgApi->exec(m_conn, sql);
  bool fatal = (!res || m_pgApi->resultStatus(res) == PGRES_FATAL_ERROR);

  if (fatal) {
    if (res)
      m_pgApi->clear(res);

    logWarn("PostgresConnection: execute failed, attempting retry");

    if (!reset()) {
      logError("PostgresConnection: retry abandoned (reset failed)");
      return DbResult::Err(InfraError::makeDbInit("Postgres: retry reset failed"));
    }

    res = m_pgApi->exec(m_conn, sql);

    if (!res || m_pgApi->resultStatus(res) == PGRES_FATAL_ERROR) {
      if (res)
        m_pgApi->clear(res);
      return DbResult::Err(InfraError::makeIo("Postgres: query failed after retry"));
    }
  }

  DbResult mapped = mapResult(res);
  m_pgApi->clear(res);
  return mapped;
}

} // namespace tasqly::p1::s2::infra::db

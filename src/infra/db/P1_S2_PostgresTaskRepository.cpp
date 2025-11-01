/*
 * 🧱 File: P1_S2_PostgresTaskRepository.cpp
 * ----------------------------------------
 * 📌 Purpose   : Implementation of PostgreSQL Task Repository using libpq (Phase 1 - Slice 2)
 * 🧱 Layer     : Infrastructure (Persistence / DB)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-21
 * 🔖 Version   : 1.0
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Implements all CRUD operations for the `tasks` table using libpq in a safe, parameterized manner.
 * Logs every operation through P1_Logger, and converts PostgreSQL errors into P1_Error/DomainResult.
 * The connection parameters are read from environment variables via P1_AppSettings.
 *
 * 🔒 Security:
 *   - No string concatenation (parameter binding only)
 *   - Credentials masked in logs
 */

#include "infra/db/P1_S2_PostgresTaskRepository.h"
#include "domain/core/entities/P1_TaskPriority.h"
#include "domain/core/entities/P1_TaskStatus.h"
#include "infra/runtime/P1_AppSettings.h"
#include "infra/runtime/P1_Error.h"
#include "infra/runtime/P1_Logger.h"

#include <algorithm>
#include <sstream>
#include <thread>

namespace tasqly::p1::infra::db {
using namespace tasqly::p1::s1::domain::core;
using namespace tasqly::p1::infra::runtime;

// 🧱 Constructor
P1_S2_PostgresTaskRepository::P1_S2_PostgresTaskRepository()
{
  connect();
}

// 🧱 Destructor
P1_S2_PostgresTaskRepository::~P1_S2_PostgresTaskRepository()
{
  disconnect();
}

// 🧩 Connect to PostgreSQL (with retry, timeout & classified error handling)
bool P1_S2_PostgresTaskRepository::connect()
{
  if (m_connected)
    return true;

  auto& settings = P1_AppSettings::instance();
  const int connectTimeout = settings.getInt("DB_CONNECT_TIMEOUT", 5);
  const int MAX_RETRIES = 1;

  std::string host = settings.getString("DB_HOST").value_or("localhost");
  std::string port = settings.getString("DB_PORT").value_or("5432");
  std::string db = settings.getString("DB_NAME").value_or("tasqly");
  std::string user = settings.getString("DB_USER").value_or("postgres");
  std::string pass = settings.getString("DB_PASS").value_or("");

  // ✅ keep timeout string alive during function scope
  std::string timeoutStr = std::to_string(connectTimeout);

  const char* keys[] = {"host", "port", "dbname", "user", "password", "connect_timeout", nullptr};
  const char* values[] = {host.c_str(),
                          port.c_str(),
                          db.c_str(),
                          user.c_str(),
                          pass.c_str(),
                          timeoutStr.c_str(),
                          nullptr};

  int attempt = 0;
  while (attempt <= MAX_RETRIES) {
    P1_Logger::instance().info("[DB] Connecting to PostgreSQL (attempt "
                               + std::to_string(attempt + 1) + ")...");

    m_conn = PQconnectdbParams(keys, values, 0);

    if (PQstatus(m_conn) == CONNECTION_OK) {
      m_connected = true;
      P1_Logger::instance().info("[DB] Connected successfully to " + host + ":" + port);
      return true;
    }

    // 🧩 Extract error message and classify
    std::string errMsg = PQerrorMessage(m_conn);
    std::string lowerErr = errMsg;
    std::transform(lowerErr.begin(), lowerErr.end(), lowerErr.begin(), ::tolower);

    P1_Error err;
    if (lowerErr.find("password") != std::string::npos
        || lowerErr.find("authentication") != std::string::npos) {
      err = P1_Error::makeAuth(errMsg);
    } else if (lowerErr.find("timeout") != std::string::npos) {
      err = P1_Error::makeTimeout(errMsg);
    } else {
      err = P1_Error::makeDbInit(errMsg);
    }

    // 🧭 Log structured error (masked) & store for fallback decision
    P1_Logger::instance().error(err);
    lastError_ = err;

    // Legacy warning log for context
    P1_Logger::instance().warn("[DB] Connection attempt failed: " + errMsg);

    PQfinish(m_conn);
    m_conn = nullptr;
    ++attempt;

    if (attempt <= MAX_RETRIES) {
      std::this_thread::sleep_for(std::chrono::milliseconds(500)); // short backoff
      P1_Logger::instance().info("[DB] Retrying connection...");
    }
  }

  // ❌ Final failure — log and emit structured error
  P1_Error err = P1_Error::makeDbInit("Failed to connect after retry");
  P1_Logger::instance().error(err);
  lastError_ = err;
  m_connected = false;
  return false;
}

// 🧩 Execute SQL query safely with parameter binding, health check, and structured error logging
bool P1_S2_PostgresTaskRepository::executeQuery(const std::string& sql,
                                                const std::vector<std::string>& params)
{
  // 🧱 Step 1 — Validate connection
  if (!m_conn) {
    P1_Error err = P1_Error::makeDbInit("[Postgres] No active connection");
    P1_Logger::instance().error(err);
    lastError_ = err;
    return false;
  }

  // 🩺 Step 2 — Check connection health
  if (PQstatus(m_conn) != CONNECTION_OK) {
    P1_Logger::instance().warn("[DB] Lost connection detected — attempting reset...");
    PQreset(m_conn);

    if (PQstatus(m_conn) != CONNECTION_OK) {
      P1_Error err = P1_Error::makeTimeout("[Postgres] Connection reset failed: "
                                           + std::string(PQerrorMessage(m_conn)));
      P1_Logger::instance().error(err);
      lastError_ = err;
      return false;
    }
  }

  // ⚙️ Step 3 — Prepare parameter bindings
  std::vector<const char*> values;
  values.reserve(params.size());
  for (const auto& p : params)
    values.push_back(p.c_str());

  // 🧾 Log query before execution
  logQuery(sql, params);

  // 🧩 Step 4 — Execute the query
  PGresult* res = PQexecParams(m_conn,
                               sql.c_str(),
                               static_cast<int>(params.size()),
                               nullptr,
                               values.data(),
                               nullptr,
                               nullptr,
                               0);

  // 🧠 Step 5 — Validate the result
  auto status = PQresultStatus(res);
  if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
    // Delegate classification to shared handler
    P1_Error err = handlePgError("executeQuery");

    // Add contextual info for debugging
    P1_Logger::instance().error("[DB][executeQuery] SQL failed: " + sql
                                + " | error: " + PQerrorMessage(m_conn));

    PQclear(res);
    lastError_ = err;
    return false;
  }

  PQclear(res);
  return true;
}

// 🧩 Disconnect safely
void P1_S2_PostgresTaskRepository::disconnect()
{
  if (m_conn) {
    PQfinish(m_conn);
    m_conn = nullptr;
  }
  m_connected = false;
  P1_Logger::instance().info("[DB] Connection closed.");
}

bool P1_S2_PostgresTaskRepository::isConnected() const noexcept
{
  return m_connected && m_conn != nullptr;
}

// 🧱 Add task (NULL-safe + no dangling pointers)
DomainResult<void> P1_S2_PostgresTaskRepository::addTask(const TaskRecord& t)
{
  if (!isConnected())
    return DomainResult<void>::err(
        P1_Error::makeDbInit("Not connected to PostgreSQL").toDomainError());

  // 🧩 SQL command
  std::string sql
      = "INSERT INTO tasks (id, title, notes, status, priority, deadline, created_at, updated_at) "
        "VALUES ($1, $2, $3, $4, $5, $6, $7, $8)";

  // 🧩 Keep local storage for domain → string conversions (text-based enums)
  // 📝 Convert enum values to textual DB representation ("Todo", "Normal", etc.)
  std::string statusStr = toString(static_cast<TaskStatus>(t.status));
  std::string priorityStr = toString(static_cast<TaskPriority>(t.priority));

  // 🧩 Build parameter array (NULL-safe)
  const char* values[] = {t.id.c_str(),
                          t.title.c_str(),
                          t.notes.empty() ? nullptr : t.notes.c_str(),
                          statusStr.c_str(),
                          priorityStr.c_str(),
                          t.deadline.has_value() ? t.deadline->c_str() : nullptr,
                          t.createdAt.c_str(),
                          t.updatedAt.c_str()};

  logQuery(sql, {t.id, t.title});

  // 🧩 Execute query safely
  PGresult* res = PQexecParams(m_conn,
                               sql.c_str(),
                               static_cast<int>(std::size(values)),
                               nullptr, // param types
                               values,
                               nullptr, // param lengths
                               nullptr, // param formats
                               0        // text mode
  );

  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    lastError_ = P1_Error::makeDbInit(PQerrorMessage(m_conn));
    PQclear(res);
    return DomainResult<void>::err(lastError_.toDomainError());
  }

  PQclear(res);
  return DomainResult<void>::ok();
}

// 🧱 Update task (NULL-safe + no dangling pointers)
DomainResult<void> P1_S2_PostgresTaskRepository::updateTask(const TaskRecord& t)
{
  if (!isConnected())
    return DomainResult<void>::err(
        P1_Error::makeDbInit("Not connected to PostgreSQL").toDomainError());

  // 🧩 SQL command
  std::string sql = "UPDATE tasks "
                    "SET title=$2, notes=$3, status=$4, priority=$5, deadline=$6, updated_at=$7 "
                    "WHERE id=$1";

  // 🧩 Keep local storage for domain → string conversions (text-based enums)
  std::string statusStr = toString(static_cast<TaskStatus>(t.status));
  std::string priorityStr = toString(static_cast<TaskPriority>(t.priority));

  // 🧩 Build NULL-safe parameters array
  const char* values[] = {t.id.c_str(),
                          t.title.c_str(),
                          t.notes.empty() ? nullptr : t.notes.c_str(),
                          statusStr.c_str(),
                          priorityStr.c_str(),
                          t.deadline.has_value() ? t.deadline->c_str() : nullptr,
                          t.updatedAt.c_str()};

  logQuery(sql, {t.id, t.title});

  PGresult* res = PQexecParams(m_conn,
                               sql.c_str(),
                               static_cast<int>(std::size(values)),
                               nullptr,
                               values,
                               nullptr,
                               nullptr,
                               0);

  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    lastError_ = P1_Error::makeDbInit(PQerrorMessage(m_conn));
    PQclear(res);
    return DomainResult<void>::err(lastError_.toDomainError());
  }

  // 🧩 Ensure at least one row was updated (detect non-existing ID)
  std::string affectedStr = PQcmdTuples(res);
  int affected = affectedStr.empty() ? 0 : std::stoi(affectedStr);

  if (affected == 0) {
    PQclear(res);
    DomainError err = DomainError::makeNotFound("Task not found: " + t.id);
    return DomainResult<void>::err(err);
  }

  PQclear(res);
  return DomainResult<void>::ok();
}

// 🧱 Delete task (NULL-safe, unified with PQexecParams)
DomainResult<void> P1_S2_PostgresTaskRepository::deleteTask(const std::string& id)
{
  if (!isConnected())
    return DomainResult<void>::err(
        P1_Error::makeDbInit("Not connected to PostgreSQL").toDomainError());

  // 🧩 SQL command
  std::string sql = "DELETE FROM tasks WHERE id=$1";

  // 🧩 Prepare parameter array
  const char* values[] = {id.c_str()};

  logQuery(sql, {id});

  // 🧩 Execute SQL safely
  PGresult* res = PQexecParams(m_conn,
                               sql.c_str(),
                               1,       // parameter count
                               nullptr, // param types
                               values,  // parameter values
                               nullptr, // param lengths
                               nullptr, // param formats
                               0        // text mode
  );

  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    lastError_ = P1_Error::makeDbInit(PQerrorMessage(m_conn));
    PQclear(res);
    return DomainResult<void>::err(lastError_.toDomainError());
  }

  PQclear(res);
  return DomainResult<void>::ok();
}

// 🧱 Get task by ID (final version - type-safe with DTO conversion)
DomainResult<TaskRecord> P1_S2_PostgresTaskRepository::getTaskById(const std::string& id)
{
  using namespace tasqly::p1::s1::domain::core;
  P1_Logger::instance().info("[TEST] getTaskById() called with id=" + id);

  if (!isConnected())
    return DomainResult<TaskRecord>::err(DomainError::makeStorage("Not connected to PostgreSQL"));

  // 🧩 SQL query
  std::string sql = "SELECT id, title, notes, status, priority, deadline, created_at, updated_at "
                    "FROM tasks WHERE id=$1";

  const char* values[] = {id.c_str()};
  logQuery(sql, {id});

  PGresult* res = PQexecParams(m_conn, sql.c_str(), 1, nullptr, values, nullptr, nullptr, 0);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    std::string msg = "PostgreSQL SELECT failed: ";
    msg += PQerrorMessage(m_conn);
    PQclear(res);
    return DomainResult<TaskRecord>::err(DomainError::makeStorage(msg));
  }

  if (PQntuples(res) == 0) {
    PQclear(res);
    return DomainResult<TaskRecord>::err(DomainError::makeNotFound("Task not found: " + id));
  }

  // 🧱 Map DB row to TaskRecord safely
  TaskRecord record;
  record.id = PQgetvalue(res, 0, 0);
  record.title = PQgetvalue(res, 0, 1);

  // Optional notes
  record.notes = PQgetisnull(res, 0, 2) ? std::string() : std::string(PQgetvalue(res, 0, 2));

  // ✅ Parse status & priority safely (string → enum → int)
  std::string statusStr = PQgetvalue(res, 0, 3);
  std::string priorityStr = PQgetvalue(res, 0, 4);

  auto statusOpt = taskStatusFromString(statusStr);
  if (!statusOpt) {
    PQclear(res);
    return DomainResult<TaskRecord>::err(
        DomainError::makeValidation("Invalid task status value: " + statusStr));
  }
  record.status = static_cast<int>(*statusOpt);

  auto priorityOpt = taskPriorityFromString(priorityStr);
  if (!priorityOpt) {
    PQclear(res);
    return DomainResult<TaskRecord>::err(
        DomainError::makeValidation("Invalid task priority value: " + priorityStr));
  }
  record.priority = static_cast<int>(*priorityOpt);

  // Optional deadline
  if (!PQgetisnull(res, 0, 5))
    record.deadline = std::make_optional<std::string>(PQgetvalue(res, 0, 5));
  else
    record.deadline = std::nullopt;

  // Timestamps
  record.createdAt = PQgetvalue(res, 0, 6);
  record.updatedAt = PQgetvalue(res, 0, 7);

  PQclear(res);
  return DomainResult<TaskRecord>::ok(record);
}

// 🧱 List all tasks (final version - type-safe and NULL-safe)
DomainResult<std::vector<TaskRecord>> P1_S2_PostgresTaskRepository::listTasks()
{
  using namespace tasqly::p1::s1::domain::core;

  if (!isConnected())
    return DomainResult<std::vector<TaskRecord>>::err(
        DomainError::makeStorage("Not connected to PostgreSQL"));

  // 🧩 SQL query
  std::string sql = "SELECT id, title, notes, status, priority, deadline, created_at, updated_at "
                    "FROM tasks ORDER BY created_at DESC";

  logQuery(sql, {});

  PGresult* res = PQexecParams(m_conn, sql.c_str(), 0, nullptr, nullptr, nullptr, nullptr, 0);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    std::string msg = "PostgreSQL LIST failed: ";
    msg += PQerrorMessage(m_conn);
    PQclear(res);
    return DomainResult<std::vector<TaskRecord>>::err(DomainError::makeStorage(msg));
  }

  std::vector<TaskRecord> tasks;
  int rows = PQntuples(res);
  tasks.reserve(rows);

  for (int i = 0; i < rows; ++i) {
    TaskRecord r;

    // Core fields
    r.id = PQgetvalue(res, i, 0);
    r.title = PQgetvalue(res, i, 1);

    // Optional notes
    r.notes = PQgetisnull(res, i, 2) ? std::string() : std::string(PQgetvalue(res, i, 2));

    // ✅ Parse status safely
    std::string statusStr = PQgetvalue(res, i, 3);
    auto statusOpt = taskStatusFromString(statusStr);
    if (!statusOpt) {
      PQclear(res);
      return DomainResult<std::vector<TaskRecord>>::err(DomainError::makeValidation(
          "Invalid task status value in row " + std::to_string(i) + ": " + statusStr));
    }
    r.status = static_cast<int>(*statusOpt);

    // ✅ Parse priority safely
    std::string priorityStr = PQgetvalue(res, i, 4);
    auto priorityOpt = taskPriorityFromString(priorityStr);
    if (!priorityOpt) {
      PQclear(res);
      return DomainResult<std::vector<TaskRecord>>::err(DomainError::makeValidation(
          "Invalid task priority value in row " + std::to_string(i) + ": " + priorityStr));
    }
    r.priority = static_cast<int>(*priorityOpt);

    // Optional deadline
    if (!PQgetisnull(res, i, 5))
      r.deadline = std::make_optional<std::string>(PQgetvalue(res, i, 5));
    else
      r.deadline = std::nullopt;

    // Timestamps
    r.createdAt = PQgetvalue(res, i, 6);
    r.updatedAt = PQgetvalue(res, i, 7);

    tasks.push_back(std::move(r));
  }

  PQclear(res);
  return DomainResult<std::vector<TaskRecord>>::ok(std::move(tasks));
}

// 🧱 Error translator (final, classified, consistent with P1_Error model)
P1_Error P1_S2_PostgresTaskRepository::handlePgError(const std::string& context)
{
  std::string msg = (m_conn ? PQerrorMessage(m_conn) : "Unknown PostgreSQL error");
  std::string lowerMsg = msg;
  std::transform(lowerMsg.begin(), lowerMsg.end(), lowerMsg.begin(), ::tolower);

  P1_Error err;

  if (lowerMsg.find("authentication") != std::string::npos
      || lowerMsg.find("password") != std::string::npos
      || lowerMsg.find("role") != std::string::npos) {
    err = P1_Error::makeAuth("[Postgres][" + context + "] " + msg);
  } else if (lowerMsg.find("timeout") != std::string::npos
             || lowerMsg.find("canceling") != std::string::npos) {
    err = P1_Error::makeTimeout("[Postgres][" + context + "] " + msg);
  } else if (lowerMsg.find("connection refused") != std::string::npos
             || lowerMsg.find("could not connect") != std::string::npos) {
    err = P1_Error::makeDbInit("[Postgres][" + context + "] Connection failed: " + msg);
  } else if (lowerMsg.find("duplicate key") != std::string::npos
             || lowerMsg.find("unique constraint") != std::string::npos) {
    err = P1_Error::makeDbInit("[Postgres][" + context + "] Duplicate entry: " + msg);
  } else if (lowerMsg.find("syntax error") != std::string::npos
             || lowerMsg.find("invalid input syntax") != std::string::npos) {
    err = P1_Error::makeConfig("[Postgres][" + context
                               + "] Invalid SQL syntax or data type: " + msg);
  } else {
    err = P1_Error::makeDbInit("[Postgres][" + context + "] " + msg);
  }

  P1_Logger::instance().error("[DB][" + context + "] " + msg);

  lastError_ = err;
  return err;
}

// 🧱 Query logger (final safe version)
void P1_S2_PostgresTaskRepository::logQuery(const std::string& sql,
                                            const std::vector<std::string>& params) const
{
  // 🧩 Skip verbose logging in Release unless tracing is enabled
#ifdef NDEBUG
  if (!P1_Logger::instance().isTraceEnabled())
    return;
#endif

  std::ostringstream oss;
  oss << "[DB][QUERY] " << sql;

  if (!params.empty()) {
    oss << " | params=[";

    for (size_t i = 0; i < params.size(); ++i) {
      if (i > 0)
        oss << ", ";

      // 🧩 Sanitize parameter value for logs
      std::string value = params[i];

      // Replace newlines and tabs to keep one-line logs
      std::replace(value.begin(), value.end(), '\n', ' ');
      std::replace(value.begin(), value.end(), '\t', ' ');

      // Truncate very long parameters (e.g., binary data or huge notes)
      constexpr size_t MAX_LEN = 80;
      if (value.size() > MAX_LEN) {
        value = value.substr(0, MAX_LEN - 3) + "...";
      }

      oss << "\"" << value << "\"";
    }

    oss << "]";
  }

  P1_Logger::instance().trace(oss.str());
}

} // namespace tasqly::p1::infra::db

/*
 * 🧱 File: P1_S2_DbTaskRepository.cpp
 * -----------------------------------
 * 📌 Purpose : Implementation of ITaskRepository using IDbConnection.
 * 🧱 Layer   : Infrastructure → Persistence
 * 🎯 Slice   : Phase 1 — Slice 2.5
 */

#include "P1_S2_DbTaskRepository.h"
#include "infra/db/P1_S2_IDbConnection.h"
#include "infra/runtime/P1_Error.h"
#include "infra/runtime/P1_S2_Result.h"
#include <cstring>

// ------------------------------------------------------------
// 🔧 FIX: aliases from correct namespace
// ------------------------------------------------------------
using tasqly::p1::s2::infra::db::DbQueryResult;
using DbResult = tasqly::p1::s2::infra::runtime::Result<DbQueryResult>;
using P1_S2_IDbConnection = tasqly::p1::s2::infra::db::IDbConnection;

// ------------------------------------------------------------
// 🔄 Retry helper — **fixed completely**
// ------------------------------------------------------------
template<typename Fn>
DbResult retryDb(Fn&& fn,
                 P1_S2_IDbConnection* conn,
                 tasqly::p1::infra::runtime::P1_Logger* logger,
                 const std::string& tag)
{
  // Attempt #1
  DbResult r = fn();
  if (r.isOk())
    return r;

  if (logger)
    logger->warn(tag + " failed — retrying…");

  // Reset connection
  conn->reset();

  // Attempt #2
  return fn();
}

namespace tasqly::p1::s2::infra::persistence {

P1_S2_DbTaskRepository::P1_S2_DbTaskRepository(IDbConnection* conn, P1_Logger* logger)
    : m_conn(conn)
    , m_logger(logger)
{
  if (!m_conn && m_logger)
    m_logger->error("DbTaskRepository: IDbConnection is null");
}

// =========================================================
// 🎯 CREATE
// =========================================================
DomainResult<Task> P1_S2_DbTaskRepository::create(const Task& task)
{
  if (!m_conn)
    return DomainResult<Task>::err(
        DomainError::makeStorage("DbTaskRepository: Null DB connection"));

  // -----------------------------------------------------
  // 1) Build SQL INSERT
  // -----------------------------------------------------
  std::string sql = "INSERT INTO tasks ("
                    "id, title, description, status, priority, deadline, created_at, updated_at"
                    ") VALUES (";

  sql += "'" + task.id + "', ";
  sql += "'" + task.title + "', ";

  if (task.notes)
    sql += "'" + *task.notes + "', ";
  else
    sql += "NULL, ";

  sql += std::to_string(static_cast<int>(task.status)) + ", ";
  sql += std::to_string(static_cast<int>(task.priority)) + ", ";

  if (task.deadline) {
    auto t = std::chrono::system_clock::to_time_t(*task.deadline);
    char bufDeadline[32];
    ctime_s(bufDeadline, sizeof(bufDeadline), &t);
    bufDeadline[strcspn(bufDeadline, "\n")] = '\0';
    sql += "'" + std::string(bufDeadline) + "', ";
  } else {
    sql += "NULL, ";
  }

  {
    auto c = std::chrono::system_clock::to_time_t(task.createdAt);
    char bufCreated[32];
    ctime_s(bufCreated, sizeof(bufCreated), &c);
    bufCreated[strcspn(bufCreated, "\n")] = '\0';
    sql += "'" + std::string(bufCreated) + "', ";
  }

  {
    auto u = std::chrono::system_clock::to_time_t(task.updatedAt);
    char bufUpdated[32];
    ctime_s(bufUpdated, sizeof(bufUpdated), &u);
    bufUpdated[strcspn(bufUpdated, "\n")] = '\0';
    sql += "'" + std::string(bufUpdated) + "'";
  }

  sql += ");";

  // -----------------------------------------------------
  // 2) Execute SQL (first attempt)
  // -----------------------------------------------------
  DbResult db1 = m_conn->execute(sql);

  if (db1.isOk())
    return DomainResult<Task>::ok(task);

  // -----------------------------------------------------
  // 3) Retry: reset() → execute() again
  // -----------------------------------------------------
  m_conn->reset();
  DbResult db2 = m_conn->execute(sql);

  if (db2.isOk())
    return DomainResult<Task>::ok(task);

  // -----------------------------------------------------
  // 4) Fail
  // -----------------------------------------------------
  return DomainResult<Task>::err(mapDbError(db2));
}

// =========================================================
// 🎯 READ — getById
// =========================================================
DomainResult<Task> P1_S2_DbTaskRepository::getById(const std::string& id) const
{
  if (!m_conn)
    return DomainResult<Task>::err(DomainError::makeStorage("Database connection unavailable"));

  constexpr int maxAttempts = 2;

  for (int attempt = 1; attempt <= maxAttempts; ++attempt) {
    std::string sql = "SELECT id, title, description, status, priority, deadline, "
                      "       created_at, updated_at "
                      "FROM tasks "
                      "WHERE id = '"
                      + id + "';";

    if (m_logger)
      m_logger->info("Executing getById SQL: " + sql);

    auto dbRes = m_conn->execute(sql);

    if (dbRes.isOk()) {
      const DbQueryResult& qr = dbRes.value();
      return mapSingleRow(qr);
    }

    // لو دي كانت آخر محاولة → رجّع error
    if (attempt == maxAttempts)
      return DomainResult<Task>::err(mapDbError(dbRes));

    // otherwise: retry
    if (m_logger)
      m_logger->warn("getById failed — retrying…");

    m_conn->reset();
  }

  return DomainResult<Task>::err(DomainError::makeStorage("Unexpected getById state"));
}

// =========================================================
// 🎯 UPDATE
// =========================================================
DomainResult<Task> P1_S2_DbTaskRepository::update(const Task& task)
{
  if (!m_conn) {
    if (m_logger)
      m_logger->error("DbTaskRepository::update - null connection");
    return DomainResult<Task>::err(DomainError::makeStorage("Database connection unavailable"));
  }

  // -----------------------------------------------------
  // 1) Convert fields to SQL-safe text
  // -----------------------------------------------------
  auto quoteOrNull = [&](const std::optional<std::string>& v) {
    if (v.has_value())
      return "'" + *v + "'";
    return std::string("NULL");
  };

  auto tpToSql = [&](const std::chrono::system_clock::time_point& tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return std::string("'") + buf + "'";
  };

  // -----------------------------------------------------
  // 2) Build SQL UPDATE
  // -----------------------------------------------------
  std::string sql = "UPDATE tasks SET "
                    "title = '"
                    + task.title
                    + "', "
                      "description = "
                    + quoteOrNull(task.notes)
                    + ", "
                      "status = "
                    + std::to_string(static_cast<int>(task.status))
                    + ", "
                      "priority = "
                    + std::to_string(static_cast<int>(task.priority)) + ", ";

  if (task.deadline.has_value())
    sql += "deadline = " + tpToSql(*task.deadline) + ", ";
  else
    sql += "deadline = NULL, ";

  sql += "created_at = " + tpToSql(task.createdAt) + ", ";
  sql += "updated_at = " + tpToSql(task.updatedAt) + " ";

  sql += "WHERE id = '" + task.id + "';";

  if (m_logger)
    m_logger->info("Executing update SQL: " + sql);

  // -----------------------------------------------------
  // 3) Execute + Retry (same strategy as create())
  // -----------------------------------------------------
  DbResult db1 = m_conn->execute(sql);
  if (db1.isOk())
    return DomainResult<Task>::ok(task);

  // Retry: reset() → execute() again
  m_conn->reset();
  DbResult db2 = m_conn->execute(sql);

  if (db2.isOk())
    return DomainResult<Task>::ok(task);

  // Both failed → error
  return DomainResult<Task>::err(mapDbError(db2));
}

// =========================================================
// 🎯 DELETE
// =========================================================
DomainResult<void> P1_S2_DbTaskRepository::remove(const std::string& id)
{
  if (!m_conn)
    return DomainResult<void>::err(DomainError::makeStorage("Database connection unavailable"));

  constexpr int maxAttempts = 2;

  for (int attempt = 1; attempt <= maxAttempts; ++attempt) {
    // 1) check existence
    auto existing = getById(id);
    if (existing.isErr())
      return DomainResult<void>::err(existing.error());

    // 2) build SQL
    std::string sql = "DELETE FROM tasks WHERE id = '" + id + "';";

    // 3) execute
    auto db = m_conn->execute(sql);

    if (db.isOk())
      return DomainResult<void>::ok();

    // fail → maybe retry
    if (attempt == maxAttempts)
      return DomainResult<void>::err(mapDbError(db));

    if (m_logger)
      m_logger->warn("remove() failed — retrying…");

    m_conn->reset(); // ← أهم حاجة
  }

  return DomainResult<void>::ok();
}

// =========================================================
// 🎯 LIST
// =========================================================
DomainResult<std::vector<Task>> P1_S2_DbTaskRepository::list(std::optional<TaskStatus> status,
                                                             std::optional<TaskPriority> priority,
                                                             std::size_t limit,
                                                             std::size_t offset) const
{
  if (!m_conn) {
    if (m_logger)
      m_logger->error("DbTaskRepository::list - null connection");

    return DomainResult<std::vector<Task>>::err(
        DomainError::makeStorage("Database connection unavailable"));
  }

  // -----------------------------------------------------
  // 1) Build SQL query with optional filters
  // -----------------------------------------------------
  std::string sql
      = "SELECT id, title, description, status, priority, deadline, created_at, updated_at "
        "FROM tasks ";

  bool whereAdded = false;

  if (status.has_value()) {
    sql += whereAdded ? " AND " : " WHERE ";
    sql += "status = " + std::to_string(static_cast<int>(*status));
    whereAdded = true;
  }

  if (priority.has_value()) {
    sql += whereAdded ? " AND " : " WHERE ";
    sql += "priority = " + std::to_string(static_cast<int>(*priority));
  }

  sql += " ORDER BY created_at DESC ";
  sql += " LIMIT " + std::to_string(limit);
  sql += " OFFSET " + std::to_string(offset);
  sql += ";";

  if (m_logger)
    m_logger->info("Executing list SQL: " + sql);

  // -----------------------------------------------------
  // 2) Execute with RETRY LOGIC (fixed!)
  // -----------------------------------------------------
  auto retryRes = retryDb([&]() { return m_conn->execute(sql); }, m_conn, m_logger, "list");

  if (retryRes.isErr())
    return DomainResult<std::vector<Task>>::err(mapDbError(retryRes));

  const DbQueryResult& qr = retryRes.value();

  // -----------------------------------------------------
  // 3) Map rows → vector<Task>
  // -----------------------------------------------------
  return mapRows(qr);
}

// =========================================================
// Helpers — map DB errors → DomainErrors
// =========================================================
DomainError P1_S2_DbTaskRepository::mapDbError(const DbResult& db) const
{
  if (db.isOk())
    return DomainError::makeUnknown("Invalid mapDbError call (db OK)");

  const auto& err = db.error(); // P1_Error

  using RE = tasqly::p1::infra::runtime::RuntimeErrorCode;

  switch (err.code) {
  case RE::DbInit:
  case RE::DbAuth:
  case RE::Timeout:
    // Connection or startup failure
    return DomainError::makeStorage(err.message);

  case RE::Io:
    return DomainError::makeStorage("IO error: " + err.message);

  case RE::Config:
    return DomainError::makeStorage("Config error: " + err.message);

  case RE::Network:
    return DomainError::makeStorage("Network error: " + err.message);

  case RE::None:
    return DomainError::makeUnknown("Unexpected success-state error");

  default:
    return DomainError::makeUnknown("Unknown runtime error: " + err.message);
  }
}

// =========================================================
// Helpers — map row → Task
// =========================================================
DomainResult<Task> P1_S2_DbTaskRepository::mapSingleRow(const DbQueryResult& qr) const
{
  if (qr.rows.empty())
    return DomainResult<Task>::err(DomainError::makeNotFound("Task not found"));

  if (qr.rows.size() > 1)
    return DomainResult<Task>::err(
        DomainError::makeStorage("Expected 1 row but got " + std::to_string(qr.rows.size())));

  const auto& row = qr.rows[0];
  const auto& cols = qr.columns;

  auto getCol = [&](const std::string& name) -> std::optional<std::string> {
    for (size_t i = 0; i < cols.size(); ++i)
      if (cols[i] == name)
        return row[i];
    return std::nullopt;
  };

  Task t;

  // id
  if (auto v = getCol("id"); v && !v->empty())
    t.id = *v;
  else
    return DomainResult<Task>::err(DomainError::makeStorage("Task row missing 'id'"));

  // title
  if (auto v = getCol("title"); v && !v->empty())
    t.title = *v;
  else
    return DomainResult<Task>::err(DomainError::makeStorage("Task row missing 'title'"));

  // description → notes
  if (auto v = getCol("description"); v && !v->empty())
    t.notes = *v;
  else
    t.notes = std::nullopt;

  // status (int)
  if (auto v = getCol("status"); v && !v->empty()) {
    int s = std::stoi(*v);
    t.status = static_cast<TaskStatus>(s);
  } else {
    return DomainResult<Task>::err(DomainError::makeStorage("Missing 'status'"));
  }

  // priority (int)
  if (auto v = getCol("priority"); v && !v->empty()) {
    int p = std::stoi(*v);
    t.priority = static_cast<TaskPriority>(p);
  } else {
    return DomainResult<Task>::err(DomainError::makeStorage("Missing 'priority'"));
  }

  // deadline (nullable timestamp)
  if (auto v = getCol("deadline"); v && !v->empty()) {
    std::tm tm{};
    std::istringstream ss(*v);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (!ss.fail()) {
      t.deadline = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    } else {
      return DomainResult<Task>::err(
          DomainError::makeStorage("Invalid 'deadline' timestamp format"));
    }
  } else {
    t.deadline = std::nullopt;
  }

  // created_at
  if (auto v = getCol("created_at"); v && !v->empty()) {
    std::tm tm{};
    std::istringstream ss(*v);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (!ss.fail()) {
      t.createdAt = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    } else {
      return DomainResult<Task>::err(DomainError::makeStorage("Invalid 'created_at' timestamp"));
    }
  }

  // updated_at
  if (auto v = getCol("updated_at"); v && !v->empty()) {
    std::tm tm{};
    std::istringstream ss(*v);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (!ss.fail()) {
      t.updatedAt = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    } else {
      return DomainResult<Task>::err(DomainError::makeStorage("Invalid 'updated_at' timestamp"));
    }
  }

  return DomainResult<Task>::ok(std::move(t));
}

// =========================================================
// Helpers — map rows → vector<Task>
// =========================================================
DomainResult<std::vector<Task>> P1_S2_DbTaskRepository::mapRows(const DbQueryResult& qr) const
{
  std::vector<Task> out;
  out.reserve(qr.rows.size());

  const auto& cols = qr.columns;

  // Helper to extract column by name
  auto getColIndex = [&](const std::string& name) -> std::optional<size_t> {
    for (size_t i = 0; i < cols.size(); ++i)
      if (cols[i] == name)
        return i;
    return std::nullopt;
  };

  // Pre-resolve column indexes for efficiency
  auto idx_id = getColIndex("id");
  auto idx_title = getColIndex("title");
  auto idx_desc = getColIndex("description");
  auto idx_status = getColIndex("status");
  auto idx_priority = getColIndex("priority");
  auto idx_deadline = getColIndex("deadline");
  auto idx_created = getColIndex("created_at");
  auto idx_updated = getColIndex("updated_at");

  if (!idx_id || !idx_title || !idx_status || !idx_priority)
    return DomainResult<std::vector<Task>>::err(
        DomainError::makeStorage("Missing required columns in DbQueryResult"));

  // Parse each row
  for (const auto& row : qr.rows) {
    Task t;

    // id
    t.id = row[*idx_id];

    // title
    t.title = row[*idx_title];

    // notes
    if (idx_desc && !row[*idx_desc].empty())
      t.notes = row[*idx_desc];
    else
      t.notes = std::nullopt;

    // status
    t.status = static_cast<TaskStatus>(std::stoi(row[*idx_status]));

    // priority
    t.priority = static_cast<TaskPriority>(std::stoi(row[*idx_priority]));

    // deadline
    if (idx_deadline && !row[*idx_deadline].empty()) {
      std::tm tm{};
      std::istringstream ss(row[*idx_deadline]);
      ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
      if (ss.fail())
        return DomainResult<std::vector<Task>>::err(
            DomainError::makeStorage("Invalid 'deadline' timestamp format"));
      t.deadline = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    } else {
      t.deadline = std::nullopt;
    }

    // created_at
    if (idx_created && !row[*idx_created].empty()) {
      std::tm tm{};
      std::istringstream ss(row[*idx_created]);
      ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
      if (ss.fail())
        return DomainResult<std::vector<Task>>::err(
            DomainError::makeStorage("Invalid 'created_at' timestamp"));
      t.createdAt = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }

    // updated_at
    if (idx_updated && !row[*idx_updated].empty()) {
      std::tm tm{};
      std::istringstream ss(row[*idx_updated]);
      ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
      if (ss.fail())
        return DomainResult<std::vector<Task>>::err(
            DomainError::makeStorage("Invalid 'updated_at' timestamp"));
      t.updatedAt = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }

    out.push_back(std::move(t));
  }

  return DomainResult<std::vector<Task>>::ok(std::move(out));
}

} // namespace tasqly::p1::s2::infra::persistence

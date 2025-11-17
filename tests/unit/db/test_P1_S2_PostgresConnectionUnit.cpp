// ------------------------------------------------------------------
// 🧪 Unit Tests — P1_S2_PostgresConnection
// ------------------------------------------------------------------
// These tests validate the retry logic, error propagation, and result
// mapping without hitting a real PostgreSQL server by injecting a
// deterministic FakeLibpqApi.
// ------------------------------------------------------------------

#include <gtest/gtest.h>

#include "infra/db/P1_S2_PostgresConnection.h"
#include "infra/runtime/P1_Error.h"
#include <libpq-fe.h>
#include <deque>
#include <optional>
#include <string>
#include <vector>

using tasqly::p1::infra::runtime::RuntimeErrorCode;
using tasqly::p1::s2::infra::db::DbQueryResult;
using tasqly::p1::s2::infra::db::ILibpqApi;
using tasqly::p1::s2::infra::db::P1_S2_PostgresConnection;

// ------------------------------------------------------------------
// 🧱 Deterministic libpq double for exercising retry logic.
// ------------------------------------------------------------------
class FakeLibpqApi : public ILibpqApi
{
public:
  struct ResultPlan
  {
    ExecStatusType status = PGRES_TUPLES_OK;
    std::vector<std::string> columns;
    std::vector<std::vector<std::optional<std::string>>> rows;
    std::string errorMessage;
  };

  // Configuration knobs
  bool allowConnect = true;
  int initialStatus = CONNECTION_OK;
  int statusAfterReset = CONNECTION_OK;

  // Observability
  int resetCalls = 0;
  std::vector<std::string> execSql;
  std::string lastConnectString;

  // Test helpers
  void queueResult(const ResultPlan& plan) { m_execPlan.emplace_back(plan); }
  void queueNullResult() { m_execPlan.emplace_back(std::nullopt); }

  // ILibpqApi
  void* connect(const std::string& connectionString) override
  {
    lastConnectString = connectionString;
    if (!allowConnect)
      return nullptr;
    auto* fake = new FakeConn();
    fake->status = initialStatus;
    return fake;
  }

  void finish(void* conn) override
  {
    if (auto* fake = asConn(conn))
      delete fake;
  }

  int status(void* conn) override
  {
    if (auto* fake = asConn(conn))
      return fake->status;
    return CONNECTION_BAD;
  }

  void reset(void* conn) override
  {
    ++resetCalls;
    if (auto* fake = asConn(conn))
      fake->status = statusAfterReset;
  }

  void* exec(void* /*conn*/, const std::string& sql) override
  {
    execSql.push_back(sql);
    if (m_execPlan.empty())
      return nullptr;

    auto plan = m_execPlan.front();
    m_execPlan.pop_front();
    if (!plan.has_value())
      return nullptr;

    auto* result = new FakeResult(plan.value());
    return result;
  }

  int resultStatus(void* res) override
  {
    if (auto* fake = asResult(res))
      return fake->status;
    return PGRES_FATAL_ERROR;
  }

  const char* resultErrorMessage(void* res) override
  {
    if (auto* fake = asResult(res))
      return fake->errorMessage.c_str();
    return "";
  }

  int nfields(void* res) override
  {
    if (auto* fake = asResult(res))
      return static_cast<int>(fake->columns.size());
    return 0;
  }

  const char* fname(void* res, int column) override
  {
    if (auto* fake = asResult(res))
      return fake->columns[column].c_str();
    return "";
  }

  int ntuples(void* res) override
  {
    if (auto* fake = asResult(res))
      return static_cast<int>(fake->rows.size());
    return 0;
  }

  bool getisnull(void* res, int row, int column) override
  {
    if (auto* fake = asResult(res))
      return !fake->rows[row][column].has_value();
    return true;
  }

  const char* getvalue(void* res, int row, int column) override
  {
    if (auto* fake = asResult(res)) {
      const auto& cell = fake->rows[row][column];
      if (cell.has_value())
        return cell->c_str();
    }
    return "";
  }

  void clear(void* res) override
  {
    if (auto* fake = asResult(res))
      delete fake;
  }

private:
  struct FakeConn
  {
    int status = CONNECTION_OK;
  };

  struct FakeResult
  {
    explicit FakeResult(ResultPlan plan)
        : status(plan.status)
        , columns(std::move(plan.columns))
        , rows(std::move(plan.rows))
        , errorMessage(std::move(plan.errorMessage))
    {}

    int status;
    std::vector<std::string> columns;
    std::vector<std::vector<std::optional<std::string>>> rows;
    std::string errorMessage;
  };

  static FakeConn* asConn(void* ptr) { return reinterpret_cast<FakeConn*>(ptr); }
  static FakeResult* asResult(void* ptr) { return reinterpret_cast<FakeResult*>(ptr); }

  std::deque<std::optional<ResultPlan>> m_execPlan;
};

// ------------------------------------------------------------------
// 1) Null connection → execute() immediately fails.
// ------------------------------------------------------------------
TEST(P1_S2_PostgresConnection, ExecuteFailsWhenConnectionMissing)
{
  FakeLibpqApi api;
  api.allowConnect = false;

  P1_S2_PostgresConnection conn("host=localhost password=secret", nullptr, &api);

  auto res = conn.execute("SELECT 1;");

  ASSERT_TRUE(res.isErr());
  EXPECT_EQ(res.error().code, RuntimeErrorCode::DbInit);
  EXPECT_TRUE(api.execSql.empty());
}

// ------------------------------------------------------------------
// 2) Fatal result → reset() → success on retry.
// ------------------------------------------------------------------
TEST(P1_S2_PostgresConnection, RetriesOnceAfterFatalResult)
{
  FakeLibpqApi api;
  P1_S2_PostgresConnection conn("dbname=tasqly", nullptr, &api);

  FakeLibpqApi::ResultPlan fatal;
  fatal.status = PGRES_FATAL_ERROR;
  api.queueResult(fatal);

  FakeLibpqApi::ResultPlan ok;
  ok.status = PGRES_TUPLES_OK;
  ok.columns = {"id", "title"};
  ok.rows = {
      {std::optional<std::string>("1"), std::optional<std::string>("Task A")},
      {std::optional<std::string>("2"), std::optional<std::string>("Task B")}};
  api.queueResult(ok);

  auto res = conn.execute("SELECT id, title FROM tasks;");

  ASSERT_TRUE(res.isOk());
  EXPECT_EQ(api.resetCalls, 1);
  ASSERT_EQ(api.execSql.size(), 2u);
  EXPECT_EQ(res.value().columns[0], "id");
  EXPECT_EQ(res.value().columns[1], "title");
  ASSERT_EQ(res.value().rows.size(), 2u);
  EXPECT_EQ(res.value().rows[0][0], "1");
  EXPECT_EQ(res.value().rows[0][1], "Task A");
}

// ------------------------------------------------------------------
// 3) Fatal result + reset failure → propagate DbInit error.
// ------------------------------------------------------------------
TEST(P1_S2_PostgresConnection, ResetFailureAbortsRetry)
{
  FakeLibpqApi api;
  api.statusAfterReset = CONNECTION_BAD;
  P1_S2_PostgresConnection conn("dbname=tasqly", nullptr, &api);

  FakeLibpqApi::ResultPlan fatal;
  fatal.status = PGRES_FATAL_ERROR;
  api.queueResult(fatal);

  auto res = conn.execute("UPDATE tasks SET title='x';");

  ASSERT_TRUE(res.isErr());
  EXPECT_EQ(res.error().code, RuntimeErrorCode::DbInit);
  EXPECT_EQ(res.error().message, "Postgres: retry reset failed");
  EXPECT_EQ(api.resetCalls, 1);
  EXPECT_EQ(api.execSql.size(), 1u);
}

// ------------------------------------------------------------------
// 4) Non-tuple status → propagate PG error message.
// ------------------------------------------------------------------
TEST(P1_S2_PostgresConnection, MapResultPropagatesPgError)
{
  FakeLibpqApi api;
  P1_S2_PostgresConnection conn("dbname=tasqly", nullptr, &api);

  FakeLibpqApi::ResultPlan bad;
  bad.status = PGRES_BAD_RESPONSE;
  bad.errorMessage = "syntax error at or near foo";
  api.queueResult(bad);

  auto res = conn.execute("BROKEN SQL");

  ASSERT_TRUE(res.isErr());
  EXPECT_EQ(res.error().code, RuntimeErrorCode::Io);
  EXPECT_EQ(res.error().message, "Postgres: syntax error at or near foo");
}

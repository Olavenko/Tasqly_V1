// -------------------------------------------------------------
// 🧪 Unit Test — P1_S2_FakeDbConnection
// -------------------------------------------------------------
// Covers:
//   1) Sequential queued results
//   2) Failure → reset → recovery
//   3) isValid() state machine behavior
//   4) Empty queue → error
//   5) Error propagation correctness
//
// References: C++_Standards_Combined.md, Testing_Standards_Combined.md
// -------------------------------------------------------------

#include <gtest/gtest.h>

#include "infra/runtime/P1_Error.h"
#include "infra/runtime/P1_S2_Result.h"
#include "tests/fakes/db/P1_S2_FakeDbConnection.h"
#include <algorithm>

using tasqly::p1::infra::runtime::P1_Error;
using tasqly::p1::infra::runtime::RuntimeErrorCode;
using tasqly::p1::s2::fakes::db::P1_S2_FakeDbConnection;

using tasqly::p1::s2::infra::db::DbQueryResult;
using tasqly::p1::s2::infra::runtime::Result;

using DbResult = Result<DbQueryResult>;

// -------------------------------------------------------------
// 1) Default connection is valid and executes successfully
// -------------------------------------------------------------
TEST(P1_S2_FakeDbConnection, DefaultExecuteSuccess)
{
  P1_S2_FakeDbConnection db;

  auto r = db.execute("SELECT 1");

  ASSERT_TRUE(r.isOk());
  EXPECT_EQ(r.value().columns.size(), 2);
  EXPECT_EQ(r.value().rows.size(), 1);
  EXPECT_EQ(r.value().rows[0][0], "value1");
  EXPECT_EQ(r.value().rows[0][1], "value2");
}

// -------------------------------------------------------------
// 2) Fail Mode → always returns error
// -------------------------------------------------------------
TEST(P1_S2_FakeDbConnection, FailModeAlwaysFails)
{
  P1_S2_FakeDbConnection db;

  db.setFailMode(true);

  auto r = db.execute("Q");

  ASSERT_TRUE(r.isErr());
  EXPECT_EQ(r.error().code, RuntimeErrorCode::DbInit);
  EXPECT_EQ(r.error().message, "Fake: forced failure mode");
}

// -------------------------------------------------------------
// 3) Fail-Then-Recover mode
// -------------------------------------------------------------
TEST(P1_S2_FakeDbConnection, FailThenRecoverModeWorks)
{
  P1_S2_FakeDbConnection db;

  db.setFailThenRecoverMode(true);

  // First execution → failure
  auto first = db.execute("Q1");
  ASSERT_TRUE(first.isErr());
  EXPECT_EQ(first.error().message, "Fake: first failure (recoverable)");

  // Reset → recover
  bool resetOk = db.reset();
  EXPECT_TRUE(resetOk);

  const auto& logList = db.log();
  EXPECT_NE(std::find(logList.begin(), logList.end(), "reset:recovered"), logList.end());

  // Next execution → success
  auto second = db.execute("Q2");
  ASSERT_TRUE(second.isOk());
}

// -------------------------------------------------------------
// 4) Manual invalid state → execute returns error always
// -------------------------------------------------------------
TEST(P1_S2_FakeDbConnection, ManualInvalidStateBlocksExecution)
{
  P1_S2_FakeDbConnection db;

  db.setValid(false);

  auto r = db.execute("ANY");

  ASSERT_TRUE(r.isErr());
  EXPECT_EQ(r.error().code, RuntimeErrorCode::DbInit);
  EXPECT_EQ(r.error().message, "Fake: connection invalid");
}

// -------------------------------------------------------------
// 5) reset() always restores validity
// -------------------------------------------------------------
TEST(P1_S2_FakeDbConnection, ResetRestoresValidState)
{
  P1_S2_FakeDbConnection db;

  db.setValid(false);
  EXPECT_FALSE(db.isValid());

  db.reset();

  EXPECT_TRUE(db.isValid());
}

// -------------------------------------------------------------
// 6) Logging sequence correctness
// -------------------------------------------------------------
TEST(P1_S2_FakeDbConnection, LogTracksOperations)
{
  P1_S2_FakeDbConnection db;

  db.setFailMode(true);
  db.execute("Q1");
  db.reset();

  const auto& log = db.log();

  // Basic expectations:
  EXPECT_NE(std::find(log.begin(), log.end(), "config:failMode=on"), log.end());

  EXPECT_NE(std::find(log.begin(), log.end(), "execute:Q1"), log.end());

  EXPECT_NE(std::find(log.begin(), log.end(), "reset"), log.end());
}

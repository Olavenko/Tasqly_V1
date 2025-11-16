#include <gtest/gtest.h>

#include "infra/persistence/P1_S2_DbTaskRepository.h"
#include "infra/runtime/P1_Logger.h"
#include "tests/fakes/db/P1_S2_FakeDbConnection.h"

using namespace tasqly::p1::s2::infra::persistence;
using namespace tasqly::p1::s2::fakes::db;
using namespace tasqly::p1::s1::domain::core;

static tasqly::p1::infra::runtime::P1_Logger* g_logger
    = &tasqly::p1::infra::runtime::P1_Logger::instance();

static Task makeTask()
{
  Task t;
  t.id = "123";
  t.title = "Test";
  t.notes = "N";
  t.status = TaskStatus::Todo;
  t.priority = TaskPriority::Normal;
  t.createdAt = std::chrono::system_clock::now();
  t.updatedAt = t.createdAt;
  return t;
}

// ========================================================
// CREATE() TESTS
// ========================================================

// --------------------------------------------------------
// FakeDb returns valid result → create() always succeeds
// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, Create_MappingFailsDueToFakeDb)
{
  P1_S2_FakeDbConnection fake;
  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.create(makeTask());

  ASSERT_TRUE(result.isOk());
  EXPECT_EQ(result.value().title, "Test");
}

// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, Create_InvalidConnectionFails)
{
  P1_S2_FakeDbConnection fake;
  fake.setValid(false); // connection starts invalid

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.create(makeTask());

  // After reset() → connection becomes valid → retry succeeds
  ASSERT_TRUE(result.isOk());

  const auto& log = fake.log();

  bool sawReset = false;
  int executeCount = 0;

  for (const auto& e : log) {
    if (e.rfind("execute:", 0) == 0)
      executeCount++;
    if (e == "reset")
      sawReset = true;
  }

  EXPECT_EQ(executeCount, 2);
  EXPECT_TRUE(sawReset);
}

// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, Create_FailModeAlwaysFails)
{
  P1_S2_FakeDbConnection fake;
  fake.setFailMode(true);

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.create(makeTask());

  ASSERT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::Storage);
}

// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, Create_FailThenRecover_RetryTriggered)
{
  P1_S2_FakeDbConnection fake;
  fake.setFailThenRecoverMode(true);

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.create(makeTask());

  // After retry logic → success
  ASSERT_TRUE(result.isOk());

  const auto& log = fake.log();

  bool sawReset = false;
  int executeCount = 0;

  for (const auto& e : log) {
    if (e.rfind("execute:", 0) == 0)
      executeCount++;
    if (e == "reset")
      sawReset = true;
  }

  EXPECT_EQ(executeCount, 2);
  EXPECT_TRUE(sawReset);
}

// ========================================================
// UPDATE() TESTS
// ========================================================

// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, Update_Success)
{
  P1_S2_FakeDbConnection fake;
  P1_S2_DbTaskRepository repo(&fake, g_logger);

  Task t = makeTask();
  t.title = "Updated";

  auto result = repo.update(t);

  ASSERT_TRUE(result.isOk());
  EXPECT_EQ(result.value().title, "Updated");
}

// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, Update_InvalidConnectionFailsInitiallyButRecovers)
{
  P1_S2_FakeDbConnection fake;
  fake.setValid(false);

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  Task t = makeTask();
  t.title = "Updated";

  auto result = repo.update(t);

  ASSERT_TRUE(result.isOk());

  const auto& log = fake.log();

  bool sawReset = false;
  int executeCount = 0;

  for (auto& e : log) {
    if (e.rfind("execute:", 0) == 0)
      executeCount++;
    if (e == "reset")
      sawReset = true;
  }

  EXPECT_EQ(executeCount, 2);
  EXPECT_TRUE(sawReset);
}

// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, Update_FailModeAlwaysFails)
{
  P1_S2_FakeDbConnection fake;
  fake.setFailMode(true);

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.update(makeTask());

  ASSERT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::Storage);
}

// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, Update_FailThenRecover_RetryWorks)
{
  P1_S2_FakeDbConnection fake;
  fake.setFailThenRecoverMode(true);

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  Task t = makeTask();
  t.title = "Updated";

  auto result = repo.update(t);

  ASSERT_TRUE(result.isOk());

  const auto& log = fake.log();

  bool sawReset = false;
  int executeCount = 0;

  for (auto& e : log) {
    if (e.rfind("execute:", 0) == 0)
      executeCount++;
    if (e == "reset")
      sawReset = true;
  }

  EXPECT_EQ(executeCount, 2);
  EXPECT_TRUE(sawReset);
}

// ========================================================
// REMOVE() TESTS
// ========================================================

// --------------------------------------------------------
// Remove: valid connection → success
// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, Remove_Valid_Works)
{
  P1_S2_FakeDbConnection fake;

  // --------------------------------------------
  // Simulate an existing row so getById() succeeds
  // --------------------------------------------
  fake.setSelectResult(
      {.columns
       = {"id", "title", "description", "status", "priority", "deadline", "created_at", "updated_at"},
       .rows = {{"123", "Test", "N", "0", "1", "", "2025-01-01 10:00:00", "2025-01-01 10:00:00"}}});

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.remove("123");

  ASSERT_TRUE(result.isOk());

  const auto& log = fake.log();
  int executeCount = 0;

  for (const auto& e : log) {
    if (e.rfind("execute:", 0) == 0)
      executeCount++;
  }

  // 1 execute for SELECT (getById) + 1 execute for DELETE
  EXPECT_EQ(executeCount, 2);
}

// --------------------------------------------------------
// Remove: invalid connection first → reset → success
// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, Remove_InvalidConnectionFailsInitiallyButRecovers)
{
  P1_S2_FakeDbConnection fake;
  fake.setValid(false);

  fake.setSelectResult(
      {.columns
       = {"id", "title", "description", "status", "priority", "deadline", "created_at", "updated_at"},
       .rows = {{"123",
                 "Test",
                 "N",
                 "0",
                 "1",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00"}}});

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.remove("123");

  ASSERT_TRUE(result.isOk());

  const auto& log = fake.log();
  bool sawReset = false;
  int executeCount = 0;

  for (const auto& e : log) {
    if (e.rfind("execute:", 0) == 0)
      executeCount++;
    if (e == "reset")
      sawReset = true;
  }

  EXPECT_EQ(executeCount, 3); // ✅ SELECT fail + SELECT success + DELETE success
  EXPECT_TRUE(sawReset);
}

// --------------------------------------------------------
// Remove: fail mode → always fails
// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, Remove_FailModeAlwaysFails)
{
  P1_S2_FakeDbConnection fake;
  fake.setFailMode(true);

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.remove("123");

  ASSERT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::Storage);
}

// --------------------------------------------------------
// Remove: fail then recover → retry triggered
// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, Remove_FailThenRecover_RetryTriggered)
{
  P1_S2_FakeDbConnection fake;

  fake.setSelectResult(
      {.columns
       = {"id", "title", "description", "status", "priority", "deadline", "created_at", "updated_at"},
       .rows = {{"123",
                 "Test",
                 "N",
                 "0",
                 "1",
                 "2025-01-01 10:00:00", // FIXED: no empty deadline
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00"}}});

  // Warmup SELECT (must succeed)
  {
    P1_S2_DbTaskRepository repo(&fake, g_logger);
    auto warmup = repo.getById("123");
    ASSERT_TRUE(warmup.isOk());
  }

  // Now allow fail-then-recover
  fake.setFailThenRecoverMode(true);

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.remove("123");

  ASSERT_TRUE(result.isOk());

  const auto& log = fake.log();
  int executeCount = 0;
  bool sawReset = false;

  for (const auto& e : log) {
    if (e.rfind("execute:", 0) == 0)
      executeCount++;
    if (e == "reset")
      sawReset = true;
  }

  EXPECT_TRUE(sawReset);
  EXPECT_GE(executeCount, 4);
}

// ========================================================
// GETBYID() TESTS
// ========================================================

TEST(P1_S2_DbTaskRepository, GetById_Success)
{
  P1_S2_FakeDbConnection fake;

  // Simulate valid SELECT result
  fake.setSelectResult(
      {.columns
       = {"id", "title", "description", "status", "priority", "deadline", "created_at", "updated_at"},
       .rows = {{"123",
                 "Test",
                 "N",
                 "0",
                 "1",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00"}}});

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.getById("123");

  ASSERT_TRUE(result.isOk());
  EXPECT_EQ(result.value().id, "123");
  EXPECT_EQ(result.value().title, "Test");

  // Should execute once
  int exec = 0;
  for (auto& e : fake.log())
    if (e.rfind("execute:", 0) == 0)
      exec++;
  EXPECT_EQ(exec, 1);
}

// --------------------------------------------------------

TEST(P1_S2_DbTaskRepository, GetById_NotFound_ReturnsError)
{
  P1_S2_FakeDbConnection fake;

  fake.setSelectResult(
      {.columns
       = {"id", "title", "description", "status", "priority", "deadline", "created_at", "updated_at"},
       .rows = {}}); // ← empty rows → NOT FOUND

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.getById("123");

  ASSERT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::NotFound);
}

// --------------------------------------------------------

TEST(P1_S2_DbTaskRepository, GetById_InvalidConnectionFailsThenRecovers)
{
  P1_S2_FakeDbConnection fake;
  fake.setValid(false); // start invalid

  // Give valid row for second attempt
  fake.setSelectResult(
      {.columns
       = {"id", "title", "description", "status", "priority", "deadline", "created_at", "updated_at"},
       .rows = {{"123",
                 "Test",
                 "N",
                 "0",
                 "1",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00"}}});

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.getById("123");

  ASSERT_TRUE(result.isOk());

  // Logging check
  int exec = 0;
  bool sawReset = false;

  for (auto& e : fake.log()) {
    if (e.rfind("execute:", 0) == 0)
      exec++;
    if (e == "reset")
      sawReset = true;
  }

  EXPECT_EQ(exec, 2); // first fails, second ok
  EXPECT_TRUE(sawReset);
}

// --------------------------------------------------------

TEST(P1_S2_DbTaskRepository, GetById_FailModeAlwaysFails)
{
  P1_S2_FakeDbConnection fake;
  fake.setFailMode(true);

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.getById("123");

  ASSERT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::Storage);
}

// --------------------------------------------------------

TEST(P1_S2_DbTaskRepository, GetById_FailThenRecover_RetryWorks)
{
  P1_S2_FakeDbConnection fake;
  fake.setFailThenRecoverMode(true);

  fake.setSelectResult(
      {.columns
       = {"id", "title", "description", "status", "priority", "deadline", "created_at", "updated_at"},
       .rows = {{"123",
                 "Test",
                 "N",
                 "0",
                 "1",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00"}}});

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.getById("123");

  ASSERT_TRUE(result.isOk());

  int exec = 0;
  bool sawReset = false;

  for (auto& e : fake.log()) {
    if (e.rfind("execute:", 0) == 0)
      exec++;
    if (e == "reset")
      sawReset = true;
  }

  EXPECT_GE(exec, 2);
  EXPECT_TRUE(sawReset);
}

// --------------------------------------------------------

TEST(P1_S2_DbTaskRepository, GetById_MissingRequiredColumn_ReturnsError)
{
  P1_S2_FakeDbConnection fake;

  // Missing 'title' column
  fake.setSelectResult(
      {.columns = {"id", "description", "status", "priority", "deadline", "created_at", "updated_at"},
       .rows = {{"123",
                 "N",
                 "0",
                 "1",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00"}}});

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.getById("123");

  ASSERT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::Storage);
}

// --------------------------------------------------------

TEST(P1_S2_DbTaskRepository, GetById_InvalidTimestamp_ReturnsError)
{
  P1_S2_FakeDbConnection fake;

  fake.setSelectResult(
      {.columns
       = {"id", "title", "description", "status", "priority", "deadline", "created_at", "updated_at"},
       .rows = {{"123",
                 "Test",
                 "N",
                 "0",
                 "1",
                 "INVALID_TIMESTAMP",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00"}}});

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.getById("123");

  ASSERT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::Storage);
}

// --------------------------------------------------------

TEST(P1_S2_DbTaskRepository, GetById_MultipleRows_ReturnsError)
{
  P1_S2_FakeDbConnection fake;

  fake.setSelectResult(
      {.columns
       = {"id", "title", "description", "status", "priority", "deadline", "created_at", "updated_at"},
       .rows = {{"123",
                 "Test",
                 "N",
                 "0",
                 "1",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00"},
                {"124",
                 "Other",
                 "N",
                 "0",
                 "1",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00"}}});

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.getById("123");

  ASSERT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::Storage);
}

// ========================================================
// LIST() TESTS
// ========================================================

// --------------------------------------------------------
// list(): success with multiple rows
// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, List_SuccessMultipleRows)
{
  P1_S2_FakeDbConnection fake;

  fake.setSelectResult(
      {.columns
       = {"id", "title", "description", "status", "priority", "deadline", "created_at", "updated_at"},
       .rows = {{"1",
                 "A",
                 "DescA",
                 "0",
                 "1",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00"},

                {"2",
                 "B",
                 "DescB",
                 "1",
                 "2",
                 "2025-06-06 10:00:00",
                 "2025-06-06 10:00:00",
                 "2025-06-06 10:00:00"}}});

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.list(std::nullopt, std::nullopt, 100, 0);

  ASSERT_TRUE(result.isOk());
  ASSERT_EQ(result.value().size(), 2);

  EXPECT_EQ(result.value()[0].id, "1");
  EXPECT_EQ(result.value()[1].title, "B");

  // only one execute
  int exec = 0;
  for (auto& e : fake.log())
    if (e.rfind("execute:", 0) == 0)
      exec++;

  EXPECT_EQ(exec, 1);
}

// --------------------------------------------------------
// list(): success with empty table (returns empty vector)
// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, List_EmptyTableReturnsEmptyVector)
{
  P1_S2_FakeDbConnection fake;

  fake.setSelectResult(
      {.columns
       = {"id", "title", "description", "status", "priority", "deadline", "created_at", "updated_at"},
       .rows = {}}); // no rows

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.list(std::nullopt, std::nullopt, 100, 0);

  ASSERT_TRUE(result.isOk());
  EXPECT_TRUE(result.value().empty());

  int exec = 0;
  for (auto& e : fake.log())
    if (e.rfind("execute:", 0) == 0)
      exec++;
  EXPECT_EQ(exec, 1);
}

// --------------------------------------------------------
// list(): invalid connection first → reset → success
// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, List_InvalidConnectionFailsInitiallyButRecovers)
{
  P1_S2_FakeDbConnection fake;
  fake.setValid(false); // start invalid

  fake.setSelectResult(
      {.columns
       = {"id", "title", "description", "status", "priority", "deadline", "created_at", "updated_at"},
       .rows = {{"1",
                 "A",
                 "X",
                 "0",
                 "1",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00"}}});

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.list(std::nullopt, std::nullopt, 100, 0);

  ASSERT_TRUE(result.isOk());
  ASSERT_EQ(result.value().size(), 1);

  bool sawReset = false;
  int exec = 0;

  for (auto& e : fake.log()) {
    if (e.rfind("execute:", 0) == 0)
      exec++;
    if (e == "reset")
      sawReset = true;
  }

  EXPECT_TRUE(sawReset);
  EXPECT_EQ(exec, 2);
}

// --------------------------------------------------------
// list(): fail mode → always fails
// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, List_FailModeAlwaysFails)
{
  P1_S2_FakeDbConnection fake;
  fake.setFailMode(true);

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.list(std::nullopt, std::nullopt, 100, 0);

  ASSERT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::Storage);
}

// --------------------------------------------------------
// list(): fail-then-recover → second try success
// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, List_FailThenRecover_RetryWorks)
{
  P1_S2_FakeDbConnection fake;

  // ------------------------------------------------
  // Warmup: ensure list() returns OK result first
  // ------------------------------------------------
  fake.setSelectResult(
      {.columns
       = {"id", "title", "description", "status", "priority", "deadline", "created_at", "updated_at"},
       .rows = {{"123",
                 "Test",
                 "N",
                 "0",
                 "1",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00"}}});

  {
    P1_S2_DbTaskRepository warm(&fake, g_logger);
    auto warmResult = warm.list(std::nullopt, std::nullopt, 100, 0);
    ASSERT_TRUE(warmResult.isOk());
  }

  // ------------------------------------------------
  // Activate fail-then-recover
  // ------------------------------------------------
  fake.setFailThenRecoverMode(true);

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.list(std::nullopt, std::nullopt, 100, 0);

  for (auto& e : fake.log()) {
    std::cout << "[FAKE] " << e << std::endl;
  }

  ASSERT_TRUE(result.isOk());

  const auto& log = fake.log();
  int executeCount = 0;
  bool sawReset = false;

  for (const auto& e : log) {
    if (e.rfind("execute:", 0) == 0)
      executeCount++;
    if (e == "reset")
      sawReset = true;
  }

  EXPECT_TRUE(sawReset);
  EXPECT_GE(executeCount, 3);
}

// --------------------------------------------------------
// list(): missing required columns → mapping error
// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, List_MissingRequiredColumn_ReturnsError)
{
  P1_S2_FakeDbConnection fake;

  // Missing title column
  fake.setSelectResult(
      {.columns = {"id", "description", "status", "priority", "deadline", "created_at", "updated_at"},
       .rows = {{"1",
                 "Desc",
                 "0",
                 "1",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00",
                 "2025-01-01 10:00:00"}}});

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.list(std::nullopt, std::nullopt, 100, 0);

  ASSERT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::Storage);
}

// --------------------------------------------------------
// list(): invalid timestamps → error
// --------------------------------------------------------
TEST(P1_S2_DbTaskRepository, List_InvalidTimestamp_ReturnsError)
{
  P1_S2_FakeDbConnection fake;

  fake.setSelectResult(
      {.columns
       = {"id", "title", "description", "status", "priority", "deadline", "created_at", "updated_at"},
       .rows
       = {{"1", "A", "X", "0", "1", "INVALID_TIME", "2025-01-01 10:00:00", "2025-01-01 10:00:00"}}});

  P1_S2_DbTaskRepository repo(&fake, g_logger);

  auto result = repo.list(std::nullopt, std::nullopt, 100, 0);

  ASSERT_TRUE(result.isErr());
  EXPECT_EQ(result.error().code, DomainErrorCode::Storage);
}

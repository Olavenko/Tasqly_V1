#include "P1_S2_TestTaskRepositoryFactory.h"

// Runtime logger
#include "infra/runtime/P1_Logger.h"

// Real DB repo
#include "infra/persistence/P1_S2_DbTaskRepository.h"

// Fake DB backend
#include "db/P1_S2_FakeDbConnection.h"

// InMemory fallback
#include "infra/persistence/P1_S2_InMemoryTaskRepository.h"

// Optional real Postgres driver (for integration tests)
#include "infra/db/P1_S2_PostgresConnection.h"

using tasqly::p1::s1::domain::core::ITaskRepository;

using tasqly::p1::infra::runtime::P1_Logger;

using tasqly::p1::s2::fakes::db::P1_S2_FakeDbConnection;
using tasqly::p1::s2::infra::db::P1_S2_PostgresConnection;

using tasqly::p1::infra::persistence::P1_S2_InMemoryTaskRepository;
using tasqly::p1::s2::infra::persistence::P1_S2_DbTaskRepository;

namespace tasqly::p1::s2::infra::factories {

// ---------------------------------------------------------
// Singleton
// ---------------------------------------------------------
P1_S2_TestTaskRepositoryFactory& P1_S2_TestTaskRepositoryFactory::instance()
{
  static P1_S2_TestTaskRepositoryFactory inst;
  return inst;
}

// ---------------------------------------------------------
// create()
// ---------------------------------------------------------
std::shared_ptr<ITaskRepository> P1_S2_TestTaskRepositoryFactory::create(TestRepoMode mode)
{
  auto& logger = P1_Logger::instance();

  switch (mode) {
  // -----------------------------------------------------
  // 1) FakeDb → DbTaskRepository
  // -----------------------------------------------------
  case TestRepoMode::FakeDb: {
    auto fake = std::make_shared<P1_S2_FakeDbConnection>();
    logger.info("[TEST] Using FakeDbConnection backend");
    return std::make_shared<P1_S2_DbTaskRepository>(fake.get(), &logger);
  }

  // -----------------------------------------------------
  // 2) InMemoryTaskRepository
  // -----------------------------------------------------
  case TestRepoMode::InMemory: {
    logger.info("[TEST] Using InMemoryTaskRepository backend");
    return std::make_shared<P1_S2_InMemoryTaskRepository>();
  }

  // -----------------------------------------------------
  // 3) Real PostgreSQL (for integration tests)
  // -----------------------------------------------------
  case TestRepoMode::Postgres: {
    const std::string connStr = "host=127.0.0.1 port=5432 dbname=tasqly user=postgres";

    logger.info("[TEST] Using PostgreSQL backend (integration tests)");
    auto pg = std::make_shared<P1_S2_PostgresConnection>(connStr, &logger);

    if (!pg->isValid())
      throw std::runtime_error("[TEST] PostgreSQL connection failed.");

    return std::make_shared<P1_S2_DbTaskRepository>(pg.get(), &logger);
  }

  default:
    throw std::runtime_error("Unknown TestRepoMode");
  }
}

} // namespace tasqly::p1::s2::infra::factories

#pragma once
#pragma once
/*
 * 🧪 File: P1_S2_TestTaskRepositoryFactory.h
 * -----------------------------------------
 * 📌 Purpose:
 *   Test-only factory used to build task repositories using:
 *     - FakeDbConnection (default)
 *     - InMemoryTaskRepository
 *     - PostgreSQL (optional for integration tests)
 *
 * 🧱 Layer: tests/fakes/factories
 * 🧠 Depends on:
 *   - tasqly_core (domain interfaces + real repos)
 *   - tasqly_fakes (FakeDbConnection)
 */

#include <memory>

namespace tasqly::p1::s1::domain::core {
class ITaskRepository;
}

namespace tasqly::p1::s2::infra::db {
class P1_S2_PostgresConnection;
}

namespace tasqly::p1::s2::fakes::db {
class P1_S2_FakeDbConnection;
}

namespace tasqly::p1::infra::persistence {
class P1_S2_InMemoryTaskRepository;
}

namespace tasqly::p1::infra::runtime {
class P1_Logger;
}

namespace tasqly::p1::s2::infra::factories {

enum class TestRepoMode { FakeDb, InMemory, Postgres };

class P1_S2_TestTaskRepositoryFactory
{
public:
  static P1_S2_TestTaskRepositoryFactory& instance();

  // 🎯 Creates repository based on selected mode
  std::shared_ptr<tasqly::p1::s1::domain::core::ITaskRepository> create(
      TestRepoMode mode = TestRepoMode::FakeDb);

private:
  P1_S2_TestTaskRepositoryFactory() = default;
  ~P1_S2_TestTaskRepositoryFactory() = default;
};

} // namespace tasqly::p1::s2::infra::factories

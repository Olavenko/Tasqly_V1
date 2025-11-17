#pragma once

/*
 * 🧱 File: P1_S2_TaskRepositoryFactory.h
 * --------------------------------------
 * Factory for creating a TaskRepository using:
 *   - PostgreSQL backend
 *   - InMemory fallback
 */

#include <memory>
#include <string>

namespace tasqly::p1::s1::domain::core {
class ITaskRepository;
}

namespace tasqly::p1::s2::infra::db {
class IDbConnection;
class P1_S2_PostgresConnection;
} // namespace tasqly::p1::s2::infra::db

namespace tasqly::p1::infra::persistence {
class P1_S2_InMemoryTaskRepository;
}

namespace tasqly::p1::infra::runtime {
class P1_Logger;
class P1_AppSettings;
class P1_Notifier;
} // namespace tasqly::p1::infra::runtime

namespace tasqly::p1::s2::infra::factories {

class P1_S2_TaskRepositoryFactory
{
public:
  static P1_S2_TaskRepositoryFactory& instance();

  std::shared_ptr<tasqly::p1::s1::domain::core::ITaskRepository> create();
  
  // Alias for integration tests
  std::shared_ptr<tasqly::p1::s1::domain::core::ITaskRepository> createRepository();

  std::string mode() const;
  
  // Alias for integration tests
  std::string currentMode() const;

private:
  P1_S2_TaskRepositoryFactory() = default;
  ~P1_S2_TaskRepositoryFactory() = default;

private:
  bool m_usingFallback = false;
  std::string m_currentMode = "Uninitialized";
};

} // namespace tasqly::p1::s2::infra::factories

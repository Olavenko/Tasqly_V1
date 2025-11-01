#pragma once
/*
 * 🧱 File: P1_S2_TaskRepositoryFactory.h
 * ---------------------------------------
 * 📌 Purpose   : Factory to create task repository (Postgres or InMemory fallback)
 * 🧱 Layer     : Infrastructure (Factory)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-21
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Creates a runtime repository instance depending on environment configuration.
 *  - Primary: PostgreSQL repository (production)
 *  - Fallback: InMemory repository (development/testing)
 *
 * 🔗 Depends On:
 *   - P1_S2_PostgresTaskRepository
 *   - P1_S2_InMemoryTaskRepository
 *   - P1_Logger / P1_Notifier / P1_AppSettings
 */

#include <memory>
#include <string>

namespace tasqly::p1::infra::factories {

class P1_S2_TaskRepositoryFactory final
{
public:
  // ⚙️ Singleton accessor
  static P1_S2_TaskRepositoryFactory& instance();

  // 🧩 Create repository (Postgres → fallback InMemory)
  std::shared_ptr<void> createRepository();

  // 🧠 Diagnostics: current mode (for logs/UI)
  std::string currentMode() const;

private:
  // 🧱 Internal lifecycle
  P1_S2_TaskRepositoryFactory() = default;
  ~P1_S2_TaskRepositoryFactory() = default;

  std::shared_ptr<void> createPostgresRepo();
  std::shared_ptr<void> createInMemoryRepo();

private:
  bool m_usingFallback = false;
  std::string m_currentMode = "PostgreSQL (Primary)";
};

} // namespace tasqly::p1::infra::factories

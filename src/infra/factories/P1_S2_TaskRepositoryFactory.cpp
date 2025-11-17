/*
 * 🧱 File: P1_S2_TaskRepositoryFactory.cpp
 * ----------------------------------------
 * 📌 Purpose   : Factory for creating the new DB-backed task repository.
 * 🧱 Layer     : Infrastructure → Factories
 * 🎯 Slice     : Phase 1 — Slice 2.5
 */

#include "P1_S2_TaskRepositoryFactory.h"

// Runtime
#include "infra/runtime/P1_AppSettings.h"
#include "infra/runtime/P1_Logger.h"
#include "infra/runtime/P1_Notifier.h"

// DB
#include "infra/db/P1_S2_PostgresConnection.h"

// Repositories
#include "infra/persistence/P1_S2_DbTaskRepository.h"
#include "infra/persistence/P1_S2_InMemoryTaskRepository.h"

using tasqly::p1::infra::runtime::P1_AppSettings;
using tasqly::p1::infra::runtime::P1_Logger;
using tasqly::p1::infra::runtime::P1_Notifier;

using tasqly::p1::infra::persistence::P1_S2_InMemoryTaskRepository;
using tasqly::p1::s2::infra::db::P1_S2_PostgresConnection;
using tasqly::p1::s2::infra::persistence::P1_S2_DbTaskRepository;

using tasqly::p1::s1::domain::core::ITaskRepository;

namespace tasqly::p1::s2::infra::factories {

// ---------------------------------------------------------
// Singleton
// ---------------------------------------------------------
P1_S2_TaskRepositoryFactory& P1_S2_TaskRepositoryFactory::instance()
{
  static P1_S2_TaskRepositoryFactory inst;
  return inst;
}

// ---------------------------------------------------------
// create()
// ---------------------------------------------------------
std::shared_ptr<ITaskRepository> P1_S2_TaskRepositoryFactory::create()
{
  auto& settings = P1_AppSettings::instance();
  auto& logger = P1_Logger::instance();
  auto& notifier = P1_Notifier::instance();

  const bool fallbackEnable = settings.getBool("features.db.fallback_inmemory", true);
  const std::string connStr = settings.getString("db.connection_string")
                                  .value_or("host=127.0.0.1 port=5432 dbname=tasqly user=postgres");

  logger.info("TaskRepositoryFactory: initializing…");

  // -----------------------------------------------------
  // Try PostgreSQL
  // -----------------------------------------------------
  try {
    logger.info("Trying PostgreSQL…");

    auto pg = std::make_shared<P1_S2_PostgresConnection>(connStr, &logger);

    if (pg->isValid()) {
      logger.info("TaskRepositoryFactory: using PostgreSQL backend");
      m_currentMode = "PostgreSQL (Primary)";
      m_usingFallback = false;
      return std::make_shared<P1_S2_DbTaskRepository>(pg.get(), &logger);
    }

    logger.error("PostgreSQL connection invalid");

    if (!fallbackEnable) {
      logger.error("Fallback is disabled, no repository available");
      m_currentMode = "Offline (No Repository)";
      m_usingFallback = false;
      notifier.error("❌ PostgreSQL failed — fallback disabled");
      throw std::runtime_error("PostgreSQL failed with no fallback");
    }

  } catch (const std::runtime_error&) {
    throw;
  } catch (const std::exception& ex) {
    logger.error("PostgreSQL exception: " + std::string(ex.what()));
    if (!fallbackEnable) {
      logger.error("Fallback is disabled, no repository available");
      m_currentMode = "Offline (No Repository)";
      m_usingFallback = false;
      throw std::runtime_error("PostgreSQL failed with no fallback");
    }
  }

  // -----------------------------------------------------
  // Fallback → InMemory
  // -----------------------------------------------------
  logger.warn("TaskRepositoryFactory: Using InMemory fallback");
  notifier.toast("⚠️ Falling back to InMemory repository");

  m_currentMode = "InMemory (Fallback)";
  m_usingFallback = true;

  return std::make_shared<P1_S2_InMemoryTaskRepository>();
}

// ---------------------------------------------------------
// createRepository() - Alias for integration tests
// ---------------------------------------------------------
std::shared_ptr<ITaskRepository> P1_S2_TaskRepositoryFactory::createRepository()
{
  return create();
}

// ---------------------------------------------------------
// Diagnostics
// ---------------------------------------------------------
std::string P1_S2_TaskRepositoryFactory::mode() const
{
  return m_currentMode;
}

// ---------------------------------------------------------
// currentMode() - Alias for integration tests
// ---------------------------------------------------------
std::string P1_S2_TaskRepositoryFactory::currentMode() const
{
  return mode();
}

} // namespace tasqly::p1::s2::infra::factories

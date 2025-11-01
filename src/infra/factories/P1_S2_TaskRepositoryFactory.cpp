/*
 * 🧱 File: P1_S2_TaskRepositoryFactory.cpp
 * ---------------------------------------
 * 📌 Purpose   : Implementation of runtime repository selector (Postgres → InMemory fallback)
 * 🧱 Layer     : Infrastructure (Factory)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-21
 * 🔖 Version   : 1.1
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Initializes the appropriate repository implementation based on environment configuration
 * and runtime connectivity. If PostgreSQL initialization fails, and the fallback feature flag
 * is enabled (`features.db.fallback_inmemory`), the factory seamlessly switches to the
 * InMemory repository and sends a user notification.
 *
 * 🔗 Related:
 *   - P1_S2_PostgresTaskRepository
 *   - P1_S2_InMemoryTaskRepository
 *   - P1_Logger, P1_Notifier, P1_AppSettings
 */

#include "infra/factories/P1_S2_TaskRepositoryFactory.h"
#include "infra/db/P1_S2_PostgresTaskRepository.h"
#include "infra/persistence/P1_S2_InMemoryTaskRepository.h"
#include "infra/runtime/P1_AppSettings.h"
#include "infra/runtime/P1_Logger.h"
#include "infra/runtime/P1_Notifier.h"

#include <exception>
#include <memory>

using namespace tasqly::p1::infra::runtime;
using namespace tasqly::p1::infra::db;
using namespace tasqly::p1::infra::persistence;

namespace tasqly::p1::infra::factories {

// ⚙️ Singleton accessor
P1_S2_TaskRepositoryFactory& P1_S2_TaskRepositoryFactory::instance()
{
  static P1_S2_TaskRepositoryFactory factory;
  return factory;
}

// 🧱 Create repository — main entry point
std::shared_ptr<void> P1_S2_TaskRepositoryFactory::createRepository()
{
  auto& logger = P1_Logger::instance();
  auto& notifier = P1_Notifier::instance();
  auto& settings = P1_AppSettings::instance();

  logger.info("[Factory] Starting repository initialization...");

  // 🔹 Step 1: Try PostgreSQL
  try {
    auto pgRepo = std::make_shared<P1_S2_PostgresTaskRepository>();

    if (pgRepo->isConnected()) {
      m_usingFallback = false;
      logger.info("[Factory] ✅ Using PostgreSQL Task Repository.");
      notifier.toast("✅ Connected to PostgreSQL database");
      return std::static_pointer_cast<void>(pgRepo);
    }

    logger.warn("[Factory] PostgreSQL connection not established at startup.");
  } catch (const std::exception& ex) {
    logger.error(std::string("[Factory] PostgreSQL init exception: ") + ex.what());
  }

  // 🔹 Step 2: Check if fallback is allowed
  const bool allowFallback = settings.getBool("features.db.fallback_inmemory", true);
  if (!allowFallback) {
    logger.error("[Factory] ❌ PostgreSQL connection failed and fallback is disabled.");
    notifier.toast("❌ Database connection failed and fallback is disabled.");

    // 🧩 Reflect offline state for diagnostics and tests
    m_usingFallback = false;
    m_currentMode = "Offline (No Repository)";

    return nullptr;
  }

  // 🔹 Step 3: Fallback to InMemory
  try {
    m_usingFallback = true;
    auto memRepo = std::make_shared<P1_S2_InMemoryTaskRepository>();

    if (settings.getBool("features.inmemory.seed", false)) {
      memRepo->seedDemoData();
      logger.info("[Factory] 🌱 InMemory repository seeded for development parity.");
    }

    logger.warn("[Factory] ⚠️ Switched to InMemory repository (fallback active).");
    notifier.toast("⚠️ Switched to InMemory repository (DB unavailable)");
    return std::static_pointer_cast<void>(memRepo);

  } catch (const std::exception& ex) {
    logger.error(std::string("[Factory] InMemory repository init failed: ") + ex.what());
    notifier.toast("❌ Fatal: No repository could be initialized.");
    return nullptr;
  }
}

// 🧩 Diagnostics — report current mode
std::string P1_S2_TaskRepositoryFactory::currentMode() const
{
  if (m_currentMode == "Offline (No Repository)")
    return m_currentMode;

  return m_usingFallback ? "InMemory (Fallback)" : "PostgreSQL (Primary)";
}

} // namespace tasqly::p1::infra::factories

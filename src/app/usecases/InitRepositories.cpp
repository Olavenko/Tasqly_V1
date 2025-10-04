/*
 * 🧱 File: InitRepositories.cpp
 * -----------------------------
 * 📌 Purpose   : Implementation of InitRepositories use case.
 * 🧱 Layer     : Application (Use Cases)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Instantiates in-memory repositories and system utilities,
 * registers them into AppContext, and marks loading state ready.
 */

#include "app/usecases/InitRepositories.h"

#include "infra/persistence/InMemoryGoalRepository.h"
#include "infra/persistence/InMemoryTaskRepository.h"
#include "infra/utils/SystemClock.h"
#include "infra/utils/SystemUuidGen.h"

using namespace tasqly::domain::core;

namespace tasqly::app::usecases {

// 🚀 Initializes repositories and utils
Result<void> InitRepositories(AppContext& context,
                              AppLoadingManager& loading,
                              tasqly::app::logging::ILogManager& log,
                              bool seed)
{
  try {
    // 🛠️ Build in-memory repositories
    auto taskRepo = std::make_shared<InMemoryTaskRepository>(seed);
    auto goalRepo = std::make_shared<InMemoryGoalRepository>(seed);

    // 🛠️ Build system utilities
    auto uuidGen = std::make_shared<SystemUuidGen>();
    auto clock = std::make_shared<SystemClock>();

    // 🗄️ Register in context
    context.setTaskRepository(taskRepo);
    context.setGoalRepository(goalRepo);
    context.setUuidGen(uuidGen);
    context.setClock(clock);

    // 📋 Log success
    log.log(tasqly::app::logging::Level::Info, "app.init", "Repositories initialized successfully");

    // 🔁 Flip loading state
    loading.setRepositoriesReady(true);

    return ok();
  } catch (const std::exception& ex) {
    log.log(tasqly::app::logging::Level::Error,
            "app.init",
            QString("Failed to initialize repositories: %1").arg(ex.what()));
    return err<void>(
        Error{Error::kUnknownCode, "InitRepositories failed", {{"exception", ex.what()}}});
  }
}

} // namespace tasqly::app::usecases

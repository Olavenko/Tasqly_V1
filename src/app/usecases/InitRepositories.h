/*
 * 🧱 File: InitRepositories.h
 * ---------------------------
 * 📌 Purpose   : Use case to initialize application repositories and utilities.
 * 🧱 Layer     : Application (Use Cases)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Builds concrete repository and utility implementations (in-memory for now),
 * registers them in AppContext, logs initialization, and updates AppLoadingManager state.
 */

#pragma once

#include "app/core/AppContext.h"
#include "app/loading/AppLoadingManager.h"
#include "app/logging/ILogManager.h"
#include "domain/core/Result.h"

namespace tasqly::app::usecases {

// 🚀 Initializes repositories and utils, wires them into AppContext
tasqly::domain::core::Result<void> InitRepositories(AppContext& context,
                                                    AppLoadingManager& loading,
                                                    logging::ILogManager& log,
                                                    bool seed = false);

} // namespace tasqly::app::usecases

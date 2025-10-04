/*
 * 🧱 File: ListTasks.h
 * --------------------
 * 📌 Purpose   : Use case to list tasks with pagination.
 * 🧱 Layer     : Application (Use Cases)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Provides a simple interface to list tasks via ITaskRepository.
 * Returns a Result with paginated tasks or an Error on failure.
 */

#pragma once

#include "app/core/AppContext.h"
#include "app/logging/ILogManager.h"
#include "domain/core/Result.h"
#include "domain/core/Task.h"

#include <vector>

namespace tasqly::app::usecases {

// 📋 Lists tasks with pagination via repository
tasqly::domain::core::Result<std::vector<tasqly::domain::core::Task>> ListTasks(
    AppContext& context, logging::ILogManager& log, int limit, int offset);

} // namespace tasqly::app::usecases

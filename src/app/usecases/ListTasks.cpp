/*
 * 🧱 File: ListTasks.cpp
 * ----------------------
 * 📌 Purpose   : Implementation of ListTasks use case.
 * 🧱 Layer     : Application (Use Cases)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Retrieves a paginated list of tasks from ITaskRepository.
 * Handles logging of success and error cases.
 */

#include "app/usecases/ListTasks.h"

using namespace tasqly::domain::core;
using namespace tasqly::app::usecases;

namespace tasqly::app::usecases {

// 📋 Lists tasks with pagination via repository
Result<std::vector<Task>> ListTasks(AppContext& context,
                                    tasqly::app::logging::ILogManager& log,
                                    int limit,
                                    int offset)
{
  try {
    auto repo = context.taskRepository();
    if (!repo) {
      log.log(tasqly::app::logging::Level::Error,
              "usecase.listtasks",
              "Task repository not available");
      return err<std::vector<Task>>(Error{Error::kUnknownCode, "Task repository missing"});
    }

    // 💡 Call repository (async)
    auto future = repo->list(limit, offset);
    auto result = future.get();

    if (result.isOk()) {
      log.log(tasqly::app::logging::Level::Info,
              "usecase.listtasks",
              QString("Listed %1 tasks (limit=%2, offset=%3)")
                  .arg(result.value().size())
                  .arg(limit)
                  .arg(offset));
      return result;
    } else {
      log.log(tasqly::app::logging::Level::Error,
              "usecase.listtasks",
              QString("Repository error: %1").arg(result.error().toString()));
      return result;
    }
  } catch (const std::exception& ex) {
    log.log(tasqly::app::logging::Level::Error,
            "usecase.listtasks",
            QString("Exception during ListTasks: %1").arg(ex.what()));
    return err<std::vector<Task>>(
        Error{Error::kUnknownCode, "ListTasks failed", {{"exception", ex.what()}}});
  }
}
} // namespace tasqly::app::usecases

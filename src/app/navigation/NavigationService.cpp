/*
 * 🧱 File: NavigationService.cpp
 * ------------------------------
 * 📌 Purpose   : Implements NavigationService with route validation & logging.
 * 🧱 Layer     : Application (Navigation Subsystem)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-05
 *
 * 🧠 Description:
 * Provides the core implementation for guarded navigation.
 * Logs attempts, prevents invalid routes, and updates navigation stack.
 */

#include "NavigationService.h"
#include "app/notifier/Notifier.h"

// ➕ Constructor injects logger dependency
NavigationService::NavigationService(tasqly::app::logging::ILogManager* logger, QObject* parent)
    : QObject(parent)
    , m_logger(logger)
{}

// ➡️ Navigate to a new route (guarded)
void NavigationService::navigateTo(const Route& route)
{
  // ♻️ Always remember last attempt (even if invalid)
  m_lastAttempt = route;

  if (!route.isValid()) {
    handleInvalidRoute(route);
    return;
  }

  m_stack.push(route);

  if (m_logger) {
    // ✅ Navigate success
    m_logger->log(tasqly::app::logging::Level::Info,
                  "navigation",
                  QString("Navigated to: %1").arg(route.toString()));
  }

  emit navigated(route);
}

// ↩️ Go back to previous route (if available)
void NavigationService::goBack()
{
  if (m_stack.isEmpty()) {
    if (m_logger) {
      // ⚠️ Stack empty
      m_logger->log(tasqly::app::logging::Level::Warn, "navigation", "Navigation stack is empty.");
    }
    emit navigationFailed("Navigation stack is empty.");
    return;
  }

  m_stack.pop();
  if (!m_stack.isEmpty()) {
    Route route = m_stack.top();
    if (m_logger) {
      // ↩️ Returned back
      m_logger->log(tasqly::app::logging::Level::Info,
                    "navigation",
                    QString("Returned to: %1").arg(route.toString()));
    }
    emit navigated(route);
  }
}

// 📋 Returns current route (top of stack)
Route NavigationService::currentRoute() const
{
  return m_stack.isEmpty() ? Route{} : m_stack.top();
}

// 🛡️ Internal: handle invalid route with logging
void NavigationService::handleInvalidRoute(const Route& route)
{
  QString reason = QString("Invalid route attempted: %1").arg(route.toString());

  if (m_logger) {
    // ❌ Invalid route
    m_logger->log(tasqly::app::logging::Level::Error, "navigation", reason);
  }

  // 🔔 Show toast notification for invalid route
  Notifier::show(reason, "error");

  emit navigationFailed(reason);
}

// 🔁 Retry the last attempted navigation
void NavigationService::retryLast()
{
  if (m_lastAttempt.isValid()) {
    // 📝 Push again into stack and emit directly
    m_stack.push(m_lastAttempt);

    if (m_logger) {
      m_logger->log(tasqly::app::logging::Level::Info,
                    "navigation",
                    QString("Retried route: %1").arg(m_lastAttempt.toString()));
    }

    emit navigated(m_lastAttempt);
  } else {
    if (m_logger) {
      m_logger->log(tasqly::app::logging::Level::Warn,
                    "navigation",
                    "Retry failed: no valid last route");
    }
    emit navigationFailed("No last route to retry.");
  }
}

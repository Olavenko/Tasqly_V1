/*
 * 🧱 File: NavigateTo.cpp
 * -----------------------
 * 📌 Purpose   : Implements the NavigateTo use case with logging & state machine.
 * 🧱 Layer     : Application (Use Cases)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-05
 *
 * 🧠 Description:
 * Provides a guarded entrypoint for navigating to a new route.
 * Integrates NavigationService for route validation, logs results,
 * and emits PageState transitions for SmartPageLoader.
 */

#include "NavigateTo.h"

using namespace tasqly::app::logging;

// ➕ Constructor injects NavigationService + Logger
NavigateTo::NavigateTo(NavigationService* navService, ILogManager* logger, QObject* parent)
    : QObject(parent)
    , m_navService(navService)
    , m_logger(logger)
{
  // 🪢 Wire NavigationService signals to page state machine
  connect(m_navService, &NavigationService::navigated, this, [this](const Route& route) {
    if (m_logger) {
      m_logger->log(Level::Info,
                    "usecase.navigate",
                    QString("✅ Navigation successful: %1").arg(route.toString()));
    }
    emit pageStateChanged(PageStateWrapper::PageState::Ready);
  });

  connect(m_navService, &NavigationService::navigationFailed, this, [this](const QString& reason) {
    if (m_logger) {
      m_logger->log(Level::Error,
                    "usecase.navigate",
                    QString("❌ Navigation failed: %1").arg(reason));
    }
    emit pageStateChanged(PageStateWrapper::PageState::Error);
  });
}

// 🎯 Executes navigation request
void NavigateTo::execute(const QVariantMap& data)
{
  Route route;
  route.id = data.value("id").toString();
  route.timestamp = data.value("timestamp").toDateTime();

  m_lastRoute = route; // 📝 Save last attempted route

  emit pageStateChanged(PageStateWrapper::Loading);
  m_navService->navigateTo(route);
}

// 🔁 Retries the last navigation attempt (if valid)
void NavigateTo::retry()
{
  if (m_lastRoute.isValid()) {
    if (m_logger) {
      m_logger->log(Level::Info,
                    "usecase.navigate",
                    QString("🔁 Retrying navigation to: %1").arg(m_lastRoute.toString()));
    }
    // ⚡ Reuse last stored route
    QVariantMap retryData;
    retryData.insert("id", m_lastRoute.id);
    retryData.insert("timestamp", m_lastRoute.timestamp);

    execute(retryData);
  } else {
    if (m_logger) {
      m_logger->log(Level::Warn,
                    "usecase.navigate",
                    "⚠️ Retry requested but no valid lastRoute is stored.");
    }
    emit pageStateChanged(PageStateWrapper::Error);
  }
}

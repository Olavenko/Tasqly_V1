/*
 * 🧱 File: NavigationService.h
 * ----------------------------
 * 📌 Purpose   : Provides navigation API with route guard and logging support.
 * 🧱 Layer     : Application (Navigation Subsystem)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-05
 *
 * 🧠 Description:
 * Encapsulates navigation logic using Route values. 
 * Validates routes before applying stack changes, integrates with LogManager 
 * for diagnostics, and emits signals for UI/QML layers.
 */

#pragma once

#include "app/logging/ILogManager.h"
#include "domain/core/Route.h"
#include <QObject>
#include <QStack>
#include <QString>

// ➡️ Provides navigation API with route guard and logging
class NavigationService : public QObject
{
  Q_OBJECT

public:
  explicit NavigationService(tasqly::app::logging::ILogManager* logger, QObject* parent = nullptr);

  // ➕ Navigate to a new route (with guard validation)
  Q_INVOKABLE void navigateTo(const Route& route);

  // ↩️ Go back to previous route (if available)
  Q_INVOKABLE void goBack();

  // 📋 Returns current route (top of stack)
  Route currentRoute() const;

  // 🔄 Retry last attempted navigation (preserves context)
  Q_INVOKABLE void retryLast();

signals:
  // 🚦 Emitted when navigation is successful
  void navigated(const Route& route);

  // ❌ Emitted when navigation fails (invalid route, guard rejection)
  void navigationFailed(const QString& reason);

private:
  // 📚 Stack of navigation history
  QStack<Route> m_stack;

  // 🪵 Logging dependency
  tasqly::app::logging::ILogManager* m_logger;

  // 🛡️ Internal: validate and log route failures
  void handleInvalidRoute(const Route& route);

  // ♻️ Last attempted route (for retry support)
  Route m_lastAttempt;
};

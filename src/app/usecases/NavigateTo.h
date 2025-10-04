/*
 * 🧱 File: NavigateTo.h
 * ---------------------
 * 📌 Purpose   : Use case for guarded navigation with loader state transitions.
 * 🧱 Layer     : Application (Use Cases)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-05
 *
 * 🧠 Description:
 * Encapsulates navigation request logic:
 *  - Validates the route via NavigationService.
 *  - Emits loader state (PageState).
 *  - Logs navigation attempts and errors.
 */

#pragma once

#include "app/logging/ILogManager.h"
#include "app/navigation/NavigationService.h"
#include "domain/core/PageState.h"
#include "domain/core/Route.h"
#include <QObject>
#include <QtQml/qqml.h>

class NavigateTo : public QObject
{
  Q_OBJECT

public:
  explicit NavigateTo(NavigationService* navService,
                      tasqly::app::logging::ILogManager* logger,
                      QObject* parent = nullptr);

  // 🎯 Execute navigation use case with data from QML (map → Route)
  Q_INVOKABLE void execute(const QVariantMap& data);

  // 🔁 Retry last navigation attempt
  Q_INVOKABLE void retry();

signals:
  // 🔄 Loader state transitions (used by SmartPageLoader/Fallback in QML)
  void pageStateChanged(PageStateWrapper::PageState state);

private:
  NavigationService* m_navService;
  tasqly::app::logging::ILogManager* m_logger;
  Route m_lastRoute; // 📝 Preserve last attempted route
};

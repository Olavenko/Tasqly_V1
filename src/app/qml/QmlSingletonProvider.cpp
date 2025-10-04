#include "QmlSingletonProvider.h"
#include "app/logging/LogFacade.h"
#include "app/navigation/NavigationService.h"
#include "app/notifier/Notifier.h"
#include "app/settings/AppSettingsManager.h"
#include "app/settings/FeatureFlagsManager.h"
#include "app/theme/ThemeManager.h"
#include "app/usecases/NavigateTo.h"
#include "domain/core/PageState.h"
#include "domain/core/ThemeMode.h"
#include "infra/logging/LogManager.h"
#include <QQmlEngine>

using ThemeMode = tasqly::domain::core::ThemeMode;

// 🚀 Register App.* facades into QML
void QmlSingletonProvider::registerTypes(AppSettingsManager* settings,
                                         ThemeManager* theme,
                                         tasqly::infra::logging::LogManager* log,
                                         QObject* notifier)
{
  qmlRegisterSingletonInstance<AppSettingsManager>("Tasqly_V1", 1, 0, "Settings", settings);
  qmlRegisterSingletonInstance<ThemeManager>("Tasqly_V1", 1, 0, "ThemeManager", theme);

  if (log) {
    auto* logFacade = new LogFacade(log);
    qmlRegisterSingletonInstance<LogFacade>("Tasqly_V1", 1, 0, "Log", logFacade);
  }

  if (notifier) {
    qmlRegisterSingletonInstance<Notifier>("Tasqly_V1",
                                           1,
                                           0,
                                           "Notifier",
                                           static_cast<Notifier*>(notifier));
  }

  // 🎨 Register ThemeMode enum for QML
  qmlRegisterUncreatableType<tasqly::domain::core::ThemeMode>("Tasqly_V1",
                                                              1,
                                                              0,
                                                              "ThemeMode",
                                                              "ThemeMode is an enum only");
  // 🔐 Register navigation pieces only when enabled
  if (FeatureFlagsManager::instance().isEnabled("features.navigation")) {
    // 🧭 NavigationService instance
    auto* navService = new NavigationService(log);

    // 🎯 NavigateTo usecase (depends on NavigationService + Log)
    auto* navigateTo = new NavigateTo(navService, log);
    qmlRegisterSingletonInstance<NavigateTo>("Tasqly_V1", 1, 0, "NavigateTo", navigateTo);
  }

  // 📄 Register PageState enum for QML (dual alias: Uppercase + lowercase)
  qmlRegisterUncreatableType<PageStateWrapper>("Tasqly_V1",
                                               1,
                                               0,
                                               "PageState",
                                               "PageState is an enum and cannot be created in QML");
}

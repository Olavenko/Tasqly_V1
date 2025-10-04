/*
 * 🧱 File: main.cpp
 * -----------------
 * 📌 Purpose   : Application entry point; sets up QML engine and registers App.* facades.
 * 🧱 Layer     : App (Entry / Bootstrap)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-02
 * 🗓️ Updated   : 2025-09-14
 *
 * 🧠 Description:
 * Initializes the Qt application and QML engine, wires a failure handler for object creation,
 * registers core AppErrors & FeatureFlagsManager, and exposes App.* facades
 * (Settings, Theme, Log, Notifier) via QmlSingletonProvider.
 */

#include "app/errors/AppErrors.h"
#include "app/logging/LogFacade.h"
#include "app/notifier/Notifier.h"
#include "app/qml/QmlSingletonProvider.h"
#include "app/settings/AppSettingsManager.h"
#include "app/settings/FeatureFlagsManager.h"
#include "app/theme/ThemeManager.h"

#include "infra/logging/LogManager.h"

// 👉 Qt includes
#include <QGuiApplication>
#include <QJSEngine>
#include <QQmlApplicationEngine>
#include <QQmlEngine>

using namespace Qt::StringLiterals;

// 🚀 Application entry point
int main(int argc, char* argv[])
{
  // 👉 Initialize Qt application
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;

  QCoreApplication::setOrganizationName("Tasqly");
  QCoreApplication::setApplicationName("Tasqly_V1");

  // 🧯 Exit gracefully if the root QML object fails to create
  QObject::connect(
      &engine,
      &QQmlApplicationEngine::objectCreationFailed,
      &app,
      []() { QCoreApplication::exit(-1); },
      Qt::QueuedConnection);

  // 🔎 Register Error type in Qt meta system (signals, QVariant, etc.)
  qRegisterMetaType<tasqly::domain::core::Error>("tasqly::domain::core::Error");

  // 🧩 Register QML Singleton: Tasqly.App → AppErrors
  qmlRegisterSingletonType<tasqly::app::errors::AppErrors>(
      "Tasqly.App", 1, 0, "AppErrors", [](QQmlEngine*, QJSEngine*) -> QObject* {
        return new tasqly::app::errors::AppErrors();
      });

  // 🧩 Register QML Singleton: Tasqly_V1 → Features
  qmlRegisterSingletonInstance("Tasqly_V1", 1, 0, "Features", &FeatureFlagsManager::instance());

  // ✅ Create managers
  auto* settings = new AppSettingsManager(&app);
  auto* theme = new ThemeManager(&app);
  auto* log = new tasqly::infra::logging::LogManager(&app); // Qt will delete it on app destruction
  auto* logFacade = new LogFacade(log, &app);
  auto* notifier = new Notifier(&app);

  // ✅ Register App.* facades (Settings, Theme, Notifier)
  QmlSingletonProvider::registerTypes(settings, theme, log, notifier);

  // 🧩 Register QML Singleton: Tasqly.App → Log
  qmlRegisterSingletonInstance("Tasqly.App", 1, 0, "Log", logFacade);

  // 🚀 Load QML entrypoint (Main.qml directly, because it imports App 1.0)
  const QUrl url(u"qrc:/qt/qml/Tasqly_V1/Main.qml"_s);
  engine.load(url);

  return app.exec();
}

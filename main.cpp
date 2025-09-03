/*
 * 🧱 File: main.cpp
 * -----------------
 * 📌 Purpose   : Application entry point; sets up QML engine and registers AppErrors & Features singletons.
 * 🧱 Layer     : App (Entry / Bootstrap)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-02
 * 🗓️ Updated   : 2025-09-03
 *
 * 🧠 Description:
 * Initializes the Qt application and QML engine, wires a failure handler for object creation,
 * registers the core Error meta-type for queued connections/variants, and exposes the
 * AppErrors registry as a QML Singleton under the module "Tasqly.App".
 * Also registers FeatureFlagsManager as a QML Singleton under "Tasqly_V1".
 */

#include <QGuiApplication>
#include <QJSEngine>
#include <QQmlApplicationEngine>
#include <QQmlEngine>

#include "src/app/errors/AppErrors.h"
#include "src/app/settings/FeatureFlagsManager.h"

// ➜ main application entry point
int main(int argc, char* argv[])
{
  QGuiApplication app(argc, argv);

  QQmlApplicationEngine engine;

  // 🧯 Exit gracefully if the root QML object fails to create
  QObject::connect(
      &engine,
      &QQmlApplicationEngine::objectCreationFailed,
      &app,
      []() { QCoreApplication::exit(-1); },
      Qt::QueuedConnection);

  // 🔎 Make Error known to Qt meta system (queued signals, QVariant, etc.)
  qRegisterMetaType<tasqly::domain::core::Error>("tasqly::domain::core::Error");

  // 🧩 Register QML Singleton: import Tasqly.App 1.0; AppErrors
  qmlRegisterSingletonType<tasqly::app::errors::AppErrors>(
      "Tasqly.App", 1, 0, "AppErrors", [](QQmlEngine*, QJSEngine*) -> QObject* {
        return new tasqly::app::errors::AppErrors();
      });

  // 🧩 Register QML Singleton: import Tasqly_V1 1.0; Features
  qmlRegisterSingletonInstance("Tasqly_V1", 1, 0, "Features", &FeatureFlagsManager::instance());

  // 🚀 Load main QML module
  engine.loadFromModule("Tasqly_V1", "Main");

  return app.exec();
}

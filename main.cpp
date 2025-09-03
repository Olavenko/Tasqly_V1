/*
 * 🧱 File: main.cpp
 * -----------------
 * 📌 Purpose   : Application entry point; sets up QML engine and registers AppErrors singleton.
 * 🧱 Layer     : App (Entry / Bootstrap)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-02
 *
 * 🧠 Description:
 * Initializes the Qt application and QML engine, wires a failure handler for object creation,
 * registers the core Error meta-type for queued connections/variants, and exposes the
 * AppErrors registry as a QML Singleton under the module "Tasqly.App".
 */

#include <QGuiApplication>
#include <QJSEngine>
#include <QQmlApplicationEngine>
#include <QQmlEngine>

#include "src/app/errors/AppErrors.h"

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

  // 🚀 Load main QML module
  engine.loadFromModule("Tasqly_V1", "Main");

  return app.exec();
}

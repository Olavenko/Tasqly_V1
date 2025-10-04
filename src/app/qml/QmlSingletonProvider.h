/*
 * 🧱 File: QmlSingletonProvider.h
 * ------------------------------
 * 📌 Purpose   : Declares QmlSingletonProvider which registers App.* facades in QML.
 * 🧱 Layer     : Application (QML Integration)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-04
 *
 * 🧠 Description:
 * Provides a static entrypoint for registering Settings, Theme, Log, and Notifier
 * facades as QML singletons in the "App" namespace.
 */

#pragma once

#include <QObject>

class AppSettingsManager;
class ThemeManager;
namespace tasqly::infra::logging {
class LogManager;
}

class QmlSingletonProvider
{
public:
  static void registerTypes(AppSettingsManager* settings,
                            ThemeManager* theme,
                            tasqly::infra::logging::LogManager* log,
                            QObject* notifier);
};

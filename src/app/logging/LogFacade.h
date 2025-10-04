/*
 * 🧱 File: LogFacade.h
 * --------------------
 * 📌 Purpose   : QML-facing wrapper around LogManager for safe logging from QML.
 * 🧱 Layer     : Application (Logging Facade)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-04
 *
 * 🧠 Description:
 * Provides minimal logging API exposed to QML (App.Log). Forwards calls to
 * LogManager (infra). Ensures QML always talks to QObject subclass instead
 * of raw infra implementation.
 */

#pragma once

#include "app/logging/ILogManager.h"
#include <QObject>

class LogFacade : public QObject
{
  Q_OBJECT
public:
  explicit LogFacade(tasqly::app::logging::ILogManager* impl, QObject* parent = nullptr);

  // 🪵 Log info message from QML
  Q_INVOKABLE void info(const QString& message);

  // ⚠️ Log warning message from QML
  Q_INVOKABLE void warn(const QString& message);

  // ❌ Log error message from QML
  Q_INVOKABLE void error(const QString& message);

private:
  tasqly::app::logging::ILogManager* m_impl;
};

/*
 * 🧱 File: LogFacade.cpp
 * ----------------------
 * 📌 Purpose   : Implementation of QML-facing LogFacade.
 * 🧱 Layer     : Application (Logging Facade)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-04
 *
 * 🧠 Description:
 * Wraps LogManager infra and exposes minimal API to QML via Q_INVOKABLE methods.
 */

#include "LogFacade.h"

using namespace tasqly::app::logging;

LogFacade::LogFacade(ILogManager* impl, QObject* parent)
    : QObject(parent)
    , m_impl(impl)
{}

void LogFacade::info(const QString& message)
{
  if (m_impl) {
    m_impl->log(Level::Info, "QML", message);
  }
}

void LogFacade::warn(const QString& message)
{
  if (m_impl) {
    m_impl->log(Level::Warn, "QML", message);
  }
}

void LogFacade::error(const QString& message)
{
  if (m_impl) {
    m_impl->log(Level::Error, "QML", message);
  }
}

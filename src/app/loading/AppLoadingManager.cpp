/*
 * 🧱 File: AppLoadingManager.cpp
 * ------------------------------
 * 📌 Purpose   : Implementation of AppLoadingManager.
 * 🧱 Layer     : Application (Loading)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Tracks repository readiness state and notifies listeners (QML/UI).
 */

#include "app/loading/AppLoadingManager.h"

// 🏗️ Constructor
AppLoadingManager::AppLoadingManager(QObject* parent)
    : QObject(parent)
{}

// 🔎 Returns current readiness state
bool AppLoadingManager::repositoriesReady() const
{
  return m_repositoriesReady;
}

// ✅ Updates readiness and emits signal if changed
void AppLoadingManager::setRepositoriesReady(bool ready)
{
  if (m_repositoriesReady == ready)
    return;
  m_repositoriesReady = ready;
  emit repositoriesReadyChanged(m_repositoriesReady);
}

/*
 * 🧱 File: AppLoadingManager.h
 * ----------------------------
 * 📌 Purpose   : Manage application-wide loading states (repositories, settings, etc.)
 * 🧱 Layer     : Application (Loading)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Provides a central manager for tracking the loading state of critical
 * subsystems. Exposes repository readiness state and emits signals to
 * notify QML/UI when loading completes.
 */

#pragma once

#include <QObject>

class AppLoadingManager : public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool repositoriesReady READ repositoriesReady NOTIFY repositoriesReadyChanged)

public:
  explicit AppLoadingManager(QObject* parent = nullptr);

  // 🔎 Returns whether repositories are initialized
  bool repositoriesReady() const;

public slots:
  // ✅ Sets the repository readiness state
  void setRepositoriesReady(bool ready);

signals:
  // 📢 Emitted when repository readiness changes
  void repositoriesReadyChanged(bool ready);

private:
  bool m_repositoriesReady{false};
};

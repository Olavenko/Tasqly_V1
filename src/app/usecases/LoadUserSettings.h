/*
 * 🧱 File: LoadUserSettings.h
 * ----------------------------
 * 📌 Purpose   : Loads persisted user settings (INI) into runtime managers.
 * 🧱 Layer     : Application (UseCase)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-04
 *
 * 🧠 Description:
 * Reads settings from AppSettingsManager and applies them to ThemeManager.
 * Logs load success/failure.
 */

#pragma once

#include "app/settings/AppSettingsManager.h"
#include "app/theme/ThemeManager.h"
#include "infra/logging/LogManager.h"

class LoadUserSettings
{
public:
  LoadUserSettings(AppSettingsManager* settings,
                   ThemeManager* theme,
                   tasqly::infra::logging::LogManager* log);

  // 📥 Executes the use case
  void execute();

private:
  AppSettingsManager* m_settings;
  ThemeManager* m_theme;
  tasqly::infra::logging::LogManager* m_log;
};

/*
 * 🧱 File: SaveUserSettings.h
 * ----------------------------
 * 📌 Purpose   : Persists user settings to INI file.
 * 🧱 Layer     : Application (UseCase)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-04
 *
 * 🧠 Description:
 * Saves runtime state (ThemeMode etc.) from ThemeManager into AppSettingsManager.
 */

#pragma once

#include "app/logging/ILogManager.h"
#include "app/settings/AppSettingsManager.h"
#include "app/theme/ThemeManager.h"

class SaveUserSettings
{
public:
  SaveUserSettings(IAppSettingsManager* settings,
                   ThemeManager* theme,
                   tasqly::app::logging::ILogManager* log);

  // 💾 Executes the use case
  void execute(bool forceSave = false);

private:
  IAppSettingsManager* m_settings;
  ThemeManager* m_theme;
  tasqly::app::logging::ILogManager* m_log;
};

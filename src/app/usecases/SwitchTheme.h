/*
 * 🧱 File: SwitchTheme.h
 * ----------------------
 * 📌 Purpose   : Switches runtime theme and persists new mode.
 * 🧱 Layer     : Application (UseCase)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-04
 *
 * 🧠 Description:
 * Toggles between Light/Dark modes, updates ThemeManager, and persists
 * to AppSettingsManager.
 */

#pragma once

#include "app/settings/AppSettingsManager.h"
#include "app/theme/ThemeManager.h"
#include "infra/logging/LogManager.h"

using ThemeMode = tasqly::domain::core::ThemeMode;

class SwitchTheme
{
public:
  SwitchTheme(AppSettingsManager* settings,
              ThemeManager* theme,
              tasqly::infra::logging::LogManager* log);

  // 🎨 Executes the use case
  void execute(ThemeMode newMode);

private:
  AppSettingsManager* m_settings;
  ThemeManager* m_theme;
  tasqly::infra::logging::LogManager* m_log;
};

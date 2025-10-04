#include "LoadUserSettings.h"
#include "domain/core/ThemeMode.h"

using namespace tasqly::infra::logging;
using ThemeMode = tasqly::domain::core::ThemeMode;

LoadUserSettings::LoadUserSettings(AppSettingsManager* settings,
                                   ThemeManager* theme,
                                   LogManager* log)
    : m_settings(settings)
    , m_theme(theme)
    , m_log(log)
{}

void LoadUserSettings::execute()
{
  try {
    ThemeMode mode = m_settings->themeMode();
    m_theme->switchTo(mode);

    if (m_log)
      m_log->log(LogManager::Level::Info, "UseCase", "✅ Loaded user settings");
  } catch (const std::exception& ex) {
    if (m_log)
      m_log->log(LogManager::Level::Error,
                 "UseCase",
                 QString("❌ Failed to load settings: %1").arg(ex.what()));
  }
}

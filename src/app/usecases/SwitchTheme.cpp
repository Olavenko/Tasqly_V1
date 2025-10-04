#include "app/usecases/SwitchTheme.h"
#include "domain/core/ThemeMode.h"

using namespace tasqly::infra::logging;

SwitchTheme::SwitchTheme(AppSettingsManager* settings, ThemeManager* theme, LogManager* log)
    : m_settings(settings)
    , m_theme(theme)
    , m_log(log)
{}

void SwitchTheme::execute(ThemeMode newMode)
{
  try {
    const auto currentRuntimeMode = m_theme ? m_theme->mode() : ThemeMode::Light;
    const auto currentPersistedMode = m_settings ? m_settings->themeMode() : ThemeMode::Light;

    if (currentRuntimeMode == newMode && currentPersistedMode == newMode)
      return;

    m_theme->switchTo(newMode);
    m_settings->setThemeMode(newMode);
    m_settings->save();

    if (m_log)
      m_log->log(LogManager::Level::Info,
                 "UseCase",
                 QString("🎨 Theme switched to %1")
                     .arg(tasqly::domain::core::ThemeModeUtils::toString(newMode)));
  } catch (const std::exception& ex) {
    if (m_log)
      m_log->log(LogManager::Level::Error,
                 "UseCase",
                 QString("❌ Failed to switch theme: %1").arg(ex.what()));
  }
}

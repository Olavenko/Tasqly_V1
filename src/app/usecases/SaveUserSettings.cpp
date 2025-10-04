#include "SaveUserSettings.h"

using namespace tasqly::app::logging;
using ThemeMode = tasqly::domain::core::ThemeMode;

SaveUserSettings::SaveUserSettings(IAppSettingsManager* settings,
                                   ThemeManager* theme,
                                   ILogManager* log)
    : m_settings(settings)
    , m_theme(theme)
    , m_log(log)
{}

void SaveUserSettings::execute(bool forceSave)
{
  try {
    m_settings->setThemeMode(m_theme->mode());
    m_settings->save(forceSave);
    if (m_log)
      m_log->log(Level::Info, "UseCase", "💾 User settings saved");
  } catch (const std::exception& ex) {
    if (m_log)
      m_log->log(Level::Error, "UseCase", QString("❌ Failed to save settings: %1").arg(ex.what()));
  }
}

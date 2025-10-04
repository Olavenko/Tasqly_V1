#pragma once

#include "app/settings/AppSettingsManager.h"

// Lightweight test double that tracks save invocations.
class FakeAppSettingsManager : public IAppSettingsManager
{
public:
  ThemeMode themeMode() const override { return m_currentMode; }

  void setThemeMode(ThemeMode mode) override { m_currentMode = mode; }

  void save(bool force = false) override
  {
    if (force) {
      ++m_forceCalls;
    } else {
      ++m_deferredCalls;
    }
  }

  void resetCounters()
  {
    m_deferredCalls = 0;
    m_forceCalls = 0;
  }

  int deferredSaveCalls() const { return m_deferredCalls; }

  int forcedSaveCalls() const { return m_forceCalls; }

private:
  ThemeMode m_currentMode{ThemeMode::Light};
  int m_deferredCalls{0};
  int m_forceCalls{0};
};

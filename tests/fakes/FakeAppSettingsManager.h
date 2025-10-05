/*
 * 🧱 File: FakeAppSettingsManager.h
 * ---------------------------------
 * 📌 Purpose   : Unified fake for AppSettingsManager used in tests and benchmarks
 * 🧱 Layer     : Test Utilities (Fakes)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-05
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * - Provides a lightweight in-memory implementation of AppSettingsManager.
 * - Uses FakeSettingsStore (no disk access, no timers).
 * - Tracks deferred and forced save calls for metrics and verification.
 *
 * ✅ Used In : 
 *   - Bench_SaveUserSettings.cpp
 *   - Bench_SwitchTheme.cpp
 *   - test_app_settings_manager.cpp
 */

#pragma once
#include "app/settings/AppSettingsManager.h"
#include "tests/fakes/FakeSettingsStore.h"

// 🎭 Unified fake implementation for benchmarks and tests
class FakeAppSettingsManager : public AppSettingsManager
{
public:
  FakeAppSettingsManager()
      : AppSettingsManager(std::make_unique<FakeSettingsStore>())
  {}

  void save(bool force = false) override
  {
    if (force)
      ++m_forceCalls;
    else
      ++m_deferredCalls;
  }

  void resetCounters()
  {
    m_forceCalls = 0;
    m_deferredCalls = 0;
  }

  int deferredSaveCalls() const { return m_deferredCalls; }
  int forcedSaveCalls() const { return m_forceCalls; }

private:
  int m_forceCalls{0};
  int m_deferredCalls{0};
};

/*
 * 🧱 File: Bench_AppSettingsManager.cpp
 * -------------------------------------
 * 📌 Purpose   : Microbenchmarks for AppSettingsManager persistence using real QSettings.
 * 🧱 Layer     : Benchmarks (Unit / Settings)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-10-05
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * - Measures cold `load()` performance with disk-backed QSettings store.
 * - Measures `setThemeMode()` + `save()` round-trips to capture sync latency.
 * - Uses QStandardPaths test mode to isolate filesystem impact per run.
 * - Ensures generated INI files are cleaned between iterations for determinism.
 *
 * 🔗 Depends On:
 *   - src/app/settings/AppSettingsManager.h
 *   - QtCore (QStandardPaths, QSettings, QDir, QFile)
 *   - Google Benchmark
 * ✅ Tested In : TasqlyBenchmarksRunner (CI target)
 */

#include "app/settings/AppSettingsManager.h"

#include <benchmark/benchmark.h>

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStandardPaths>
#include <QString>

#include <memory>

using tasqly::domain::core::ThemeMode;

namespace {

class BenchmarkableAppSettingsManager : public AppSettingsManager
{
public:
  using AppSettingsManager::AppSettingsManager;

  void stopBenchmarkTimers()
  {
    if (m_debounceTimer && m_debounceTimer->isActive()) {
      m_debounceTimer->stop();
    }
    if (m_periodicSyncTimer && m_periodicSyncTimer->isActive()) {
      m_periodicSyncTimer->stop();
    }
  }
};

void ensureTestMode()
{
  static bool initialized = []() {
    QStandardPaths::setTestModeEnabled(true);
    return true;
  }();
  (void) initialized;
}

QString settingsFilePath()
{
  ensureTestMode();
  const QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
  return configDir + "/settings.ini";
}

void prepareSettingsFile(const QString& theme)
{
  ensureTestMode();
  const QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
  QDir().mkpath(configDir);

  QSettings settings(settingsFilePath(), QSettings::IniFormat);
  settings.setFallbacksEnabled(false);
  settings.setValue("themeMode", theme);
  settings.sync();
}

} // namespace

static void BM_AppSettingsManager_Load(benchmark::State& state)
{
  ensureTestMode();

  for (auto _ : state) {
    state.PauseTiming();
    prepareSettingsFile(QStringLiteral("dark"));
    state.ResumeTiming();

    auto manager = std::make_unique<BenchmarkableAppSettingsManager>();
    manager->stopBenchmarkTimers();
    manager->load();
    benchmark::DoNotOptimize(manager->themeMode());

    state.PauseTiming();
    const QString filePath = manager->settingsFilePath();
    manager->stopBenchmarkTimers();
    manager.reset();
    QFile::remove(filePath);
    state.ResumeTiming();
  }
}
BENCHMARK(BM_AppSettingsManager_Load);

static void BM_AppSettingsManager_SetThemeAndSave(benchmark::State& state)
{
  ensureTestMode();

  for (auto _ : state) {
    state.PauseTiming();
    auto manager = std::make_unique<BenchmarkableAppSettingsManager>();
    manager->stopBenchmarkTimers();
    manager->load();
    state.ResumeTiming();

    manager->setThemeMode(ThemeMode::Dark);
    manager->save();
    manager->setThemeMode(ThemeMode::Light);
    manager->save();

    state.PauseTiming();
    const QString filePath = manager->settingsFilePath();
    manager->stopBenchmarkTimers();
    manager.reset();
    QFile::remove(filePath);
    state.ResumeTiming();
  }
}
BENCHMARK(BM_AppSettingsManager_SetThemeAndSave);

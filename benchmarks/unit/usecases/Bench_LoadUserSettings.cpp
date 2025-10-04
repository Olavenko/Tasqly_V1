/*
 * 🧱 File: Bench_LoadUserSettings.cpp
 * -----------------------------------
 * 📌 Purpose   : Benchmark LoadUserSettings use case (CPU-bound logic only).
 * 🧱 Layer     : Benchmarks (Use Cases)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-03
 *
 * 🧠 Description:
 * Benchmarks the execution of the LoadUserSettings use case with fake managers.
 * Focus is strictly on CPU-bound logic (themeMode lookup + theme switch + logging).
 * All sinks in LogManager are disabled to avoid I/O.
 *
 * 🔗 Depends On:
 *   - app/usecases/LoadUserSettings.h
 *   - app/settings/AppSettingsManager.h
 *   - app/theme/ThemeManager.h
 *   - infra/logging/LogManager.h
 *   - domain/core/ThemeMode.h
 *   - Google Benchmark
 *
 * ✅ Tested In : TasqlyBenchmarksRunner (CI target)
 */

#include "app/settings/AppSettingsManager.h"
#include "app/theme/ThemeManager.h"
#include "app/usecases/LoadUserSettings.h"
#include "infra/logging/LogManager.h"
#include <benchmark/benchmark.h>

using namespace tasqly::domain::core;

// 👉 FakeAppSettingsManager (returns fixed ThemeMode)
class FakeAppSettingsManager : public AppSettingsManager
{
public:
  ThemeMode themeMode() const { return ThemeMode::Dark; }
};

// 👉 FakeThemeManager (no-op switch)
class FakeThemeManager : public ThemeManager
{
public:
  void switchTo(ThemeMode)
  {
    // no-op
  }
};

// 🧪 Benchmark
static void BM_LoadUserSettings(benchmark::State& state)
{
  FakeAppSettingsManager settings;
  FakeThemeManager theme;

  // ✅ Use real LogManager but disable sinks (CPU-bound only)
  tasqly::infra::logging::LogManager log(nullptr);
  log.enableConsole(false);
  log.enableFileSink(false);

  LoadUserSettings usecase(&settings, &theme, &log);

  for (auto _ : state) {
    usecase.execute();
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// 📊 Register
BENCHMARK(BM_LoadUserSettings);

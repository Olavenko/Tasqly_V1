/*
 * 🧱 File: Bench_SaveUserSettings.cpp
 * -----------------------------------
 * 📌 Purpose   : Benchmark SaveUserSettings use case (CPU-bound logic only).
 * 🧱 Layer     : Benchmarks (Use Cases)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-03
 *
 * 🧠 Description:
 * Benchmarks the execution of the SaveUserSettings use case with fake managers.
 * Focus is strictly on CPU-bound logic (themeMode save + logging).
 * All sinks in LogManager are disabled to avoid I/O.
 *
 * 🔗 Depends On:
 *   - app/usecases/SaveUserSettings.h
 *   - app/settings/AppSettingsManager.h
 *   - app/theme/ThemeManager.h
 *   - infra/logging/LogManager.h
 *   - domain/core/ThemeMode.h
 *   - Google Benchmark
 *
 * ✅ Tested In : TasqlyBenchmarksRunner (CI target)
 */

#include "app/theme/ThemeManager.h"
#include "app/usecases/SaveUserSettings.h"
#include "infra/logging/LogManager.h"
#include "tests/fakes/FakeAppSettingsManager.h"
#include <benchmark/benchmark.h>

using namespace tasqly::domain::core;

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
static void BM_SaveUserSettings(benchmark::State& state)
{
  FakeAppSettingsManager settings;
  FakeThemeManager theme;

  // ✅ Use real LogManager but disable sinks (CPU-bound only)
  tasqly::infra::logging::LogManager log(nullptr);
  log.enableConsole(false);
  log.enableFileSink(false);

  SaveUserSettings usecase(&settings, &theme, &log);

  for (auto _ : state) {
    usecase.execute();
  }

  const auto forcedFlushes = settings.forcedSaveCalls();
  if (forcedFlushes != 0) {
    state.SkipWithError("Deferred save benchmark should not trigger forced flushes");
  }
  state.counters["deferred_calls"] = static_cast<double>(settings.deferredSaveCalls());

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// 📊 Register
BENCHMARK(BM_SaveUserSettings);

// 🧪 Benchmark: forced save path
static void BM_SaveUserSettings_ForcedFlush(benchmark::State& state)
{
  FakeAppSettingsManager settings;
  FakeThemeManager theme;

  tasqly::infra::logging::LogManager log(nullptr);
  log.enableConsole(false);
  log.enableFileSink(false);

  SaveUserSettings usecase(&settings, &theme, &log);

  for (auto _ : state) {
    usecase.execute(true);
  }

  const auto forcedFlushes = settings.forcedSaveCalls();
  if (forcedFlushes == 0) {
    state.SkipWithError("Forced save benchmark must record forced flushes");
  }
  state.counters["force_calls"] = static_cast<double>(forcedFlushes);
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

BENCHMARK(BM_SaveUserSettings_ForcedFlush);

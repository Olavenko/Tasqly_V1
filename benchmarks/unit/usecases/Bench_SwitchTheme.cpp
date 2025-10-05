/*
 * 🧱 File: Bench_SwitchTheme.cpp
 * -------------------------------
 * 📌 Purpose   : Benchmark SwitchTheme use case (CPU-bound logic only).
 * 🧱 Layer     : Benchmarks (Use Cases)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-03
 *
 * 🧠 Description:
 * Benchmarks the execution of the SwitchTheme use case with fake managers.
 * Focus is strictly on CPU-bound logic (switch theme + update settings + logging).
 * All sinks in LogManager are disabled to avoid I/O.
 *
 * 🔗 Depends On:
 *   - app/usecases/SwitchTheme.h
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
#include "app/usecases/SwitchTheme.h"
#include "tests/fakes/FakeAppSettingsManager.h"
#include <benchmark/benchmark.h>

#include <QHash>
#include <QString>

using ThemeMode = tasqly::domain::core::ThemeMode;

static void BM_SwitchTheme_Transition(benchmark::State& state)
{
  FakeAppSettingsManager settings;
  ThemeManager theme;
  SwitchTheme usecase(&settings, &theme, nullptr);

  for (auto _ : state) {
    usecase.execute(ThemeMode::Dark);
    usecase.execute(ThemeMode::Light);
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * 2);
}
BENCHMARK(BM_SwitchTheme_Transition);

static void BM_SwitchTheme_NoOp(benchmark::State& state)
{
  FakeAppSettingsManager settings;
  ThemeManager theme;
  SwitchTheme usecase(&settings, &theme, nullptr);

  usecase.execute(ThemeMode::Dark);

  for (auto _ : state) {
    usecase.execute(ThemeMode::Dark);
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// 📊 Register
BENCHMARK(BM_SwitchTheme_NoOp);

/*
 * 🧱 File: Bench_ThemeManager.cpp
 * -------------------------------
 * 📌 Purpose   : Google Benchmark suite for ThemeManager (runtime theme control).
 * 🧱 Layer     : Benchmarks (Unit / Theme)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-22
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * - Measures cost of theme switching, palette retrieval, and fonts retrieval.
 * - Focused on internal logic only (no UI signals connected in Phase0).
 *
 * 🔗 Depends On:
 *   - src/app/theme/ThemeManager.h
 *   - src/domain/core/ThemeMode.h
 * 🔗 Related   : Bench_AppSettingsManager.cpp
 * ✅ Tested In : CI → TasqlyBenchmarksRunner
 * ⚡ Performance: Expected trivial allocations (QVariantMap).
 * 🕒 Last Major Change: 2025-09-22
 */

#include "app/theme/ThemeManager.h"
#include <benchmark/benchmark.h>

using ThemeMode = tasqly::domain::core::ThemeMode;

// 📝 Benchmark: get current mode
static void BM_ThemeManager_Mode(benchmark::State& state)
{
  ThemeManager mgr;
  for (auto _ : state) {
    auto m = mgr.mode();
    benchmark::DoNotOptimize(m);
  }
}
BENCHMARK(BM_ThemeManager_Mode);

// 📝 Benchmark: switchTo (Light <-> Dark)
static void BM_ThemeManager_SwitchMode(benchmark::State& state)
{
  ThemeManager mgr;
  for (auto _ : state) {
    mgr.switchTo(ThemeMode::Dark);
    mgr.switchTo(ThemeMode::Light);
  }
}
BENCHMARK(BM_ThemeManager_SwitchMode);

// 📝 Benchmark: palette getter
static void BM_ThemeManager_Palette(benchmark::State& state)
{
  ThemeManager mgr;
  for (auto _ : state) {
    auto p = mgr.palette();
    benchmark::DoNotOptimize(p);
  }
}
BENCHMARK(BM_ThemeManager_Palette);

// 📝 Benchmark: fonts getter
static void BM_ThemeManager_Fonts(benchmark::State& state)
{
  ThemeManager mgr;
  for (auto _ : state) {
    auto f = mgr.fonts();
    benchmark::DoNotOptimize(f);
  }
}
BENCHMARK(BM_ThemeManager_Fonts);

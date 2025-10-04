/*
 * 🧱 File: Bench_FeatureFlagsManager.cpp
 * --------------------------------------
 * 📌 Purpose   : Google Benchmark suite for FeatureFlagsManager (runtime flags).
 * 🧱 Layer     : Benchmarks (Unit / Settings)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-23
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * - Measures cost of feature flag access and modification.
 * - Focus only on core logic (hash lookups, inserts, signal emission).
 * - No UI layer involved (foundational phase).
 *
 * 🔗 Depends On:
 *   - src/app/settings/FeatureFlagsManager.h
 * 🔗 Related   : Bench_AppSettingsManager.cpp
 * ✅ Tested In : CI → TasqlyBenchmarksRunner
 * ⚡ Performance: Expect O(1) average per op (QHash).
 * 🕒 Last Major Change: 2025-09-23
 */

#include "app/settings/FeatureFlagsManager.h"
#include <benchmark/benchmark.h>
#include <QObject>

// 📝 Benchmark: Check if feature flag is enabled
static void BM_FeatureFlagsManager_Get(benchmark::State& state)
{
  auto& mgr = FeatureFlagsManager::instance();
  for (auto _ : state) {
    bool val = mgr.isEnabled("features.logging");
    benchmark::DoNotOptimize(val);
  }
}
BENCHMARK(BM_FeatureFlagsManager_Get);

// 📝 Benchmark: Toggle feature flag
static void BM_FeatureFlagsManager_SetToggle(benchmark::State& state)
{
  auto& mgr = FeatureFlagsManager::instance();
  for (auto _ : state) {
    mgr.setFeature("features.logging", true);
    mgr.setFeature("features.logging", false);
  }
}
BENCHMARK(BM_FeatureFlagsManager_SetToggle);

// 📝 Benchmark: Bulk insert new flags (stress test)
static void BM_FeatureFlagsManager_BulkInsert(benchmark::State& state)
{
  auto& mgr = FeatureFlagsManager::instance();
  for (auto _ : state) {
    for (int i = 0; i < 1000; ++i) {
      mgr.setFeature(QString("features.test.%1").arg(i), i % 2 == 0);
    }
  }
}
BENCHMARK(BM_FeatureFlagsManager_BulkInsert);

// 📝 Benchmark: Signal emission cost
static void BM_FeatureFlagsManager_Signal(benchmark::State& state)
{
  auto& mgr = FeatureFlagsManager::instance();
  QObject::connect(&mgr,
                   &FeatureFlagsManager::featureChanged,
                   [](const QString&, bool) { /* no-op slot */ });

  for (auto _ : state) {
    mgr.setFeature("features.signal.test", true);
    mgr.setFeature("features.signal.test", false);
  }
}
BENCHMARK(BM_FeatureFlagsManager_Signal);

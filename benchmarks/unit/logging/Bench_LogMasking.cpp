/*
 * 🧱 File: Bench_LogMasking.cpp
 * -----------------------------
 * 📌 Purpose   : Benchmark overhead of context masking in LogManager.
 * 🧱 Layer     : Benchmark (Performance)
 * 👤 Author    : Mohamed Ali / Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-23
 * 🔖 Version   : 1.1 (Repetitions + Unified iterations)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Benchmarks the additional overhead introduced when logging with
 * different sizes of context (masking key-value pairs).
 *
 * Scenarios:
 *   - No context
 *   - Small context (10 keys)
 *   - Large context (100 keys)
 *
 * Metrics:
 *   - Average latency per log
 *   - Relative overhead vs no-context baseline
 *
 * 🔗 Depends On:
 *   - infra/logging/LogManager.h
 *   - <benchmark/benchmark.h>
 * ✅ Tested In : benchmarks/perf/Bench_LogMasking.cpp
 * ⚡ Performance: Stress-tests LogManager with masked contexts
 * 🔒 Security  : No sensitive data is logged
 */

#include "infra/logging/LogManager.h"
#include <benchmark/benchmark.h>

using namespace tasqly::infra::logging;

// ⚙️ Shared helper to configure LogManager
static std::unique_ptr<LogManager> makeLogManager()
{
  auto lm = std::make_unique<LogManager>();
  lm->setLevel(LogManager::Level::Info);
  lm->enableConsole(false);
  lm->enableFileSink(false, ""); // disable sinks, measure raw overhead
  lm->setAutoFlush(false);
  return lm;
}

// 🛠️ Helper to generate contexts
static QVariantMap makeContext(int size)
{
  QVariantMap ctx;
  for (int i = 0; i < size; ++i) {
    ctx.insert(QString("key%1").arg(i), QString("value%1").arg(i));
  }
  return ctx;
}

// 📝 Benchmarks
static void BM_LogMasking_NoContext(benchmark::State& state)
{
  auto logManager = makeLogManager();
  for (auto _ : state) {
    logManager->log(LogManager::Level::Info, "Bench", "Message without context");
  }
  state.SetItemsProcessed(state.iterations());
}

static void BM_LogMasking_SmallContext(benchmark::State& state)
{
  auto logManager = makeLogManager();
  auto ctx = makeContext(10);
  for (auto _ : state) {
    logManager->log(LogManager::Level::Info, "Bench", "Message with small context", ctx);
  }
  state.SetItemsProcessed(state.iterations());
}

static void BM_LogMasking_LargeContext(benchmark::State& state)
{
  auto logManager = makeLogManager();
  auto ctx = makeContext(100);
  for (auto _ : state) {
    logManager->log(LogManager::Level::Info, "Bench", "Message with large context", ctx);
  }
  state.SetItemsProcessed(state.iterations());
}

// ⚡ Register benchmarks (unified 1M iterations + repetitions)
BENCHMARK(BM_LogMasking_NoContext)
    ->Iterations(1'000'000)
    ->Unit(benchmark::kNanosecond)
    ->Repetitions(5);

BENCHMARK(BM_LogMasking_SmallContext)
    ->Iterations(1'000'000)
    ->Unit(benchmark::kNanosecond)
    ->Repetitions(5);

BENCHMARK(BM_LogMasking_LargeContext)
    ->Iterations(1'000'000)
    ->Unit(benchmark::kNanosecond)
    ->Repetitions(5);

/*
 * 🧱 File: Bench_LogManager_Throughput.cpp
 * ----------------------------------------
 * 📌 Purpose   : Benchmark LogManager throughput (CPU-bound logging only).
 * 🧱 Layer     : Benchmarks (Logging)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-03
 *
 * 🧠 Description:
 * Benchmarks the throughput of LogManager::log() when logging many messages.
 * Focus is strictly on CPU-bound cost of formatting + routing (no sinks).
 * Both console and file sinks are disabled.
 *
 * 🔗 Depends On:
 *   - infra/logging/LogManager.h
 *   - domain/core/Error.h
 *   - Google Benchmark
 *
 * ✅ Tested In : TasqlyBenchmarksRunner (CI target)
 */

#include "infra/logging/LogManager.h"
#include <benchmark/benchmark.h>

using namespace tasqly::app::logging;

// 🧪 Benchmark: log throughput
static void BM_LogManager_Throughput(benchmark::State& state)
{
  // ✅ Prepare LogManager with all sinks disabled
  tasqly::infra::logging::LogManager log(nullptr);
  log.enableConsole(false);
  log.enableFileSink(false);

  QString category = "bench.log";
  QString message = "This is a benchmark log entry with some text";

  for (auto _ : state) {
    for (int i = 0; i < state.range(0); ++i) {
      log.log(Level::Info, category, message);
    }
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * state.range(0));
}

// 📊 Register with different batch sizes
BENCHMARK(BM_LogManager_Throughput)->Arg(100)->Arg(1000)->Arg(10000);

/*
 * 🧱 File: Bench_LogContinuous.cpp
 * -------------------------------
 * 📌 Purpose   : Benchmark throughput of continuous logging using LogManager.
 * 🧱 Layer     : Benchmark (Performance)
 * 👤 Author    : Mohamed Ali / Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-23
 * 🔖 Version   : 1.4 (Cleanup + Param sizes)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Benchmarks continuous logging performance of LogManager across safe modes:
 *   - NullSink (no I/O)
 *   - File only
 *
 * Metrics:
 *   - Average time per log message
 *   - Messages per second (throughput)
 *
 * 🔗 Depends On:
 *   - infra/logging/LogManager.h
 *   - <benchmark/benchmark.h>
 * ✅ Tested In : benchmarks/perf/Bench_LogContinuous.cpp
 * ⚡ Performance: Stress-tests LogManager under 1M log entries
 * 🔒 Security  : No sensitive data is logged
 */

#include "infra/logging/LogManager.h"
#include <benchmark/benchmark.h>
#include <QDir>
#include <QString>

using namespace tasqly::infra::logging;

static const char* kBenchLogPath = "reports/benchmarks/logs/bench_logcontinuous.log";

// ⚙️ Helper to configure LogManager for benchmark (no console spam)
static std::unique_ptr<LogManager> makeLogManager(bool enableFile)
{
  auto lm = std::make_unique<LogManager>();
  lm->setLevel(LogManager::Level::Info);

  // ❌ disable console sink to avoid spam
  lm->enableConsole(false);

  if (enableFile) {
    lm->enableFileSink(true, kBenchLogPath);
  } else {
    // 📝 no sink → acts like null sink (safe for throughput measurement)
    lm->enableFileSink(false, "");
  }

  lm->setAutoFlush(false); // ⚡ disable flush for throughput

  return lm;
}

// 📝 Benchmark continuous logging throughput (NullSink)
static void BM_LogContinuous_NullSink(benchmark::State& state)
{
  auto logManager = makeLogManager(false); // no console, no file

  // 🪵 Prepare message of given size
  int logSize = state.range(0);
  std::string sampleLog(logSize, 'X');
  QString message = QString::fromStdString(sampleLog);

  for (auto _ : state) {
    logManager->log(LogManager::Level::Info, "Bench", message);
  }

  state.SetItemsProcessed(state.iterations());

  // 🧹 cleanup not needed (no file)
}

// 📝 Benchmark continuous logging throughput (FileOnly)
static void BM_LogContinuous_FileOnly(benchmark::State& state)
{
  auto logManager = makeLogManager(true); // file sink enabled

  // 🪵 Prepare message of given size
  int logSize = state.range(0);
  std::string sampleLog(logSize, 'X');
  QString message = QString::fromStdString(sampleLog);

  for (auto _ : state) {
    logManager->log(LogManager::Level::Info, "Bench", message);
  }

  state.SetItemsProcessed(state.iterations());

  // 🧹 Clean up file after benchmark
  QFile::remove(kBenchLogPath);
}

// ⚡ Register benchmarks
BENCHMARK(BM_LogContinuous_NullSink)
    ->Iterations(1'000'000)
    ->Unit(benchmark::kMillisecond)
    ->Args({128})    // tiny log
    ->Args({1024})   // normal log
    ->Args({4096})   // big log
    ->Args({16384}); // very big log

BENCHMARK(BM_LogContinuous_FileOnly)
    ->Iterations(1'000'000)
    ->Unit(benchmark::kMillisecond)
    ->Args({128})
    ->Args({1024})
    ->Args({4096})
    ->Args({16384});

/*
 * 🧱 File: Bench_LogFacade.cpp
 * ----------------------------
 * 📌 Purpose   : Google Benchmark suite for LogFacade (QML-facing logging API).
 * 🧱 Layer     : Benchmarks (Unit / Logging)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-22
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * - Measures cost of logging through LogFacade (info, warn, error).
 * - Uses NullLogger stub to avoid I/O while exercising delegation paths.
 *
 * 🔗 Depends On:
 *   - src/app/logging/LogFacade.h
 *   - src/app/logging/ILogManager.h
 * 🔗 Related   : Bench_ErrorReporter.cpp, Bench_FirstLogWrite.cpp
 * ✅ Tested In : CI → TasqlyBenchmarksRunner
 * ⚡ Performance: Expected trivial overhead (forwarding to ILogManager).
 * 🕒 Last Major Change: 2025-09-22
 */

#include "app/logging/ILogManager.h"
#include "app/logging/LogFacade.h"
#include <benchmark/benchmark.h>

using namespace tasqly::app::logging;

// 🧩 NullLogger: implements ILogManager with no-op methods
class NullLogger final : public ILogManager
{
public:
  void setLevel(Level) override {}
  Level level() const override { return Level::Info; }
  bool isEnabled(Level) const override { return true; }
  void enableConsole(bool) override {}
  void enableFileSink(bool, const QString& = QString()) override {}
  void setRotationPolicy(const RotationPolicy&) override {}
  void rotateNow() override {}
  void flush() override {}
  void addMaskedKey(const QString&) override {}
  void setMaskedKeys(const QStringList&) override {}
  QStringList maskedKeys() const override { return {}; }
  void log(Level, const QString&, const QString&, const QVariantMap& = {}) override {}
};

// 📝 Benchmark: LogFacade::info
static void BM_LogFacade_Info(benchmark::State& state)
{
  NullLogger logger;
  LogFacade facade(&logger);
  for (auto _ : state) {
    facade.info(QStringLiteral("Info message"));
  }
}
BENCHMARK(BM_LogFacade_Info);

// 📝 Benchmark: LogFacade::warn
static void BM_LogFacade_Warn(benchmark::State& state)
{
  NullLogger logger;
  LogFacade facade(&logger);
  for (auto _ : state) {
    facade.warn(QStringLiteral("Warn message"));
  }
}
BENCHMARK(BM_LogFacade_Warn);

// 📝 Benchmark: LogFacade::error
static void BM_LogFacade_Error(benchmark::State& state)
{
  NullLogger logger;
  LogFacade facade(&logger);
  for (auto _ : state) {
    facade.error(QStringLiteral("Error message"));
  }
}
BENCHMARK(BM_LogFacade_Error);

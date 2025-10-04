/*
 * 🧱 File: Bench_NavigateTo.cpp
 * -----------------------------
 * 📌 Purpose   : Google Benchmark suite for NavigateTo use case (guarded navigation).
 * 🧱 Layer     : Benchmarks (Unit / Navigation)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-22
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * - Measures cost of executing navigation requests and retry logic.
 * - Covers valid/invalid execute() paths and retry() behavior.
 * - Signals (pageStateChanged) are emitted implicitly but not connected (Phase0: no UI).
 *
 * 🔗 Depends On:
 *   - src/app/navigation/NavigateTo.h
 *   - src/app/navigation/NavigationService.h
 *   - src/domain/core/Route.h
 *   - src/domain/core/PageState.h
 * 🔗 Related   : Bench_NavigationService.cpp
 * ✅ Tested In : CI → TasqlyBenchmarksRunner
 * ⚡ Performance: Expected trivial (map→Route conversion, stack ops).
 * 🕒 Last Major Change: 2025-09-22
 */

#include "app/logging/ILogManager.h"
#include "app/navigation/NavigationService.h"
#include "app/usecases/NavigateTo.h"
#include <benchmark/benchmark.h>

using namespace tasqly::app::logging;

// 🧩 NullLogger stub
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

// 🧩 Helper: make valid data for execute()
static QVariantMap makeValidData(const QString& id)
{
  QVariantMap map;
  map.insert("id", id);
  map.insert("timestamp", QDateTime::currentDateTime());
  return map;
}

// 📝 Benchmark: execute() with valid data
static void BM_NavigateTo_ExecuteValid(benchmark::State& state)
{
  NullLogger logger;
  NavigationService nav(&logger);
  NavigateTo useCase(&nav, &logger);

  auto data = makeValidData("home");
  for (auto _ : state) {
    useCase.execute(data);
  }
}
BENCHMARK(BM_NavigateTo_ExecuteValid);

// 📝 Benchmark: execute() with invalid data
static void BM_NavigateTo_ExecuteInvalid(benchmark::State& state)
{
  NullLogger logger;
  NavigationService nav(&logger);
  NavigateTo useCase(&nav, &logger);

  QVariantMap data; // empty → invalid route
  for (auto _ : state) {
    useCase.execute(data);
  }
}
BENCHMARK(BM_NavigateTo_ExecuteInvalid);

// 📝 Benchmark: retry() after valid execute
static void BM_NavigateTo_RetryValid(benchmark::State& state)
{
  NullLogger logger;
  NavigationService nav(&logger);
  NavigateTo useCase(&nav, &logger);

  auto data = makeValidData("profile");
  useCase.execute(data); // store lastRoute

  for (auto _ : state) {
    useCase.retry();
  }
}
BENCHMARK(BM_NavigateTo_RetryValid);

// 📝 Benchmark: retry() without valid route
static void BM_NavigateTo_RetryInvalid(benchmark::State& state)
{
  NullLogger logger;
  NavigationService nav(&logger);
  NavigateTo useCase(&nav, &logger);

  for (auto _ : state) {
    useCase.retry();
  }
}
BENCHMARK(BM_NavigateTo_RetryInvalid);

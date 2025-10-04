/*
 * 🧱 File: Bench_NavigationService.cpp
 * ------------------------------------
 * 📌 Purpose   : Google Benchmark suite for NavigationService (stack-based navigation).
 * 🧱 Layer     : Benchmarks (Unit / Navigation)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-22
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * - Measures cost of navigation operations: navigateTo, goBack, currentRoute, retryLast.
 * - Uses NullLogger stub to avoid I/O while exercising code paths.
 * - Notifier/Signals are emitted implicitly but not connected (Phase0: no UI).
 *
 * 🔗 Depends On:
 *   - src/app/navigation/NavigationService.h
 *   - src/domain/core/Route.h
 * 🔗 Related   : Bench_AppLoadingManager.cpp
 * ✅ Tested In : CI → TasqlyBenchmarksRunner
 * ⚡ Performance: Expected trivial (stack ops + QString).
 * 🕒 Last Major Change: 2025-09-22
 */

#include "app/logging/ILogManager.h"
#include "app/navigation/NavigationService.h"
#include "domain/core/Route.h"
#include <benchmark/benchmark.h>

using namespace tasqly::app::logging;

// 🧩 NullLogger for NavigationService
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

// 🧩 Helper: construct a valid Route
static Route makeValidRoute(const QString& id)
{
  return Route{id, QDateTime::currentDateTime()};
}

// 📝 Benchmark: navigateTo(valid route)
static void BM_NavigationService_NavigateValid(benchmark::State& state)
{
  NullLogger logger;
  NavigationService nav(&logger);

  Route valid = makeValidRoute("home");
  for (auto _ : state) {
    nav.navigateTo(valid);
  }
}
BENCHMARK(BM_NavigationService_NavigateValid);

// 📝 Benchmark: navigateTo(invalid route)
static void BM_NavigationService_NavigateInvalid(benchmark::State& state)
{
  NullLogger logger;
  NavigationService nav(&logger);

  Route invalid; // default constructed → id empty, timestamp invalid
  for (auto _ : state) {
    nav.navigateTo(invalid);
  }
}
BENCHMARK(BM_NavigationService_NavigateInvalid);

// 📝 Benchmark: goBack (with stack)
static void BM_NavigationService_GoBack(benchmark::State& state)
{
  NullLogger logger;
  NavigationService nav(&logger);

  Route valid = makeValidRoute("dashboard");
  nav.navigateTo(valid);
  for (auto _ : state) {
    nav.goBack();
    nav.navigateTo(valid);
  }
}
BENCHMARK(BM_NavigationService_GoBack);

// 📝 Benchmark: currentRoute
static void BM_NavigationService_CurrentRoute(benchmark::State& state)
{
  NullLogger logger;
  NavigationService nav(&logger);

  Route valid = makeValidRoute("settings");
  nav.navigateTo(valid);
  for (auto _ : state) {
    auto r = nav.currentRoute();
    benchmark::DoNotOptimize(r);
  }
}
BENCHMARK(BM_NavigationService_CurrentRoute);

// 📝 Benchmark: retryLast
static void BM_NavigationService_RetryLast(benchmark::State& state)
{
  NullLogger logger;
  NavigationService nav(&logger);

  Route valid = makeValidRoute("profile");
  nav.navigateTo(valid);
  for (auto _ : state) {
    nav.retryLast();
  }
}
BENCHMARK(BM_NavigationService_RetryLast);

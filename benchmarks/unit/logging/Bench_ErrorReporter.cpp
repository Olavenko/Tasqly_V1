/*
 * 🧱 File: Bench_ErrorReporter.cpp
 * -------------------------------
 * 📌 Purpose   : Google Benchmark suite for ErrorReporter use-case.
 * 🧱 Layer     : Benchmarks (Unit / Logging)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-22
 * 🔖 Version   : 1.3 (Iterations + Repetitions + Nanoseconds)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * - Measures performance of ErrorReporter when reporting errors.
 * - Covers structured report() and reportRaw() APIs.
 * - Uses NullLogger stub to avoid I/O and exercise code paths deterministically.
 *
 * 🔗 Depends On:
 *   - src/app/logging/ErrorReporter.h
 *   - src/domain/core/Error.h
 * 🔗 Related   : Bench_FirstLogWrite.cpp (logging baseline)
 * ✅ Tested In : CI → TasqlyBenchmarksRunner
 * ⚡ Performance: Expected trivial overhead (QString copies, QVariantMap merge).
 * 🕒 Last Major Change: 2025-09-25
 */

#include "app/logging/ErrorReporter.h"
#include "domain/core/Error.h"
#include <benchmark/benchmark.h>

using namespace tasqly::app::logging;
using tasqly::domain::core::Error;

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

// 📝 Benchmark: report() with structured Error
static void BM_ErrorReporter_Report(benchmark::State& state)
{
  auto logger = std::make_shared<NullLogger>();
  ErrorReporter reporter(logger);

  Error e(123, QStringLiteral("Benchmark error"), {{"ctx", "val"}});
  for (auto _ : state) {
    reporter.report(e, Severity::Error, QStringLiteral("bench"));
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_ErrorReporter_Report)
    ->Iterations(1'000'000)
    ->Repetitions(5)
    ->Unit(benchmark::kNanosecond);

// 📝 Benchmark: reportRaw()
static void BM_ErrorReporter_ReportRaw(benchmark::State& state)
{
  auto logger = std::make_shared<NullLogger>();
  ErrorReporter reporter(logger);

  for (auto _ : state) {
    reporter.reportRaw(404, QStringLiteral("Not Found"), Severity::Warning, QStringLiteral("bench"));
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_ErrorReporter_ReportRaw)
    ->Iterations(1'000'000)
    ->Repetitions(5)
    ->Unit(benchmark::kNanosecond);

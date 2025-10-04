/*
 * 🧱 File: bench_main.cpp
 * -----------------------
 * 📌 Purpose   : Entry point for Google Benchmark suite in Tasqly project.
 * 🧱 Layer     : Benchmarks (Runner Main)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-16
 * 🗓️ Updated   : 2025-09-28
 *
 * 🧠 Description:
 * Provides the main entry point for all benchmarks (unit + integration).
 * Unlike BENCHMARK_MAIN(), this version initializes a QCoreApplication
 * to allow Qt-based managers (AppSettingsManager, ThemeManager, Notifier)
 * to safely use timers, signals, and event loop features.
 *
 * Additionally:
 * - Ensures all benchmark-related logs are redirected into
 *   `reports/benchmarks/logs/` instead of project root.
 * - Cleans up old benchmark log files before each run to avoid clutter.
 *
 * 🔗 Depends On:
 *   - QtCore (QCoreApplication, QDir, QFileInfoList)
 *   - Google Benchmark
 *   - tasqly::infra::logging::LogManager
 * 🔗 Related   : Bench_* files in benchmarks/unit and benchmarks/integration
 * ✅ Tested In : TasqlyBenchmarksRunner (CI target)
 * ⚡ Performance: Adds minimal overhead (single event loop init + cleanup).
 * 🕒 Last Major Change: 2025-09-28
 */

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfoList>

#include <benchmark/benchmark.h>

#include "infra/logging/LogManager.h"

int main(int argc, char** argv)
{
  // ✅ Bootstrap Qt event loop (required for QObject timers/signals)
  QCoreApplication app(argc, argv);

  // 🧹 Ensure clean log directory for benchmarks
  QDir logDir("reports/benchmarks/logs");
  if (!logDir.exists()) {
    logDir.mkpath(".");
  }

  // Remove any old log files in this directory
  const QFileInfoList files = logDir.entryInfoList(QStringList() << "*.log*", QDir::Files);
  for (const QFileInfo& fi : files) {
    QFile::remove(fi.absoluteFilePath());
  }

  // 🔧 Redirect LogManager to benchmarks log file
  auto* logManager = new tasqly::infra::logging::LogManager(&app);
  QString benchLogFile = logDir.absoluteFilePath("benchmarks.log");
  logManager->enableFileSink(true, benchLogFile);

  // 🚀 Initialize Google Benchmark
  benchmark::Initialize(&argc, argv);
  if (benchmark::ReportUnrecognizedArguments(argc, argv)) {
    return 1;
  }

  benchmark::RunSpecifiedBenchmarks();

  // 🧹 Delete entire benchmark logs folder after run
  if (logDir.exists()) {
    logDir.removeRecursively();
  }

  return 0;
}
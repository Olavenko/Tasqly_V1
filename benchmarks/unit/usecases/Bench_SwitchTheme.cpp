/*
 * 🧱 File: Bench_SwitchTheme.cpp
 * -------------------------------
 * 📌 Purpose   : Benchmark SwitchTheme use case (CPU-bound logic only).
 * 🧱 Layer     : Benchmarks (Use Cases)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-03
 *
 * 🧠 Description:
 * Benchmarks the execution of the SwitchTheme use case with fake managers.
 * Focus is strictly on CPU-bound logic (switch theme + update settings + logging).
 * All sinks in LogManager are disabled to avoid I/O.
 *
 * 🔗 Depends On:
 *   - app/usecases/SwitchTheme.h
 *   - app/settings/AppSettingsManager.h
 *   - app/theme/ThemeManager.h
 *   - infra/logging/LogManager.h
 *   - domain/core/ThemeMode.h
 *   - Google Benchmark
 *
 * ✅ Tested In : TasqlyBenchmarksRunner (CI target)
 */

#include "app/settings/AppSettingsManager.h"
#include "app/theme/ThemeManager.h"
#include "app/usecases/SwitchTheme.h"
#include <benchmark/benchmark.h>

#include <QHash>
#include <QString>

using ThemeMode = tasqly::domain::core::ThemeMode;

// 👉 FakeAppSettingsManager (in-memory only)
namespace {

class InMemorySettingsStore : public ISettingsStore
{
public:
  QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const override
  {
    return m_values.value(key, defaultValue);
  }

  void setValue(const QString& key, const QVariant& value) override { m_values.insert(key, value); }

  void sync() override {}

  QSettings::Status status() const override { return QSettings::NoError; }

  QString fileName() const override { return QStringLiteral("bench-in-memory"); }

private:
  QHash<QString, QVariant> m_values;
};

// 🎭 Fake settings manager for benchmark
class BenchSettingsManager : public AppSettingsManager
{
public:
  BenchSettingsManager(QObject* parent = nullptr)
      : AppSettingsManager(parent)
  {}

  void save(bool force = false) override
  {
    lastForce = force;
    saved = true;
  }

  bool saved{false};
  bool lastForce{false};
};

} // namespace

static void BM_SwitchTheme_Transition(benchmark::State& state)
{
  BenchSettingsManager settings;
  ThemeManager theme;
  SwitchTheme usecase(&settings, &theme, nullptr);

  for (auto _ : state) {
    usecase.execute(ThemeMode::Dark);
    usecase.execute(ThemeMode::Light);
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * 2);
}
BENCHMARK(BM_SwitchTheme_Transition);

static void BM_SwitchTheme_NoOp(benchmark::State& state)
{
  BenchSettingsManager settings;
  ThemeManager theme;
  SwitchTheme usecase(&settings, &theme, nullptr);

  usecase.execute(ThemeMode::Dark);

  for (auto _ : state) {
    usecase.execute(ThemeMode::Dark);
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// 📊 Register
BENCHMARK(BM_SwitchTheme_NoOp);

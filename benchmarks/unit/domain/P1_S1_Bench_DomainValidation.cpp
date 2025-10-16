/*
 * 🧱 File: P1_S1_Bench_DomainValidation.cpp
 * -----------------------------------------
 * 📌 Purpose   : Benchmarks for DomainValidation (title & deadline checks)
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-15
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Measures performance of:
 *   - Title validation (short / long / empty)
 *   - Deadline validation (valid / past / future)
 *
 * 🧭 Namespace : tasqly::p1::s1::benchmarks::domain::validation
 * 🧪 Benchmark Tool : Google Benchmark
 */

#include "domain/core/entities/P1_DomainValidation.h"
#include <benchmark/benchmark.h>

using namespace tasqly::domain::core::v1;
namespace tasqly::p1::s1::benchmarks::domain::validation {

// 🧪 Benchmark: Validate 1000 titles (short, valid)
static void BM_ValidateTitle_Valid_1000(benchmark::State& state)
{
  std::string title = "My Task";
  for (auto _ : state) {
    for (int i = 0; i < 1000; ++i)
      benchmark::DoNotOptimize(DomainValidation::validateTitle(title));
  }
}
BENCHMARK(BM_ValidateTitle_Valid_1000)->Unit(benchmark::kMicrosecond);

// 🧪 Benchmark: Validate 1000 titles (empty)
static void BM_ValidateTitle_Empty_1000(benchmark::State& state)
{
  std::string title = "";
  for (auto _ : state) {
    for (int i = 0; i < 1000; ++i)
      benchmark::DoNotOptimize(DomainValidation::validateTitle(title));
  }
}
BENCHMARK(BM_ValidateTitle_Empty_1000)->Unit(benchmark::kMicrosecond);

// 🧪 Benchmark: Validate 1000 titles (very long)
static void BM_ValidateTitle_Long_1000(benchmark::State& state)
{
  std::string longTitle(250, 'A');
  for (auto _ : state) {
    for (int i = 0; i < 1000; ++i)
      benchmark::DoNotOptimize(DomainValidation::validateTitle(longTitle));
  }
}
BENCHMARK(BM_ValidateTitle_Long_1000)->Unit(benchmark::kMicrosecond);

// 🧪 Benchmark: Validate 1000 deadlines (future)
static void BM_ValidateDeadline_Future_1000(benchmark::State& state)
{
  using namespace std::chrono;
  auto future = system_clock::now() + hours(24);
  for (auto _ : state) {
    for (int i = 0; i < 1000; ++i)
      benchmark::DoNotOptimize(DomainValidation::validateDeadline(future));
  }
}
BENCHMARK(BM_ValidateDeadline_Future_1000)->Unit(benchmark::kMicrosecond);

// 🧪 Benchmark: Validate 1000 deadlines (past)
static void BM_ValidateDeadline_Past_1000(benchmark::State& state)
{
  using namespace std::chrono;
  auto past = system_clock::now() - hours(24);
  for (auto _ : state) {
    for (int i = 0; i < 1000; ++i)
      benchmark::DoNotOptimize(DomainValidation::validateDeadline(past));
  }
}
BENCHMARK(BM_ValidateDeadline_Past_1000)->Unit(benchmark::kMicrosecond);

} // namespace tasqly::p1::s1::benchmarks::domain::validation

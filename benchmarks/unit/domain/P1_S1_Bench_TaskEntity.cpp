/*
 * 🧱 File: P1_S1_Bench_TaskEntity.cpp
 * -----------------------------------
 * 📌 Purpose   : Benchmarks for Task entity (construction, copy, move)
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-15
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Measures:
 *   - Task construction cost (with UUID + timestamps)
 *   - Copy performance
 *   - Move performance
 *
 * 🧭 Namespace : tasqly::p1::s1::benchmarks::domain::entity
 * 🧪 Benchmark Tool : Google Benchmark
 */

#include "domain/core/entities/P1_Task.h"
#include <benchmark/benchmark.h>

using namespace tasqly::p1::s1::domain::core;
namespace tasqly::p1::s1::benchmarks::domain::entity {

// 🧪 Benchmark: Task construction
static void BM_Task_Construct(benchmark::State& state)
{
  for (auto _ : state) {
    benchmark::DoNotOptimize(Task());
  }
}
BENCHMARK(BM_Task_Construct)->Unit(benchmark::kMicrosecond);

// 🧪 Benchmark: Task copy
static void BM_Task_Copy(benchmark::State& state)
{
  Task original;
  original.title = "Original Task";
  for (auto _ : state) {
    Task copy = original;
    benchmark::DoNotOptimize(copy);
  }
}
BENCHMARK(BM_Task_Copy)->Unit(benchmark::kMicrosecond);

// 🧪 Benchmark: Task move
static void BM_Task_Move(benchmark::State& state)
{
  for (auto _ : state) {
    Task temp;
    Task moved = std::move(temp);
    benchmark::DoNotOptimize(moved);
  }
}
BENCHMARK(BM_Task_Move)->Unit(benchmark::kMicrosecond);

// 🧪 Benchmark: UUID generation (utility only)
static void BM_GenerateUuid_1000(benchmark::State& state)
{
  for (auto _ : state) {
    for (int i = 0; i < 1000; ++i)
      benchmark::DoNotOptimize(generateUuid());
  }
}
BENCHMARK(BM_GenerateUuid_1000)->Unit(benchmark::kMillisecond);

} // namespace tasqly::p1::s1::benchmarks::domain::entity

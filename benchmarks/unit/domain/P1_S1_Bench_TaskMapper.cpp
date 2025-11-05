/*
 * 🧱 File: P1_S1_Bench_TaskMapper.cpp
 * -----------------------------------
 * 📌 Purpose   : Benchmarks for TaskMapper (Task ↔ TaskDto conversions)
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-15
 * 🔖 Version   : 1.1 (Namespace Fix + Qualified Calls)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Measures performance of:
 *   - Task → TaskDto conversion
 *   - TaskDto → Task conversion
 *   - Time helpers (ISO ↔ chrono)
 *
 * 🧭 Namespace : tasqly::p1::s1::benchmarks::domain::mapper
 * 🧪 Benchmark Tool : Google Benchmark
 */

#include "domain/core/entities/P1_Task.h"
#include "domain/core/mappers/P1_TaskDto.h"
#include "domain/core/mappers/P1_TaskMapper.h"
#include <benchmark/benchmark.h>

using namespace tasqly::p1::s1::domain::core;

// ==========================================================
// Internal Friend Accessor (must match TaskMapper friend decl)
// ==========================================================
namespace tasqly::p1::s1::domain::core {
class TaskMapperBenchAccess
{
public:
  static std::string timePointToIso(const std::chrono::system_clock::time_point& tp)
  {
    return TaskMapper::timePointToIso(tp);
  }

  static std::chrono::system_clock::time_point isoToTimePoint(const std::string& iso)
  {
    return TaskMapper::isoToTimePoint(iso);
  }
};
} // namespace tasqly::p1::s1::domain::core

// ==========================================================
// Benchmarks
// ==========================================================
namespace tasqly::p1::s1::benchmarks::domain::mapper {

// 🧪 Benchmark: Task → TaskDto (1000 iterations)
static void BM_TaskMapper_ToDto_1000(benchmark::State& state)
{
  std::vector<Task> tasks;
  tasks.reserve(1000);
  for (int i = 0; i < 1000; ++i) {
    Task t;
    t.id = "id_" + std::to_string(i);
    t.title = "Task Title " + std::to_string(i);
    t.status = TaskStatus::Doing;
    t.priority = TaskPriority::High;
    t.createdAt = std::chrono::system_clock::now();
    t.updatedAt = std::chrono::system_clock::now();
    tasks.push_back(std::move(t));
  }

  for (auto _ : state) {
    for (const auto& task : tasks)
      benchmark::DoNotOptimize(TaskMapper::toDto(task));
  }
}
BENCHMARK(BM_TaskMapper_ToDto_1000)->Unit(benchmark::kMillisecond);

// 🧪 Benchmark: TaskDto → Task (1000 iterations)
static void BM_TaskMapper_FromDto_1000(benchmark::State& state)
{
  std::vector<TaskDto> dtos;
  dtos.reserve(1000);
  for (int i = 0; i < 1000; ++i) {
    TaskDto d;
    d.id = "id_" + std::to_string(i);
    d.title = "Task Dto " + std::to_string(i);
    d.status = "InProgress";
    d.priority = "High";
    d.createdAt = "2025-10-15T12:00:00Z";
    d.updatedAt = "2025-10-15T12:00:00Z";
    dtos.push_back(std::move(d));
  }

  for (auto _ : state) {
    for (const auto& dto : dtos)
      benchmark::DoNotOptimize(TaskMapper::fromDto(dto)); // ✅ fully qualified call
  }
}
BENCHMARK(BM_TaskMapper_FromDto_1000)->Unit(benchmark::kMillisecond);

// 🧪 Benchmark: Time helpers ISO ↔ chrono (10000 iterations)
static void BM_TaskMapper_TimeHelpers_10000(benchmark::State& state)
{
  std::string iso = "2025-10-15T12:00:00Z";
  auto now = std::chrono::system_clock::now();

  for (auto _ : state) {
    for (int i = 0; i < 10000; ++i) {
      benchmark::DoNotOptimize(
          ::tasqly::p1::s1::domain::core::TaskMapperBenchAccess::timePointToIso(now));
      benchmark::DoNotOptimize(
          ::tasqly::p1::s1::domain::core::TaskMapperBenchAccess::isoToTimePoint(iso));
    }
  }
}
BENCHMARK(BM_TaskMapper_TimeHelpers_10000)->Unit(benchmark::kMillisecond);

} // namespace tasqly::p1::s1::benchmarks::domain::mapper

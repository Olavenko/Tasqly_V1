/*
 * 🧱 File: Bench_CreateTaskFlow.cpp
 * -----------------
 * 📌 Purpose   : Integration benchmark for "Create Task" use case (UUID + Log + Repo).
 * 🧱 Layer     : Performance (Integration Benchmark)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-16
 *
 * 🧠 Description:
 * Measures the end-to-end latency of creating a task:
 *  - Generate UUID
 *  - Log the creation event
 *  - Insert into InMemoryTaskRepository
 *
 * KPIs (Phase 0):
 *  - Log write < 2ms (inline latency).
 *  - CRUD ×10k < 500ms (repository throughput).
 */

#include "domain/core/Task.h"
#include "infra/logging/LogManager.h"
#include "infra/persistence/InMemoryTaskRepository.h"
#include "infra/utils/SystemUuidGen.h"
#include <benchmark/benchmark.h>

using namespace tasqly::domain::core;   // ✅ Task + InMemoryTaskRepository
using namespace tasqly::infra::logging; // ✅ LogManager

static void BM_CreateTaskFlow(benchmark::State& state)
{
  SystemUuidGen uuidGen;

  // 🆕 Pre-reserve 10k buckets to avoid QHash rehashing in stress benchmarks
  InMemoryTaskRepository repo(false, 10000);

  // Silent mode logger for clean benchmarks
  LogManager logManager(nullptr, true);

  for (auto _ : state) {
    Task t;
    t.id = uuidGen.generate();
    t.title = "Benchmark Task";
    t.description = "Created during benchmark";
    t.status = TaskStatus::Pending;

    // 📝 log creation (silent mode, no I/O)
    logManager.log(LogManager::Level::Info, "Benchmark", "Task created", {{"id", t.id}});

    // 📥 repo add (async API + get())
    auto result = repo.add(t).get();
    benchmark::DoNotOptimize(result);
  }

  // 📊 Report how many tasks created
  state.counters["Tasks"] = static_cast<double>(state.iterations());
}

BENCHMARK(BM_CreateTaskFlow)->Iterations(10000);

/*
 * 🧱 File: Bench_InMemoryTaskRepo_CRUD.cpp
 * ---------------------------------------
 * 📌 Purpose   : Benchmark coverage for InMemoryTaskRepository full CRUD operations
 * 🧱 Layer     : Persistence (InMemory) — Benchmarks
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-22
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Benchmarks to improve coverage of InMemoryTaskRepository by exercising
 * its complete CRUD API:
 *  - add (copy & move overloads)
 *  - update (copy & move overloads)
 *  - getById
 *  - remove
 *  - list (pagination)
 *  - addBulk
 *
 * 🔗 Depends On: domain/core/Task.h, infra/persistence/InMemoryTaskRepository.h
 * 🔗 Related   : Bench_CreateTaskFlow.cpp, Bench_InMemoryGoalRepo_CRUD.cpp
 * ✅ Tested In : TasqlyBenchmarksRunner (CI target)
 * ⚠️ Errors    : N/A (benchmark only, no error throwing)
 * ⚡ Performance: Benchmarks run with default Google Benchmark automatic timing
 * 🔒 Security  : N/A
 * 🔒 Thread Safety: N/A
 * 🧭 Benchmark Measurement Policy:
 *   - Automatic timing (CPUTime/RealTime as provided by Google Benchmark).
 *   - No manual timing used in this file.
 * 🕒 Last Major Change: 2025-09-22
 */

#include "domain/core/Task.h"
#include "infra/persistence/InMemoryTaskRepository.h"
#include <benchmark/benchmark.h>

using namespace tasqly::domain::core;

// 🧰 Helper: make dummy Task with unique id
static Task makeTask(const QString& idSuffix)
{
  const QDateTime now = QDateTime::currentDateTimeUtc();
  Task t;
  t.id = QString("task-%1").arg(idSuffix);
  t.title = QString("Task Title %1").arg(idSuffix);
  t.description = "Benchmark Task";
  t.status = TaskStatus::Pending;
  t.priority = TaskPriority::Low;
  t.createdAt = now;
  t.updatedAt = now;
  t.dueDate = std::nullopt;
  return t;
}

// 📝 Benchmark add (copy & move)
static void BM_TaskRepo_Add_CopyMove(benchmark::State& state)
{
  InMemoryTaskRepository repo;
  for (auto _ : state) {
    Task t = makeTask(QString::number(state.iterations()));
    auto res1 = repo.add(t).get(); // copy
    benchmark::DoNotOptimize(res1);

    auto res2 = repo.add(Task(makeTask("moved"))).get(); // move
    benchmark::DoNotOptimize(res2);
  }
}
BENCHMARK(BM_TaskRepo_Add_CopyMove);

// 📝 Benchmark update (copy & move)
static void BM_TaskRepo_Update_CopyMove(benchmark::State& state)
{
  InMemoryTaskRepository repo;
  auto base = makeTask("seed");
  repo.add(base).get();

  for (auto _ : state) {
    Task t1 = base;
    t1.title = "Updated Copy";
    auto res1 = repo.update(t1).get();
    benchmark::DoNotOptimize(res1);

    Task t2 = base;
    t2.title = "Updated Move";
    auto res2 = repo.update(Task(std::move(t2))).get();
    benchmark::DoNotOptimize(res2);
  }
}
BENCHMARK(BM_TaskRepo_Update_CopyMove);

// 📝 Benchmark getById
static void BM_TaskRepo_GetById(benchmark::State& state)
{
  InMemoryTaskRepository repo;
  auto t = makeTask("42");
  repo.add(t).get();

  for (auto _ : state) {
    auto res = repo.getById(t.id).get();
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_TaskRepo_GetById);

// 📝 Benchmark remove
static void BM_TaskRepo_Remove(benchmark::State& state)
{
  InMemoryTaskRepository repo;
  auto t = makeTask("99");
  repo.add(t).get();

  for (auto _ : state) {
    auto res = repo.remove(t.id).get();
    benchmark::DoNotOptimize(res);
    // re-add for next iteration
    repo.add(makeTask("99")).get();
  }
}
BENCHMARK(BM_TaskRepo_Remove);

// 📝 Benchmark list
static void BM_TaskRepo_List(benchmark::State& state)
{
  InMemoryTaskRepository repo;
  // seed 100 tasks
  for (int i = 0; i < 100; ++i) {
    repo.add(makeTask(QString::number(i))).get();
  }

  for (auto _ : state) {
    auto res = repo.list(10, 20).get(); // page of 10 tasks
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_TaskRepo_List);

// 📝 Benchmark addBulk
static void BM_TaskRepo_AddBulk(benchmark::State& state)
{
  for (auto _ : state) {
    InMemoryTaskRepository repo;
    std::vector<Task> tasks;
    tasks.reserve(100);
    for (int i = 0; i < 100; ++i) {
      tasks.push_back(makeTask(QString::number(i)));
    }
    repo.addBulk(std::move(tasks)); // void return
    benchmark::DoNotOptimize(repo);
  }
}
BENCHMARK(BM_TaskRepo_AddBulk);

// Entry point provided by benchmarks/bench_main.cpp

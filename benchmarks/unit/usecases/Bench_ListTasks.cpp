/*
 * 🧱 File: Bench_ListTasks.cpp
 * ----------------------------
 * 📌 Purpose   : Benchmark ListTasks use case (CPU-bound logic only).
 * 🧱 Layer     : Benchmarks (Use Cases)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-03
 *
 * 🧠 Description:
 * Benchmarks the performance of the ListTasks use case with varying dataset sizes.
 * Focus is strictly on CPU-bound execution (in-memory repository + fake logging).
 *
 * 🔗 Depends On:
 *   - app/usecases/ListTasks.h
 *   - domain/core/Task.h
 *   - domain/core/Error.h
 *   - Google Benchmark
 *
 * ✅ Tested In : TasqlyBenchmarksRunner (CI target)
 */

#include "app/usecases/ListTasks.h"
#include "domain/core/ITaskRepository.h"
#include "domain/core/Task.h"
#include <benchmark/benchmark.h>

using namespace tasqly::domain::core;
using namespace tasqly::app::usecases;

// 👉 Fake LogManager (in-memory, no I/O)
class FakeLogManager : public tasqly::app::logging::ILogManager
{
public:
  void setLevel(tasqly::app::logging::Level) override {}
  tasqly::app::logging::Level level() const override { return tasqly::app::logging::Level::Info; }
  bool isEnabled(tasqly::app::logging::Level) const override { return true; }

  void enableConsole(bool) override {}
  void enableFileSink(bool, const QString& = QString()) override {}
  void setRotationPolicy(const tasqly::app::logging::RotationPolicy&) override {}
  void rotateNow() override {}
  void flush() override {}

  void addMaskedKey(const QString&) override {}
  void setMaskedKeys(const QStringList&) override {}
  QStringList maskedKeys() const override { return {}; }

  void log(tasqly::app::logging::Level,
           const QString&,
           const QString&,
           const QVariantMap& = {}) override
  {
    // no-op
  }
};

// 👉 Fake TaskRepository (in-memory vector)
class FakeTaskRepository : public tasqly::domain::core::ITaskRepository
{
public:
  explicit FakeTaskRepository(std::vector<Task> tasks)
      : tasks_(std::move(tasks))
  {}

  // ✅ signature fixed: (limit, offset) to match real ListTasks use case
  std::future<Result<std::vector<Task>>> list(int limit, int offset) override
  {
    return std::async(std::launch::deferred, [=, this]() {
      if (offset >= static_cast<int>(tasks_.size())) {
        return ok<std::vector<Task>>({});
      }
      int end = std::min(offset + limit, static_cast<int>(tasks_.size()));
      std::vector<Task> slice(tasks_.begin() + offset, tasks_.begin() + end);
      return ok(slice);
    });
  }

  // 🆕 Added stubs for required pure virtuals
  std::future<Result<Task>> add(const Task& task) override
  {
    return std::async(std::launch::deferred, [=]() { return ok(task); });
  }

  std::future<Result<Task>> update(const Task& task) override
  {
    return std::async(std::launch::deferred, [=]() { return ok(task); });
  }

  std::future<Result<void>> remove(const QString&) override
  {
    return std::async(std::launch::deferred, []() { return ok(); });
  }

  std::future<Result<Task>> getById(const QString& id) override
  {
    (void) id; // silence unused warning
    return std::async(std::launch::deferred, [=]() {
      return err<Task>(Error{Error::kUnknownCode, "Fake repo: not implemented"});
    });
  }

private:
  std::vector<Task> tasks_;
};

// 👉 Fake AppContext (returns FakeRepo)
class FakeAppContext : public AppContext
{
public:
  explicit FakeAppContext(std::shared_ptr<FakeTaskRepository> repo)
      : repo_(std::move(repo))
  {}

  std::shared_ptr<ITaskRepository> taskRepository() const { return repo_; }

private:
  std::shared_ptr<FakeTaskRepository> repo_;
};

// 🧪 Benchmark helper
static void BM_ListTasks(benchmark::State& state)
{
  int numTasks = state.range(0);
  std::vector<Task> tasks;
  tasks.reserve(numTasks);

  // Fill with dummy tasks using full constructor
  for (int i = 0; i < numTasks; ++i) {
    tasks.emplace_back(QString::number(i),               // id
                       QString("Task %1").arg(i),        // title
                       QString("Description %1").arg(i), // description
                       TaskStatus::Pending,              // status
                       TaskPriority::Normal,             // priority
                       QDateTime::currentDateTime(),     // createdAt
                       QDateTime::currentDateTime(),     // updatedAt
                       std::nullopt                      // dueDate
    );
  }

  auto repo = std::make_shared<FakeTaskRepository>(tasks);
  FakeAppContext ctx(repo);
  FakeLogManager log;

  for (auto _ : state) {
    // ✅ Call ListTasks with (limit, offset) signature
    auto result = ListTasks(ctx, log, 50, 0);
    benchmark::DoNotOptimize(result);
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * numTasks);
}

// 📊 Register benchmarks (Small / Medium / Large)
BENCHMARK(BM_ListTasks)->Arg(10)->Arg(100)->Arg(1000);

/*
 * 🧱 File: test_fake_uuid_clock.cpp
 * -------------------------------
 * 📌 Purpose   : Unit tests for FakeUuidGen and FakeClock (deterministic fakes).
 * 🧱 Layer     : Tests (Unit)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-08
 *
 * 🧠 Description:
 * Validates deterministic behavior of FakeUuidGen (sequential IDs)
 * and FakeClock (manual time progression).
 */

#include "tests/fakes/FakeClock.h"
#include "tests/fakes/FakeUuidGen.h"
#include <gtest/gtest.h>

// 🔑 FakeUuidGen Tests
TEST(FakeUuidGenTests, GeneratesSequentialIds)
{
  FakeUuidGen gen;

  EXPECT_EQ(gen.generate(), "fake-0");
  EXPECT_EQ(gen.generate(), "fake-1");
  EXPECT_EQ(gen.generate(), "fake-2");
}

TEST(FakeUuidGenTests, GeneratesUniqueIdsInParallel)
{
  FakeUuidGen gen;

  // 🧵 Spawn multiple threads to check thread safety
  constexpr int N = 100;
  std::vector<std::string> results;
  results.reserve(N);

  std::mutex mtx;
  std::vector<std::thread> threads;

  for (int i = 0; i < N; ++i) {
    threads.emplace_back([&]() {
      auto id = gen.generate().toStdString();
      std::lock_guard<std::mutex> lock(mtx);
      results.push_back(id);
    });
  }

  for (auto& t : threads) {
    t.join();
  }

  // Ensure all IDs are unique
  std::sort(results.begin(), results.end());
  auto it = std::unique(results.begin(), results.end());
  EXPECT_EQ(it, results.end());
}

// ⏰ FakeClock Tests
TEST(FakeClockTests, ReturnsInitialTime)
{
  QDateTime fixed = QDateTime::fromSecsSinceEpoch(1000);
  FakeClock clock(fixed);

  EXPECT_EQ(clock.now(), fixed);
}

TEST(FakeClockTests, AdvancesTimeByMilliseconds)
{
  QDateTime fixed = QDateTime::fromSecsSinceEpoch(1000);
  FakeClock clock(fixed);

  clock.advance(5000); // +5 seconds
  EXPECT_EQ(clock.now(), fixed.addSecs(5));
}

TEST(FakeClockTests, MultipleAdvancesAccumulate)
{
  QDateTime fixed = QDateTime::fromSecsSinceEpoch(1000);
  FakeClock clock(fixed);

  clock.advance(1000); // +1 sec
  clock.advance(2000); // +2 sec
  EXPECT_EQ(clock.now(), fixed.addSecs(3));
}

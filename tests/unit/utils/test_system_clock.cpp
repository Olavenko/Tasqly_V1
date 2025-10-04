/*
 * 🧱 File: test_system_clock.cpp
 * ------------------------------
 * 📌 Purpose   : Unit tests for SystemClock implementation of IClock.
 * 🧱 Layer     : Tests (Unit - Utils)
 * 👤 Author    : Tasqly Team
 * 🗓️ Created   : 2025-09-15
 *
 * 🧠 Description:
 * Validates that SystemClock correctly returns the current system time
 * and compares behavior against FakeClock for determinism in tests.
 *
 * 🔗 Depends On:
 *   - src/infra/utils/SystemClock.h
 *   - src/domain/core/IClock.h
 *   - tests/fakes/FakeClock.h
 * 🔗 Related   : test_fake_uuid_clock.cpp
 * ✅ Tested In : TasqlyTestsRunner
 * ⚠️ Errors    : N/A (QDateTime always valid unless system clock misconfigured)
 * ⚡ Performance: Non-critical, single QDateTime fetch
 * 🔒 Security  : N/A
 * 🔒 Thread Safety: Safe (QDateTime is value type)
 * 🕒 Last Major Change: 2025-09-15
 */

#include "infra/utils/SystemClock.h"
#include "tests/fakes/FakeClock.h"

#include <gtest/gtest.h>
#include <qthread.h>

// 🧪 Test Suite: SystemClock
// --------------------------

TEST(SystemClockTest, NowReturnsValidDateTime)
{
  SystemClock clock;
  auto now = clock.now();

  // ✅ Expectation: returned datetime should be valid
  EXPECT_TRUE(now.isValid());
}

TEST(SystemClockTest, NowIsCloseToFakeClockStart)
{
  // Arrange: FakeClock with current time as baseline
  auto baseline = QDateTime::currentDateTime();
  FakeClock fake(baseline);

  SystemClock sysClock;

  // Act: get times
  auto realNow = sysClock.now();
  auto fakeNow = fake.now();

  // ✅ Expectation: SystemClock time should not differ by more than ~2s
  qint64 diffMs = realNow.msecsTo(fakeNow);
  EXPECT_LE(std::abs(diffMs), 2000);
}

TEST(SystemClockTest, FakeClockCanAdvanceWhileSystemClockMovesForward)
{
  FakeClock fake(QDateTime::currentDateTime());
  SystemClock sysClock;

  auto t1 = sysClock.now();
  auto f1 = fake.now();

  fake.advance(5000); // advance by 5s
  auto f2 = fake.now();

  // 🔄 Sleep a bit to ensure system clock moves forward
  QThread::msleep(5);
  auto t2 = sysClock.now();

  // ✅ FakeClock advanced by exactly 5s
  EXPECT_EQ(f1.msecsTo(f2), 5000);

  // ✅ SystemClock should not go backwards
  EXPECT_GE(t2.toMSecsSinceEpoch(), t1.toMSecsSinceEpoch());
}

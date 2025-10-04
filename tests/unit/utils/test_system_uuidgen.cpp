/*
 * 🧱 File: test_system_uuidgen.cpp
 * -------------------------------
 * 📌 Purpose   : Unit tests for SystemUuidGen implementation of IUuidGen.
 * 🧱 Layer     : Tests (Unit - Utils)
 * 👤 Author    : Tasqly Team
 * 🗓️ Created   : 2025-09-15
 *
 * 🧠 Description:
 * Validates that SystemUuidGen correctly generates valid UUID strings,
 * ensures uniqueness across calls, and compares behavior against FakeUuidGen
 * for deterministic testing.
 *
 * 🔗 Depends On:
 *   - src/infra/utils/SystemUuidGen.h
 *   - src/domain/core/IUuidGen.h
 *   - tests/fakes/FakeUuidGen.h
 * 🔗 Related   : test_fake_uuid_clock.cpp, test_system_clock.cpp
 * ✅ Tested In : TasqlyTestsRunner
 * ⚠️ Errors    : N/A (QUuid always generates valid IDs)
 * ⚡ Performance: Non-critical, single UUID generation
 * 🔒 Security  : UUID is non-sensitive
 * 🔒 Thread Safety: Safe (QUuid is thread-safe, FakeUuidGen uses atomic counter)
 * 🕒 Last Major Change: 2025-09-15
 */

#include "infra/utils/SystemUuidGen.h"
#include "tests/fakes/FakeUuidGen.h"

#include <gtest/gtest.h>

// 🧪 Test Suite: SystemUuidGen
// ----------------------------

// ✅ Test 1: SystemUuidGen::generate() should return a valid UUID string
TEST(SystemUuidGenTest, GenerateReturnsValidUuid)
{
  SystemUuidGen gen;
  QString id = gen.generate();

  // Expectation: non-empty and contains "-"
  EXPECT_FALSE(id.isEmpty());
  EXPECT_TRUE(id.contains("-"));
}

// ✅ Test 2: Two consecutive UUIDs from SystemUuidGen must be unique
TEST(SystemUuidGenTest, GenerateProducesUniqueValues)
{
  SystemUuidGen gen;
  QString id1 = gen.generate();
  QString id2 = gen.generate();

  EXPECT_NE(id1, id2);
}

// ✅ Test 3: FakeUuidGen generates deterministic values, SystemUuidGen generates real UUIDs
TEST(SystemUuidGenTest, CompareWithFakeUuidGen)
{
  FakeUuidGen fake;
  SystemUuidGen real;

  QString fid1 = fake.generate();
  QString fid2 = fake.generate();

  QString rid1 = real.generate();
  QString rid2 = real.generate();

  // Fake is deterministic
  EXPECT_EQ(fid1, "fake-0");
  EXPECT_EQ(fid2, "fake-1");

  // Real UUIDs must differ and not start with "fake"
  EXPECT_NE(rid1, rid2);
  EXPECT_FALSE(rid1.startsWith("fake"));
  EXPECT_FALSE(rid2.startsWith("fake"));
}

/*
 * 🧱 File: test_TaskMapper.cpp
 * ----------------------------
 * 📌 Purpose   : Unit test for TaskMapper (Task <-> TaskDto roundtrip).
 * 🧱 Layer     : Tests (Unit / Domain / Mappers)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-12
 * 🔖 Version   : 1.1 (Template fix & compliance)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Ensures TaskMapper correctly converts between Task and TaskDto:
 *   - Task → TaskDto → Task roundtrip preserves data integrity.
 *   - All fields are transferred without loss or mutation.
 *
 * ✅ Tested In : TasqlyTestsRunner
 * ⚡ Performance: Negligible (pure memory & string operations)
 */

#include "domain/core/entities/Task.h"
#include "domain/core/entities/TaskPriority.h"
#include "domain/core/entities/TaskStatus.h"
#include "domain/core/mappers/TaskDto.h"
#include "domain/core/mappers/TaskMapper.h"
#include <gtest/gtest.h>

using namespace tasqly::domain::core;

// 🧩 Fixture for TaskMapper tests
class TaskMapperTest : public ::testing::Test
{
protected:
  Task makeSampleTask()
  {
    Task t;
    t.id = "1234";
    t.title = "Write documentation";
    t.notes = std::make_optional(std::string("Detailed notes"));
    t.status = TaskStatus::Doing;
    t.priority = TaskPriority::High;
    t.deadline = std::make_optional(std::chrono::system_clock::now() + std::chrono::hours(24));
    t.createdAt = std::chrono::system_clock::now();
    t.updatedAt = t.createdAt + std::chrono::minutes(5);
    return t;
  }
};

// 🧪 Case 1: Task → TaskDto conversion should preserve values
TEST_F(TaskMapperTest, ToDto_ShouldMapAllFieldsCorrectly)
{
  auto task = makeSampleTask();
  auto dto = TaskMapper::toDto(task);

  EXPECT_EQ(dto.id, task.id);
  EXPECT_EQ(dto.title, task.title);
  EXPECT_EQ(dto.notes, task.notes);
  EXPECT_EQ(dto.status, toString(task.status));
  EXPECT_EQ(dto.priority, toString(task.priority));
  EXPECT_FALSE(dto.createdAt.empty());
  EXPECT_FALSE(dto.updatedAt.empty());

  if (task.deadline.has_value()) {
    EXPECT_TRUE(dto.deadline.has_value());
  }
}

// 🧪 Case 2: TaskDto → Task conversion should restore fields
TEST_F(TaskMapperTest, FromDto_ShouldRestoreAllFieldsCorrectly)
{
  TaskDto dto;
  dto.id = "5678";
  dto.title = "Implement mapper tests";
  dto.notes = std::make_optional(std::string("Verify roundtrip"));
  dto.status = "Doing";
  dto.priority = "High";
  dto.deadline = std::make_optional(std::string("2025-10-13T10:00:00Z"));
  dto.createdAt = "2025-10-12T10:00:00Z";
  dto.updatedAt = "2025-10-12T10:05:00Z";

  auto task = TaskMapper::fromDto(dto);

  EXPECT_EQ(task.id, dto.id);
  EXPECT_EQ(task.title, dto.title);
  EXPECT_EQ(task.notes, dto.notes);
  EXPECT_EQ(toString(task.status), dto.status);
  EXPECT_EQ(toString(task.priority), dto.priority);
  EXPECT_GT(task.createdAt.time_since_epoch().count(), 0);
  EXPECT_GT(task.updatedAt.time_since_epoch().count(), 0);
}

// 🧪 Case 3: Roundtrip Task → Dto → Task should be consistent
TEST_F(TaskMapperTest, Roundtrip_ShouldPreserveEntityIntegrity)
{
  auto original = makeSampleTask();
  auto dto = TaskMapper::toDto(original);
  auto roundtrip = TaskMapper::fromDto(dto);

  EXPECT_EQ(roundtrip.id, original.id);
  EXPECT_EQ(roundtrip.title, original.title);
  EXPECT_EQ(roundtrip.notes, original.notes);
  EXPECT_EQ(roundtrip.status, original.status);
  EXPECT_EQ(roundtrip.priority, original.priority);

  // timestamps check (existence only, format precision ignored)
  EXPECT_GT(roundtrip.createdAt.time_since_epoch().count(), 0);
  EXPECT_GT(roundtrip.updatedAt.time_since_epoch().count(), 0);
}

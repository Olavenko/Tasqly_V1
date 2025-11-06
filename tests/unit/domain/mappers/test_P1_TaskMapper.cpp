/*
 * 🧱 File: test_P1_TaskMapper.cpp
 * -------------------------------
 * 📌 Purpose   : Unit tests for P1_TaskMapper (Task <-> TaskDto conversions)
 * 🧱 Layer     : Domain (Core)
 * 🧪 Type      : Unit Test (GoogleTest)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-14
 * 🔖 Version   : 1.1 (Full Bidirectional Test)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 *   Verifies conversion logic between Task (domain entity)
 *   and TaskDto (data transfer object). Ensures symmetry and
 *   safety of time, string, and optional fields.
 */

#include "domain/core/entities/P1_Task.h"
#include "domain/core/mappers/P1_TaskMapper.h"
#include <chrono>
#include <gtest/gtest.h>

using namespace tasqly::p1::s1::domain::core;

// -----------------------------------------------------------------------------
// 🧩 Utility helpers
// -----------------------------------------------------------------------------
static bool nearlyEqualTime(const std::chrono::system_clock::time_point& a,
                            const std::chrono::system_clock::time_point& b,
                            std::chrono::seconds tolerance = std::chrono::seconds(2))
{
  auto diff = std::chrono::duration_cast<std::chrono::seconds>(a - b);
  return std::abs(diff.count()) <= tolerance.count();
}

// -----------------------------------------------------------------------------
// 🧪 Test 1: Basic conversion (Task → TaskDto)
// -----------------------------------------------------------------------------
TEST(TaskMapperTest, ToDto_ConvertsAllFields)
{
  Task task;
  task.id = "task-001";
  task.title = "Complete Phase1 Refactor";
  task.notes = std::make_optional<std::string>("Refactor Domain Layer to STL");
  task.status = TaskStatus::Doing;
  task.priority = TaskPriority::High;
  task.deadline = std::chrono::system_clock::now() + std::chrono::hours(24);
  task.createdAt = std::chrono::system_clock::now();
  task.updatedAt = task.createdAt + std::chrono::minutes(15);

  TaskDto dto = TaskMapper::toDto(task);

  EXPECT_EQ(dto.id, "task-001");
  EXPECT_EQ(dto.title, "Complete Phase1 Refactor");
  EXPECT_TRUE(dto.notes.has_value());
  EXPECT_EQ(dto.notes.value(), "Refactor Domain Layer to STL");
  EXPECT_EQ(dto.status, "Doing");
  EXPECT_EQ(dto.priority, "High");
  EXPECT_TRUE(dto.deadline.has_value());
  EXPECT_FALSE(dto.createdAt.empty());
  EXPECT_FALSE(dto.updatedAt.empty());
}

// -----------------------------------------------------------------------------
// 🧪 Test 2: Round-trip (Task → DTO → Task)
// -----------------------------------------------------------------------------
TEST(TaskMapperTest, RoundTrip_PreservesFields)
{
  Task original;
  original.id = "task-XYZ";
  original.title = "Refactor Mapper RoundTrip";
  original.notes = std::make_optional<std::string>("Ensure toDto and fromDto symmetry");
  original.status = TaskStatus::Done;
  original.priority = TaskPriority::Normal;
  original.deadline = std::chrono::system_clock::now() + std::chrono::hours(2);
  original.createdAt = std::chrono::system_clock::now();
  original.updatedAt = original.createdAt + std::chrono::minutes(1);

  TaskDto dto = TaskMapper::toDto(original);
  Task reconstructed = TaskMapper::fromDto(dto);

  EXPECT_EQ(reconstructed.id, original.id);
  EXPECT_EQ(reconstructed.title, original.title);
  EXPECT_EQ(reconstructed.status, original.status);
  EXPECT_EQ(reconstructed.priority, original.priority);

  ASSERT_TRUE(reconstructed.notes.has_value());
  EXPECT_EQ(reconstructed.notes.value(), original.notes.value());

  ASSERT_TRUE(reconstructed.deadline.has_value());
  EXPECT_TRUE(nearlyEqualTime(reconstructed.deadline.value(), original.deadline.value()));

  EXPECT_TRUE(nearlyEqualTime(reconstructed.createdAt, original.createdAt));
  EXPECT_TRUE(nearlyEqualTime(reconstructed.updatedAt, original.updatedAt));
}

// -----------------------------------------------------------------------------
// 🧪 Test 3: Handles Empty Optionals
// -----------------------------------------------------------------------------
TEST(TaskMapperTest, HandlesEmptyOptionals)
{
  Task task;
  task.id = "opt-001";
  task.title = "Optional Safety Test";
  task.notes.reset();
  task.deadline.reset();

  TaskDto dto = TaskMapper::toDto(task);

  EXPECT_FALSE(dto.notes.has_value());
  EXPECT_FALSE(dto.deadline.has_value());

  Task result = TaskMapper::fromDto(dto);
  EXPECT_FALSE(result.notes.has_value());
  EXPECT_FALSE(result.deadline.has_value());
}

// -----------------------------------------------------------------------------
// 🧪 Test 4: Parses ISO8601 Back to TimePoint
// -----------------------------------------------------------------------------
TEST(TaskMapperTest, FromDto_ParsesISO8601Successfully)
{
  TaskDto dto;
  dto.id = "iso-001";
  dto.title = "Parse ISO8601";
  dto.status = "Todo";
  dto.priority = "Low";
  dto.createdAt = "2025-10-14T12:00:00Z";
  dto.updatedAt = "2025-10-14T13:00:00Z";
  dto.deadline = std::make_optional<std::string>("2025-10-15T00:00:00Z");

  Task t = TaskMapper::fromDto(dto);

  EXPECT_EQ(t.id, "iso-001");
  EXPECT_EQ(t.title, "Parse ISO8601");
  EXPECT_EQ(t.status, TaskStatus::Todo);
  EXPECT_EQ(t.priority, TaskPriority::Low);
  EXPECT_TRUE(t.deadline.has_value());
}

// -----------------------------------------------------------------------------
// 🧪 Test 5: Handles Invalid ISO8601 Gracefully
// -----------------------------------------------------------------------------
TEST(TaskMapperTest, FromDto_InvalidISOHandled)
{
  TaskDto dto;
  dto.id = "iso-invalid";
  dto.title = "Bad Date Test";
  dto.status = "Doing";
  dto.priority = "Normal";
  dto.createdAt = "InvalidDate";
  dto.updatedAt = "AlsoBad";

  // No crash should happen
  EXPECT_NO_THROW({
    Task t = TaskMapper::fromDto(dto);
    EXPECT_EQ(t.id, "iso-invalid");
    EXPECT_EQ(t.status, TaskStatus::Doing);
    EXPECT_EQ(t.priority, TaskPriority::Normal);
  });
}

// -----------------------------------------------------------------------------
// 🧪 Test 6: Handles Short ISO8601 String
// -----------------------------------------------------------------------------
TEST(TaskMapperTest, FromDto_ShortISOString)
{
  TaskDto dto;
  dto.id = "iso-short";
  dto.title = "Short ISO Test";
  dto.status = "Todo";
  dto.priority = "Low";
  dto.createdAt = "2025-10-14"; // Too short (< 20 chars)
  dto.updatedAt = "2025-10-14"; // Too short

  // Should handle gracefully and return epoch time
  EXPECT_NO_THROW({
    Task t = TaskMapper::fromDto(dto);
    EXPECT_EQ(t.id, "iso-short");
    EXPECT_EQ(t.status, TaskStatus::Todo);
    EXPECT_EQ(t.priority, TaskPriority::Low);
  });
}

// -----------------------------------------------------------------------------
// 🧪 Test 7: Handles Empty Optional Notes
// -----------------------------------------------------------------------------
TEST(TaskMapperTest, ToDto_EmptyOptionalNotes)
{
  Task task;
  task.id = "empty-notes";
  task.title = "Empty Notes Test";
  task.notes = std::make_optional<std::string>(""); // Empty string
  task.status = TaskStatus::Todo;
  task.priority = TaskPriority::Normal;
  task.createdAt = std::chrono::system_clock::now();
  task.updatedAt = task.createdAt;

  TaskDto dto = TaskMapper::toDto(task);
  
  // Empty string should reset optional
  EXPECT_FALSE(dto.notes.has_value());
}

// -----------------------------------------------------------------------------
// 🧪 Test 8: Handles All Status Values
// -----------------------------------------------------------------------------
TEST(TaskMapperTest, FromDto_AllStatusValues)
{
  std::vector<std::string> statusStrings = {"Todo", "Doing", "Done"};
  std::vector<TaskStatus> expectedStatuses = {TaskStatus::Todo, TaskStatus::Doing, TaskStatus::Done};
  
  for (size_t i = 0; i < statusStrings.size(); ++i) {
    TaskDto dto;
    dto.id = "status-test-" + std::to_string(i);
    dto.title = "Status Test";
    dto.status = statusStrings[i];
    dto.priority = "Normal";
    dto.createdAt = "2025-10-14T12:00:00Z";
    dto.updatedAt = "2025-10-14T12:00:00Z";
    
    Task t = TaskMapper::fromDto(dto);
    EXPECT_EQ(t.status, expectedStatuses[i]) << "Status should match for " << statusStrings[i];
  }
}

// -----------------------------------------------------------------------------
// 🧪 Test 9: Handles All Priority Values
// -----------------------------------------------------------------------------
TEST(TaskMapperTest, FromDto_AllPriorityValues)
{
  std::vector<std::string> priorityStrings = {"Low", "Normal", "High"};
  std::vector<TaskPriority> expectedPriorities = {TaskPriority::Low, TaskPriority::Normal, TaskPriority::High};
  
  for (size_t i = 0; i < priorityStrings.size(); ++i) {
    TaskDto dto;
    dto.id = "priority-test-" + std::to_string(i);
    dto.title = "Priority Test";
    dto.status = "Todo";
    dto.priority = priorityStrings[i];
    dto.createdAt = "2025-10-14T12:00:00Z";
    dto.updatedAt = "2025-10-14T12:00:00Z";
    
    Task t = TaskMapper::fromDto(dto);
    EXPECT_EQ(t.priority, expectedPriorities[i]) << "Priority should match for " << priorityStrings[i];
  }
}

// -----------------------------------------------------------------------------
// 🧪 Test 10: Handles Invalid Status/Priority (Defaults)
// -----------------------------------------------------------------------------
TEST(TaskMapperTest, FromDto_InvalidStatusPriorityDefaults)
{
  TaskDto dto;
  dto.id = "invalid-enum";
  dto.title = "Invalid Enum Test";
  dto.status = "InvalidStatus"; // Not in table
  dto.priority = "InvalidPriority"; // Not in table
  dto.createdAt = "2025-10-14T12:00:00Z";
  dto.updatedAt = "2025-10-14T12:00:00Z";
  
  Task t = TaskMapper::fromDto(dto);
  // Should default to Todo and Normal
  EXPECT_EQ(t.status, TaskStatus::Todo);
  EXPECT_EQ(t.priority, TaskPriority::Normal);
}
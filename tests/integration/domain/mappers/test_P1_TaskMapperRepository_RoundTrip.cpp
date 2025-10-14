/*
 * 🧱 File: test_P1_TaskMapperRepository_RoundTrip.cpp
 * --------------------------------------------------
 * 📌 Purpose   : Full integration test for TaskMapper + FakeTaskRepository
 * 🧱 Layer     : Domain (Integration)
 * 🧪 Type      : Round-trip Integration Test (GoogleTest)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-14
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 *   Validates full cycle:
 *     Domain Task → DTO → Repository (Create) → Repository (Read)
 *     → DTO again → Compare all values for consistency.
 *
 *   Ensures:
 *   - Mapper correctness inside repository context
 *   - Optional and time fields preserved accurately
 *   - DomainResult safety across all layers
 */

#include "domain/core/entities/P1_Task.h"
#include "domain/core/mappers/P1_TaskMapper.h"
#include "tests/fakes/domain/FakeTaskRepository.h"
#include <chrono>
#include <gtest/gtest.h>
#include <iostream>

using namespace tasqly::domain::core::v1; // repository

// ----------------------------------------------------------------------------
// 🧩 Utility — Compare two TaskDto objects logically
// ----------------------------------------------------------------------------
static void expectDtoEqual(const TaskDto& a, const TaskDto& b)
{
  EXPECT_EQ(a.id, b.id);
  EXPECT_EQ(a.title, b.title);
  EXPECT_EQ(a.status, b.status);
  EXPECT_EQ(a.priority, b.priority);

  if (a.notes.has_value() || b.notes.has_value()) {
    EXPECT_EQ(a.notes.value_or(""), b.notes.value_or(""));
  }

  if (a.deadline.has_value() || b.deadline.has_value()) {
    EXPECT_EQ(a.deadline.value_or(""), b.deadline.value_or(""));
  }

  EXPECT_EQ(a.createdAt.substr(0, 16), b.createdAt.substr(0, 16)); // ignore seconds
  EXPECT_EQ(a.updatedAt.substr(0, 16), b.updatedAt.substr(0, 16));
}

// ----------------------------------------------------------------------------
// 🧪 Test: Round-trip between TaskMapper + Repository
// ----------------------------------------------------------------------------
TEST(TaskMapperRepositoryIntegration, FullRoundTrip)
{
  FakeTaskRepository repo;
  repo.clear();

  // 🧱 Step 1: Create Domain Entity
  Task domainTask;
  domainTask.id = "roundtrip-001";
  domainTask.title = "RoundTrip Integration Test";
  domainTask.notes = "Ensure mapper and repository coherence";
  domainTask.status = TaskStatus::Doing;
  domainTask.priority = TaskPriority::High;
  domainTask.deadline = std::chrono::system_clock::now() + std::chrono::hours(3);
  domainTask.createdAt = std::chrono::system_clock::now();
  domainTask.updatedAt = domainTask.createdAt;

  // 🧱 Step 2: Convert Domain → DTO
  TaskDto originalDto = TaskMapper::toDto(domainTask);

  // 🧱 Step 3: Convert DTO → Domain and store in repository
  Task repoTask = TaskMapper::fromDto(originalDto);
  auto createResult = repo.create(repoTask);
  ASSERT_TRUE(createResult.isOk());

  // 🧱 Step 4: Retrieve from repository
  auto fetchResult = repo.getById("roundtrip-001");
  ASSERT_TRUE(fetchResult.isOk());
  Task fetchedTask = fetchResult.value();

  // 🧱 Step 5: Convert back to DTO
  TaskDto fetchedDto = TaskMapper::toDto(fetchedTask);

  // 🧱 Step 6: Compare input/output DTOs
  expectDtoEqual(originalDto, fetchedDto);

  std::cout << "[RoundTrip ✅] Title: " << fetchedDto.title << "\n";
  std::cout << "Status: " << fetchedDto.status << ", Priority: " << fetchedDto.priority << "\n";
  std::cout << "CreatedAt: " << fetchedDto.createdAt << "\n";
  std::cout << "UpdatedAt: " << fetchedDto.updatedAt << "\n";
}

// ----------------------------------------------------------------------------
// 🧪 Test: Round-trip with Empty Optionals
// ----------------------------------------------------------------------------
TEST(TaskMapperRepositoryIntegration, RoundTripWithEmptyOptionals)
{
  FakeTaskRepository repo;
  repo.clear();

  Task t;
  t.id = "roundtrip-empty";
  t.title = "Empty Optional Fields";
  t.status = TaskStatus::Todo;
  t.priority = TaskPriority::Normal;
  t.notes.reset();
  t.deadline.reset();

  TaskDto dtoIn = TaskMapper::toDto(t);
  Task stored = TaskMapper::fromDto(dtoIn);

  ASSERT_TRUE(repo.create(stored).isOk());

  auto loaded = repo.getById("roundtrip-empty");
  ASSERT_TRUE(loaded.isOk());

  TaskDto dtoOut = TaskMapper::toDto(loaded.value());

  EXPECT_FALSE(dtoOut.notes.has_value());
  EXPECT_FALSE(dtoOut.deadline.has_value());
  EXPECT_EQ(dtoOut.title, "Empty Optional Fields");

  std::cout << "[RoundTrip ✅] Empty optionals handled correctly.\n";
}

// ----------------------------------------------------------------------------
// 🧪 Test: Repository returns DomainResult errors gracefully
// ----------------------------------------------------------------------------
TEST(TaskMapperRepositoryIntegration, HandlesRepositoryErrors)
{
  FakeTaskRepository repo;
  repo.clear();

  Task t;
  t.id = "dup-roundtrip";
  t.title = "First Insert";
  ASSERT_TRUE(repo.create(t).isOk());

  // Insert same ID again
  auto dup = repo.create(t);
  ASSERT_TRUE(dup.isErr());
  EXPECT_EQ(dup.error().code, DomainErrorCode::Conflict);

  // Try to fetch missing ID
  auto missing = repo.getById("not-exist-123");
  ASSERT_TRUE(missing.isErr());
  EXPECT_EQ(missing.error().code, DomainErrorCode::NotFound);
}

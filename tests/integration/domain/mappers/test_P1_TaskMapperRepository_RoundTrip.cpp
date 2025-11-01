/*
 * 🧱 File: test_P1_TaskMapperRepository_RoundTrip.cpp
 * --------------------------------------------------
 * 📌 Purpose   : Integration tests for TaskMapper + FakeTaskRepository (Phase 1)
 * 🧱 Layer     : Domain (Integration)
 * 🧪 Type      : Round-trip & Error Integration Tests
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-21
 * 🔖 Version   : 2.0 (Rewritten — Namespace Safe & Memory-Safe)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 *   Verifies the integrity of task round-trip transformations:
 *     Domain Task → DTO → Repository (Create) → Repository (Read)
 *     → Domain → DTO → Compare equality.
 *
 *   Ensures:
 *   - Mapper consistency across all conversions
 *   - Repository value semantics (no dangling references)
 *   - Proper DomainResult error behavior
 */

#include <chrono>
#include <gtest/gtest.h>
#include <iostream>

#include "domain/core/entities/P1_Task.h"
#include "domain/core/errors/P1_DomainError.h"
#include "domain/core/errors/P1_DomainResult.h"
#include "domain/core/mappers/P1_TaskMapper.h"
#include "tests/fakes/domain/FakeTaskRepository.h"

using namespace tasqly::p1::s1::domain::core;

// ---------------------------------------------------------------------------
// 🧩 Utility — Compare two TaskDto objects logically
// ---------------------------------------------------------------------------
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

  EXPECT_EQ(a.createdAt.substr(0, 16), b.createdAt.substr(0, 16));
  EXPECT_EQ(a.updatedAt.substr(0, 16), b.updatedAt.substr(0, 16));
}

// ---------------------------------------------------------------------------
// 🧪 Test Case 1 — Full round-trip through mapper + repository
// ---------------------------------------------------------------------------
TEST(TaskMapperRepositoryIntegration, FullRoundTrip)
{
  FakeTaskRepository repo;
  repo.clear();

  // Step 1: Create domain Task
  Task domainTask;
  domainTask.id = "task-rt-001";
  domainTask.title = "Mapper/Repo Integration Test";
  domainTask.notes = "Round-trip full consistency validation";
  domainTask.status = TaskStatus::Doing;
  domainTask.priority = TaskPriority::High;
  domainTask.deadline = std::chrono::system_clock::now() + std::chrono::hours(6);
  domainTask.createdAt = std::chrono::system_clock::now();
  domainTask.updatedAt = domainTask.createdAt;

  // Step 2: Domain → DTO
  TaskDto dtoOriginal = TaskMapper::toDto(domainTask);

  // Step 3: DTO → Domain → store
  Task repoTask = TaskMapper::fromDto(dtoOriginal);
  auto createRes = repo.create(repoTask);
  ASSERT_TRUE(createRes.isOk()) << "Repository create() failed unexpectedly";

  // Step 4: Retrieve from repository
  auto fetchRes = repo.getById(domainTask.id);
  ASSERT_TRUE(fetchRes.isOk()) << "Repository getById() failed";
  Task fetchedTask = fetchRes.value();

  // Step 5: Convert back to DTO
  TaskDto dtoFetched = TaskMapper::toDto(fetchedTask);

  // Step 6: Validate
  expectDtoEqual(dtoOriginal, dtoFetched);

  std::cout << "[✅ RoundTrip] Title: " << dtoFetched.title << "\n";
  std::cout << "Status: " << dtoFetched.status << ", Priority: " << dtoFetched.priority << "\n";
  std::cout << "CreatedAt: " << dtoFetched.createdAt << "\n";
  std::cout << "UpdatedAt: " << dtoFetched.updatedAt << "\n";
}

// ---------------------------------------------------------------------------
// 🧪 Test Case 2 — Round-trip with empty optionals
// ---------------------------------------------------------------------------
TEST(TaskMapperRepositoryIntegration, RoundTripWithEmptyOptionals)
{
  FakeTaskRepository repo;
  repo.clear();

  Task t;
  t.id = "task-rt-empty";
  t.title = "Empty Optionals Test";
  t.status = TaskStatus::Todo;
  t.priority = TaskPriority::Normal;
  t.notes.reset();
  t.deadline.reset();

  TaskDto dtoIn = TaskMapper::toDto(t);
  Task stored = TaskMapper::fromDto(dtoIn);
  ASSERT_TRUE(repo.create(stored).isOk());

  auto res = repo.getById("task-rt-empty");
  ASSERT_TRUE(res.isOk());
  Task loaded = res.value();

  TaskDto dtoOut = TaskMapper::toDto(loaded);

  EXPECT_FALSE(dtoOut.notes.has_value());
  EXPECT_FALSE(dtoOut.deadline.has_value());
  EXPECT_EQ(dtoOut.title, "Empty Optionals Test");

  std::cout << "[✅ RoundTrip] Empty optionals handled safely.\n";
}

// ---------------------------------------------------------------------------
// 🧪 Test Case 3 — Repository error propagation
// ---------------------------------------------------------------------------
TEST(TaskMapperRepositoryIntegration, HandlesRepositoryErrors)
{
  FakeTaskRepository repo;
  repo.clear();

  Task t;
  t.id = "dup-id-test";
  t.title = "First Insert";

  ASSERT_TRUE(repo.create(t).isOk()) << "Initial insert failed";

  // Insert duplicate ID → Conflict
  auto dupRes = repo.create(t);
  ASSERT_TRUE(dupRes.isErr());
  EXPECT_EQ(dupRes.error().code, DomainErrorCode::Conflict);

  // Missing ID → NotFound
  auto missRes = repo.getById("nonexistent-id-999");
  ASSERT_TRUE(missRes.isErr());
  EXPECT_EQ(missRes.error().code, DomainErrorCode::NotFound);

  std::cout << "[✅ RepositoryErrors] Conflict & NotFound handled correctly.\n";
}

/*
 * 🧱 File: test_TaskMapper_Single.cpp
 * ----------------------------
 * 📌 Purpose   : Minimal smoke test for TaskMapper::toDto()
 * 🧱 Layer     : Domain (Core) — Tests
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-14
 * 🔖 Version   : 1.1 (Namespace Isolation Fix)
 * 🛡️ Stability : Safe for debug builds
 *
 * 🧠 Description:
 * This minimal test isolates TaskMapper::toDto() to validate that
 * optional fields and time conversions behave safely without crashes.
 * It helps detect invalid std::optional access before running full suites.
 */

#include "domain/core/entities/P1_Task.h"
#include "domain/core/mappers/P1_TaskMapper.h"
#include <gtest/gtest.h>
#include <iostream>

using namespace tasqly::domain::core::v1; // ✅ FIXED: isolate Phase1 namespace

// ============================================================================
// 🧪 TEST CASE — Minimal ToDto conversion smoke test
// ============================================================================
TEST(TaskMapperDiagnostic, ToDtoBasicConversion)
{
  Task task;
  task.id = "diag-001";
  task.title = "Diagnostic Task";
  task.status = TaskStatus::Todo;
  task.priority = TaskPriority::Normal;
  task.notes.reset();    // ensure optional is empty
  task.deadline.reset(); // ensure optional is empty

  std::cout << "[INFO] Starting diagnostic test...\n";

  EXPECT_NO_THROW({
    TaskDto dto = TaskMapper::toDto(task);

    std::cout << "[INFO] Conversion succeeded.\n";
    std::cout << "  id: " << dto.id << "\n";
    std::cout << "  title: " << dto.title << "\n";
    std::cout << "  status: " << dto.status << "\n";
    std::cout << "  priority: " << dto.priority << "\n";

    // sanity check: optionals should be empty
    EXPECT_FALSE(dto.notes.has_value());
    EXPECT_FALSE(dto.deadline.has_value());
    EXPECT_FALSE(dto.createdAt.empty());
    EXPECT_FALSE(dto.updatedAt.empty());
  });
}

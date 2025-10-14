/*
 * 🧱 File: test_TaskPriority.cpp
 * ------------------------------
 * 📌 Purpose   : Unit tests for TaskPriority conversion utilities.
 * 🧱 Layer     : Domain (Core)
 * 🧪 Type      : Unit Test (GoogleTest)
 * 👤 Author    : Tasqly QA Bot
 * 🗓️ Created   : 2025-10-12
 * 🔖 Version   : 1.0
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Tests the correctness of TaskPriority enum conversion functions:
 *  - toString(TaskPriority)
 *  - taskPriorityFromString(std::string)
 *
 * Ensures round-trip consistency and case-sensitivity behavior.
 */

#include "domain/core/entities/TaskPriority.h"
#include <gtest/gtest.h>

using namespace tasqly::domain::core;

// 🧩 Test: toString() should return correct string for each enum value
TEST(TaskPriorityTest, ToStringReturnsCorrectValue)
{
  EXPECT_EQ(toString(TaskPriority::Low), "Low");
  EXPECT_EQ(toString(TaskPriority::Normal), "Normal");
  EXPECT_EQ(toString(TaskPriority::High), "High");
}

// 🧩 Test: taskPriorityFromString() should parse valid values correctly
TEST(TaskPriorityTest, FromStringParsesValidValues)
{
  auto low = taskPriorityFromString("Low");
  auto normal = taskPriorityFromString("Normal");
  auto high = taskPriorityFromString("High");

  ASSERT_TRUE(low.has_value());
  ASSERT_TRUE(normal.has_value());
  ASSERT_TRUE(high.has_value());

  EXPECT_EQ(low.value(), TaskPriority::Low);
  EXPECT_EQ(normal.value(), TaskPriority::Normal);
  EXPECT_EQ(high.value(), TaskPriority::High);
}

// 🧩 Test: taskPriorityFromString() should support lowercase inputs
TEST(TaskPriorityTest, FromStringSupportsLowercase)
{
  EXPECT_EQ(taskPriorityFromString("low").value(), TaskPriority::Low);
  EXPECT_EQ(taskPriorityFromString("normal").value(), TaskPriority::Normal);
  EXPECT_EQ(taskPriorityFromString("high").value(), TaskPriority::High);
}

// 🧩 Test: taskPriorityFromString() should return nullopt for invalid strings
TEST(TaskPriorityTest, FromStringHandlesInvalidInput)
{
  EXPECT_FALSE(taskPriorityFromString("invalid").has_value());
  EXPECT_FALSE(taskPriorityFromString("").has_value());
  EXPECT_FALSE(taskPriorityFromString("LOWHIGH").has_value());
}

// 🧩 Test: taskPriorityFromString() should be case-sensitive beyond defined cases
TEST(TaskPriorityTest, FromStringIsCaseSensitive)
{
  EXPECT_FALSE(taskPriorityFromString("HIGH").has_value());
  EXPECT_FALSE(taskPriorityFromString("NORMAL").has_value());
  EXPECT_FALSE(taskPriorityFromString("LOW").has_value());
}

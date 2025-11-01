/*
 * 🧱 File: test_TaskStatus.cpp
 * ----------------------------
 * 📌 Purpose   : Unit tests for TaskStatus conversion utilities.
 * 🧱 Layer     : Domain (Core)
 * 🧪 Type      : Unit Test (GoogleTest)
 * 👤 Author    : Tasqly QA Bot
 * 🗓️ Created   : 2025-10-12
 * 🔖 Version   : 1.0
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Tests conversion logic between TaskStatus enum and its string representation.
 * Verifies correctness, case handling, and invalid input behavior.
 */

#include "domain/core/entities/P1_TaskStatus.h"
#include <gtest/gtest.h>

using namespace tasqly::p1::s1::domain::core;

// 🧩 Test: toString() returns correct values
TEST(TaskStatusTest, ToStringReturnsExpectedValues)
{
  EXPECT_EQ(toString(TaskStatus::Todo), "Todo");
  EXPECT_EQ(toString(TaskStatus::Doing), "Doing");
  EXPECT_EQ(toString(TaskStatus::Done), "Done");
}

// 🧩 Test: taskStatusFromString() parses valid strings
TEST(TaskStatusTest, FromStringParsesValidValues)
{
  auto todo = taskStatusFromString("Todo");
  auto doing = taskStatusFromString("Doing");
  auto done = taskStatusFromString("Done");

  ASSERT_TRUE(todo.has_value());
  ASSERT_TRUE(doing.has_value());
  ASSERT_TRUE(done.has_value());

  EXPECT_EQ(todo.value(), TaskStatus::Todo);
  EXPECT_EQ(doing.value(), TaskStatus::Doing);
  EXPECT_EQ(done.value(), TaskStatus::Done);
}

// 🧩 Test: lowercase variants are accepted
TEST(TaskStatusTest, FromStringAcceptsLowercaseVariants)
{
  EXPECT_EQ(taskStatusFromString("todo").value(), TaskStatus::Todo);
  EXPECT_EQ(taskStatusFromString("doing").value(), TaskStatus::Doing);
  EXPECT_EQ(taskStatusFromString("done").value(), TaskStatus::Done);
}

// 🧩 Test: invalid inputs return nullopt
TEST(TaskStatusTest, FromStringHandlesInvalidInputs)
{
  EXPECT_FALSE(taskStatusFromString("unknown").has_value());
  EXPECT_FALSE(taskStatusFromString("").has_value());
  EXPECT_FALSE(taskStatusFromString("ToDoDone").has_value());
}

// 🧩 Test: uppercase inputs are case-sensitive
TEST(TaskStatusTest, FromStringIsCaseSensitive)
{
  EXPECT_FALSE(taskStatusFromString("TODO").has_value());
  EXPECT_FALSE(taskStatusFromString("DOING").has_value());
  EXPECT_FALSE(taskStatusFromString("DONE").has_value());
}

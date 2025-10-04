/*
 * 🧱 File: test_goal.cpp
 * ----------------------
 * 📌 Purpose   : Unit tests for domain entity Goal
 * 🧱 Layer     : Tests (Unit - Domain Core)
 * 👤 Author    : Tasqly Team
 * 🗓️ Created   : 2025-09-23
 *
 * 🧠 Description:
 * Verifies constructors and assignment operators of Goal entity:
 * - Default constructor
 * - Parameterized constructor
 * - Copy/Move constructors
 * - Copy/Move assignments
 */

#include "domain/core/Goal.h"

#include <gtest/gtest.h>

using namespace tasqly::domain::core;

// 📝 Test 1: Default constructor initializes empty fields
TEST(GoalEntityTest, DefaultConstructorInitializesEmpty)
{
  Goal g;

  EXPECT_TRUE(g.id.isEmpty());
  EXPECT_TRUE(g.title.isEmpty());
  EXPECT_TRUE(g.description.isEmpty());
  EXPECT_FALSE(g.dueDate.has_value());
}

// 📝 Test 2: Parameterized constructor sets fields correctly
TEST(GoalEntityTest, ParameterizedConstructorSetsFields)
{
  QDateTime now = QDateTime::currentDateTimeUtc();
  Goal g("id1", "My Goal", "Description", now, now, now.addDays(7));

  EXPECT_EQ(g.id, "id1");
  EXPECT_EQ(g.title, "My Goal");
  EXPECT_EQ(g.description, "Description");
  EXPECT_EQ(g.createdAt, now);
  EXPECT_EQ(g.updatedAt, now);
  ASSERT_TRUE(g.dueDate.has_value());
  EXPECT_EQ(g.dueDate.value(), now.addDays(7));
}

// 📝 Test 3: Copy constructor clones fields
TEST(GoalEntityTest, CopyConstructorClonesFields)
{
  QDateTime now = QDateTime::currentDateTimeUtc();
  Goal g1("id2", "Copy Goal", "Desc", now, now);
  Goal g2(g1);

  EXPECT_EQ(g2.id, g1.id);
  EXPECT_EQ(g2.title, g1.title);
  EXPECT_EQ(g2.description, g1.description);
  EXPECT_EQ(g2.createdAt, g1.createdAt);
  EXPECT_EQ(g2.updatedAt, g1.updatedAt);
  EXPECT_EQ(g2.dueDate, g1.dueDate);
}

// 📝 Test 4: Move constructor transfers fields
TEST(GoalEntityTest, MoveConstructorTransfersFields)
{
  QDateTime now = QDateTime::currentDateTimeUtc();
  Goal g1("id3", "Move Goal", "Desc", now, now, now.addDays(1));
  Goal g2(std::move(g1));

  EXPECT_EQ(g2.id, "id3");
  EXPECT_EQ(g2.title, "Move Goal");
  EXPECT_EQ(g2.description, "Desc");
  EXPECT_EQ(g2.createdAt, now);
  EXPECT_EQ(g2.updatedAt, now);
  ASSERT_TRUE(g2.dueDate.has_value());
  EXPECT_EQ(g2.dueDate.value(), now.addDays(1));
}

// 📝 Test 5: Copy assignment works
TEST(GoalEntityTest, CopyAssignmentWorks)
{
  QDateTime now = QDateTime::currentDateTimeUtc();
  Goal g1("id4", "Assigned Goal", "Desc", now, now);
  Goal g2;
  g2 = g1;

  EXPECT_EQ(g2.id, g1.id);
  EXPECT_EQ(g2.title, g1.title);
  EXPECT_EQ(g2.description, g1.description);
  EXPECT_EQ(g2.createdAt, g1.createdAt);
  EXPECT_EQ(g2.updatedAt, g1.updatedAt);
  EXPECT_EQ(g2.dueDate, g1.dueDate);
}

// 📝 Test 6: Move assignment works
TEST(GoalEntityTest, MoveAssignmentWorks)
{
  QDateTime now = QDateTime::currentDateTimeUtc();
  Goal g1("id5", "MoveAssign Goal", "Desc", now, now, now.addDays(2));
  Goal g2;
  g2 = std::move(g1);

  EXPECT_EQ(g2.id, "id5");
  EXPECT_EQ(g2.title, "MoveAssign Goal");
  EXPECT_EQ(g2.description, "Desc");
  EXPECT_EQ(g2.createdAt, now);
  EXPECT_EQ(g2.updatedAt, now);
  ASSERT_TRUE(g2.dueDate.has_value());
  EXPECT_EQ(g2.dueDate.value(), now.addDays(2));
}

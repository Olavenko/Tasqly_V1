/*
 * 🧱 File: test_task.cpp
 * ----------------------
 * 📌 Purpose   : Unit tests for domain entity Task
 * 🧱 Layer     : Tests (Unit - Domain Core)
 * 👤 Author    : Tasqly Team
 * 🗓️ Created   : 2025-09-23
 *
 * 🧠 Description:
 * Verifies enums converters and constructors/assignments of Task entity.
 */

#include "domain/core/Task.h"

#include <gtest/gtest.h>

using namespace tasqly::domain::core;

// 📝 Test 1: toString(TaskStatus) returns correct values
TEST(TaskEntityTest, TaskStatusToString)
{
  EXPECT_EQ(toString(TaskStatus::Pending), "Pending");
  EXPECT_EQ(toString(TaskStatus::InProgress), "InProgress");
  EXPECT_EQ(toString(TaskStatus::Completed), "Completed");
  EXPECT_EQ(toString(TaskStatus::Cancelled), "Cancelled");
}

// 📝 Test 2: taskStatusFromString returns correct enums
TEST(TaskEntityTest, TaskStatusFromString)
{
  EXPECT_EQ(taskStatusFromString("Pending"), TaskStatus::Pending);
  EXPECT_EQ(taskStatusFromString("InProgress"), TaskStatus::InProgress);
  EXPECT_EQ(taskStatusFromString("Completed"), TaskStatus::Completed);
  EXPECT_EQ(taskStatusFromString("Cancelled"), TaskStatus::Cancelled);
  EXPECT_EQ(taskStatusFromString("Invalid"), TaskStatus::Pending); // fallback
}

// 📝 Test 3: toString(TaskPriority) returns correct values
TEST(TaskEntityTest, TaskPriorityToString)
{
  EXPECT_EQ(toString(TaskPriority::Low), "Low");
  EXPECT_EQ(toString(TaskPriority::Normal), "Normal");
  EXPECT_EQ(toString(TaskPriority::High), "High");
  EXPECT_EQ(toString(TaskPriority::Critical), "Critical");
}

// 📝 Test 4: taskPriorityFromString returns correct enums
TEST(TaskEntityTest, TaskPriorityFromString)
{
  EXPECT_EQ(taskPriorityFromString("Low"), TaskPriority::Low);
  EXPECT_EQ(taskPriorityFromString("Normal"), TaskPriority::Normal);
  EXPECT_EQ(taskPriorityFromString("High"), TaskPriority::High);
  EXPECT_EQ(taskPriorityFromString("Critical"), TaskPriority::Critical);
  EXPECT_EQ(taskPriorityFromString("Invalid"), TaskPriority::Normal); // fallback
}

// 📝 Test 5: Default constructor initializes empty fields
TEST(TaskEntityTest, DefaultConstructorInitializesEmpty)
{
  Task t;

  EXPECT_TRUE(t.id.isEmpty());
  EXPECT_TRUE(t.title.isEmpty());
  EXPECT_TRUE(t.description.isEmpty());
  EXPECT_FALSE(t.dueDate.has_value());
}

// 📝 Test 6: Parameterized constructor sets fields correctly
TEST(TaskEntityTest, ParameterizedConstructorSetsFields)
{
  QDateTime now = QDateTime::currentDateTimeUtc();
  Task t("id1",
         "Task title",
         "Task desc",
         TaskStatus::InProgress,
         TaskPriority::High,
         now,
         now,
         now.addDays(3));

  EXPECT_EQ(t.id, "id1");
  EXPECT_EQ(t.title, "Task title");
  EXPECT_EQ(t.description, "Task desc");
  EXPECT_EQ(t.status, TaskStatus::InProgress);
  EXPECT_EQ(t.priority, TaskPriority::High);
  EXPECT_EQ(t.createdAt, now);
  EXPECT_EQ(t.updatedAt, now);
  ASSERT_TRUE(t.dueDate.has_value());
  EXPECT_EQ(t.dueDate.value(), now.addDays(3));
}

// 📝 Test 7: Copy constructor clones fields
TEST(TaskEntityTest, CopyConstructorClonesFields)
{
  QDateTime now = QDateTime::currentDateTimeUtc();
  Task t1("id2", "Copy Task", "Desc", TaskStatus::Pending, TaskPriority::Low, now, now);
  Task t2(t1);

  EXPECT_EQ(t2.id, t1.id);
  EXPECT_EQ(t2.title, t1.title);
  EXPECT_EQ(t2.description, t1.description);
  EXPECT_EQ(t2.status, t1.status);
  EXPECT_EQ(t2.priority, t1.priority);
  EXPECT_EQ(t2.createdAt, t1.createdAt);
  EXPECT_EQ(t2.updatedAt, t1.updatedAt);
  EXPECT_EQ(t2.dueDate, t1.dueDate);
}

// 📝 Test 8: Move constructor transfers fields
TEST(TaskEntityTest, MoveConstructorTransfersFields)
{
  QDateTime now = QDateTime::currentDateTimeUtc();
  Task t1("id3",
          "Move Task",
          "Desc",
          TaskStatus::Completed,
          TaskPriority::Critical,
          now,
          now,
          now.addDays(5));
  Task t2(std::move(t1));

  EXPECT_EQ(t2.id, "id3");
  EXPECT_EQ(t2.title, "Move Task");
  EXPECT_EQ(t2.description, "Desc");
  EXPECT_EQ(t2.status, TaskStatus::Completed);
  EXPECT_EQ(t2.priority, TaskPriority::Critical);
  EXPECT_EQ(t2.createdAt, now);
  EXPECT_EQ(t2.updatedAt, now);
  ASSERT_TRUE(t2.dueDate.has_value());
  EXPECT_EQ(t2.dueDate.value(), now.addDays(5));
}

// 📝 Test 9: Copy assignment works
TEST(TaskEntityTest, CopyAssignmentWorks)
{
  QDateTime now = QDateTime::currentDateTimeUtc();
  Task t1("id4", "Assigned Task", "Desc", TaskStatus::InProgress, TaskPriority::Normal, now, now);
  Task t2;
  t2 = t1;

  EXPECT_EQ(t2.id, t1.id);
  EXPECT_EQ(t2.title, t1.title);
  EXPECT_EQ(t2.description, t1.description);
  EXPECT_EQ(t2.status, t1.status);
  EXPECT_EQ(t2.priority, t1.priority);
  EXPECT_EQ(t2.createdAt, t1.createdAt);
  EXPECT_EQ(t2.updatedAt, t1.updatedAt);
  EXPECT_EQ(t2.dueDate, t1.dueDate);
}

// 📝 Test 10: Move assignment works
TEST(TaskEntityTest, MoveAssignmentWorks)
{
  QDateTime now = QDateTime::currentDateTimeUtc();
  Task t1("id5",
          "MoveAssign Task",
          "Desc",
          TaskStatus::Cancelled,
          TaskPriority::Low,
          now,
          now,
          now.addDays(10));
  Task t2;
  t2 = std::move(t1);

  EXPECT_EQ(t2.id, "id5");
  EXPECT_EQ(t2.title, "MoveAssign Task");
  EXPECT_EQ(t2.description, "Desc");
  EXPECT_EQ(t2.status, TaskStatus::Cancelled);
  EXPECT_EQ(t2.priority, TaskPriority::Low);
  EXPECT_EQ(t2.createdAt, now);
  EXPECT_EQ(t2.updatedAt, now);
  ASSERT_TRUE(t2.dueDate.has_value());
  EXPECT_EQ(t2.dueDate.value(), now.addDays(10));
}

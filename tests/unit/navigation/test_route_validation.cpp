/*
 * 🧱 File: tests/unit/navigation/test_route_validation.cpp
 * -------------------------------------------------------
 * 📌 Purpose   : Unit tests for Route value object (validation & formatting).
 * 🧱 Layer     : Unit Test (Domain/Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-06
 *
 * 🧠 Description:
 * Verifies correctness of Route struct: validation logic and toString formatting.
 */

#include "domain/core/Route.h"
#include <gtest/gtest.h>
#include <QDateTime>

TEST(RouteValidationTest, InvalidWhenEmptyId)
{
  Route r;
  r.id = "";
  r.timestamp = QDateTime::currentDateTime();
  EXPECT_FALSE(r.isValid());
}

TEST(RouteValidationTest, InvalidWhenTimestampInvalid)
{
  Route r;
  r.id = "dashboard";
  r.timestamp = QDateTime(); // invalid
  EXPECT_FALSE(r.isValid());
}

TEST(RouteValidationTest, ValidWhenIdAndTimestampSet)
{
  Route r;
  r.id = "settings";
  r.timestamp = QDateTime::currentDateTime();
  EXPECT_TRUE(r.isValid());
}

TEST(RouteValidationTest, ToStringContainsIdAndTimestamp)
{
  Route r;
  r.id = "home";
  r.timestamp = QDateTime::fromString("2025-09-06T12:00:00", Qt::ISODate);
  QString str = r.toString();
  EXPECT_TRUE(str.contains("home"));
  EXPECT_TRUE(str.contains("2025-09-06"));
}

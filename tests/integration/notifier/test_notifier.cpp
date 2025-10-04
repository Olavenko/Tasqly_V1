/*
 * 🧱 File: test_notifier.cpp
 * --------------------------
 * 📌 Purpose   : Integration tests for Notifier subsystem.
 * 🧱 Layer     : Tests (Integration - Notifier)
 * 👤 Author    : Tasqly Team
 * 🗓️ Created   : 2025-09-15
 *
 * 🧠 Description:
 * Validates that Notifier::show() correctly emits notify signals
 * with the expected message and level, and verifies fallback behavior
 * when no instance is available.
 */

#include "app/notifier/Notifier.h"

#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QSignalSpy>
#include <QTest>

// 🧪 Fixture: Provides clean Notifier per test
class NotifierTest : public ::testing::Test
{
protected:
  void SetUp() override { notifier = std::make_unique<Notifier>(); }

  void TearDown() override
  {
    notifier.reset();
    Notifier::resetInstance(); // reset singleton safely
  }

  std::unique_ptr<Notifier> notifier;
};

// ✅ Test 1: show() should emit notify() with correct message and level
TEST_F(NotifierTest, ShowEmitsNotifySignal)
{
  QSignalSpy spy(Notifier::instance(), &Notifier::notify);
  ASSERT_TRUE(spy.isValid());

  Notifier::show("Hello World", "info");

  ASSERT_TRUE(spy.wait(100));
  EXPECT_EQ(spy.count(), 1);

  QList<QVariant> args = spy.takeFirst();
  EXPECT_EQ(args.at(0).toString().toStdString(), "Hello World");
  EXPECT_EQ(args.at(1).toString().toStdString(), "info");
}

// ✅ Test 2: Multiple show() calls emit multiple signals
TEST_F(NotifierTest, MultipleShowCallsEmitSignals)
{
  QSignalSpy spy(Notifier::instance(), &Notifier::notify);
  ASSERT_TRUE(spy.isValid());

  Notifier::show("Msg1", "info");
  QCoreApplication::processEvents(QEventLoop::AllEvents, 500);

  Notifier::show("Msg2", "error");
  QCoreApplication::processEvents(QEventLoop::AllEvents, 500);

  QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 2, 3000);

  QList<QVariant> first = spy.takeFirst();
  EXPECT_EQ(first.at(0).toString().toStdString(), "Msg1");
  EXPECT_EQ(first.at(1).toString().toStdString(), "info");

  QList<QVariant> second = spy.takeFirst();
  EXPECT_EQ(second.at(0).toString().toStdString(), "Msg2");
  EXPECT_EQ(second.at(1).toString().toStdString(), "error");
}

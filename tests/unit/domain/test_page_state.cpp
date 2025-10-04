/*
 * 🧱 File: test_page_state.cpp
 * ----------------------------
 * 📌 Purpose   : Unit tests for PageState enum and helper functions.
 * 🧱 Layer     : Testing (Unit)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-14
 *
 * 🧠 Description:
 * Verifies that all PageState enum values are covered, correctly converted
 * to string via toString(), and properly exposed to the Qt meta-object system.
 *
 * 🔗 Depends On:
 *   - domain/core/PageState.h
 * 🔗 Related   : PageStateWrapper (Q_ENUM, QObject integration)
 * ✅ Tested In : TasqlyTestsRunner (GoogleTest)
 * 🕒 Last Major Change: 2025-09-14
 */

#include "domain/core/PageState.h"

#include <gtest/gtest.h>
#include <QMetaEnum>
#include <QString>

// 👉 Fixture
class PageStateTest : public ::testing::Test
{};

// ✅ All enum values map to correct strings
TEST_F(PageStateTest, ToStringMatchesEnumValues)
{
  EXPECT_EQ(toString(PageStateWrapper::Ready), "Ready");
  EXPECT_EQ(toString(PageStateWrapper::Loading), "Loading");
  EXPECT_EQ(toString(PageStateWrapper::Empty), "Empty");
  EXPECT_EQ(toString(PageStateWrapper::Error), "Error");
}

// ✅ Enum is registered in Qt meta system
TEST_F(PageStateTest, EnumIsRegisteredInQtMeta)
{
  const QMetaObject &meta = PageStateWrapper::staticMetaObject;
  int idx = meta.indexOfEnumerator("PageState");
  ASSERT_GE(idx, 0) << "PageState enum not registered with QMetaObject";

  QMetaEnum e = meta.enumerator(idx);
  EXPECT_STREQ(e.valueToKey(PageStateWrapper::Ready), "Ready");
  EXPECT_STREQ(e.valueToKey(PageStateWrapper::Loading), "Loading");
  EXPECT_STREQ(e.valueToKey(PageStateWrapper::Empty), "Empty");
  EXPECT_STREQ(e.valueToKey(PageStateWrapper::Error), "Error");
}

// 🔄 State transitions are valid
TEST_F(PageStateTest, StateTransitions)
{
  PageStateWrapper::PageState state = PageStateWrapper::Ready;
  EXPECT_EQ(toString(state), "Ready");

  state = PageStateWrapper::Loading;
  EXPECT_EQ(toString(state), "Loading");

  state = PageStateWrapper::Empty;
  EXPECT_EQ(toString(state), "Empty");

  state = PageStateWrapper::Error;
  EXPECT_EQ(toString(state), "Error");
}

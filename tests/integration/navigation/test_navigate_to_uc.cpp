/*
 * 🧱 File: tests/integration/navigation/test_navigate_to_uc.cpp
 * ------------------------------------------------------------
 * 📌 Purpose   : Integration tests for NavigateTo use case.
 * 🧱 Layer     : Integration Test (Application/Use Cases)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-06
 *
 * 🧠 Description:
 * Tests NavigateTo + NavigationService interaction:
 * - Valid/invalid navigation requests.
 * - Loader state transitions (Loading → Ready/Error).
 * - Retry logic preserving context.
 */

#include "app/usecases/NavigateTo.h"
#include "domain/core/PageState.h"
#include "domain/core/Route.h"
#include <gtest/gtest.h>
#include <QDateTime>
#include <QSignalSpy>
#include <QVariantMap>

Q_DECLARE_METATYPE(Route)
Q_DECLARE_METATYPE(PageStateWrapper::PageState)

// 🧩 Dummy logger (implements ILogManager with no-op methods)
class DummyLogger : public tasqly::app::logging::ILogManager
{
public:
  void setLevel(tasqly::app::logging::Level) override {}
  tasqly::app::logging::Level level() const override { return tasqly::app::logging::Level::Info; }
  bool isEnabled(tasqly::app::logging::Level) const override { return true; }

  void enableConsole(bool) override {}
  void enableFileSink(bool, const QString& = QString()) override {}
  void setRotationPolicy(const tasqly::app::logging::RotationPolicy&) override {}
  void rotateNow() override {}
  void flush() override {}

  void addMaskedKey(const QString&) override {}
  void setMaskedKeys(const QStringList&) override {}
  QStringList maskedKeys() const override { return {}; }

  void log(tasqly::app::logging::Level,
           const QString&,
           const QString&,
           const QVariantMap& = {}) override
  {}
};

TEST(NavigateToUseCaseTest, ExecuteValidRouteChangesStateToReady)
{
  qRegisterMetaType<Route>("Route");
  qRegisterMetaType<PageStateWrapper::PageState>("PageStateWrapper::PageState");

  DummyLogger logger;
  NavigationService nav(&logger);
  NavigateTo usecase(&nav, &logger);

  QSignalSpy spy(&usecase, &NavigateTo::pageStateChanged);

  QVariantMap data;
  data["id"] = "dashboard";
  data["timestamp"] = QDateTime::currentDateTime();

  usecase.execute(data);

  ASSERT_GE(spy.count(), 2); // Loading + Ready
  EXPECT_EQ(spy.takeFirst().at(0).toInt(), PageStateWrapper::Loading);
  EXPECT_EQ(spy.takeLast().at(0).toInt(), PageStateWrapper::Ready);
}

TEST(NavigateToUseCaseTest, ExecuteInvalidRouteChangesStateToError)
{
  qRegisterMetaType<Route>("Route");
  qRegisterMetaType<PageStateWrapper::PageState>("PageStateWrapper::PageState");

  DummyLogger logger;
  NavigationService nav(&logger);
  NavigateTo usecase(&nav, &logger);

  QSignalSpy spy(&usecase, &NavigateTo::pageStateChanged);

  QVariantMap data; // missing id + timestamp
  usecase.execute(data);

  ASSERT_GE(spy.count(), 2); // Loading + Error
  EXPECT_EQ(spy.takeLast().at(0).toInt(), PageStateWrapper::Error);
}

TEST(NavigateToUseCaseTest, RetryUsesLastValidRoute)
{
  qRegisterMetaType<Route>("Route");
  qRegisterMetaType<PageStateWrapper::PageState>("PageStateWrapper::PageState");

  DummyLogger logger;
  NavigationService nav(&logger);
  NavigateTo usecase(&nav, &logger);

  QSignalSpy spy(&usecase, &NavigateTo::pageStateChanged);

  QVariantMap data;
  data["id"] = "settings";
  data["timestamp"] = QDateTime::currentDateTime();
  usecase.execute(data);
  spy.clear();

  usecase.retry();

  ASSERT_GE(spy.count(), 2); // Loading + Ready again
  EXPECT_EQ(spy.takeLast().at(0).toInt(), PageStateWrapper::Ready);
}

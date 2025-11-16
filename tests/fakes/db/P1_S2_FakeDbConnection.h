#pragma once

/*
 * 🧱 File: P1_S2_FakeDbConnection.h
 * -------------------------------
 * 📌 Purpose   : Fake implementation of IDbConnection for testing.
 * 🧱 Layer     : Test/Fake
 * 🎯 Slice     : Phase 1 — Slice 2.5 (DB Abstraction)
 *
 * 📄 Notes:
 * - Used to simulate DB behavior in unit/integration tests.
 * - Supports: success, failure, and fail-then-recover modes.
 * - Tracks call sequence for retry verification.
 */

#include <string>
#include <vector>

#include "infra/db/P1_S2_IDbConnection.h"

namespace tasqly::p1::s2::fakes::db {

class P1_S2_FakeDbConnection : public tasqly::p1::s2::infra::db::IDbConnection
{
public:
  P1_S2_FakeDbConnection();

  // ------------------------------------
  //       Configuration Methods
  // ------------------------------------
  // Force next execute() to fail
  void setFailMode(bool enabled);

  // Fail first time, succeed after reset()
  void setFailThenRecoverMode(bool enabled);

  // Manually set valid/invalid state
  void setValid(bool v);

  // ------------------------------------
  //       IDbConnection API
  // ------------------------------------
  tasqly::p1::s2::infra::db::DbResult execute(const std::string& sql) override;

  bool reset() override;

  bool isValid() const override;

  // ------------------------------------
  //       Testing Helpers
  // ------------------------------------
  const std::vector<std::string>& log() const;

  // 👉 Testing helper: override next SELECT result
  void setSelectResult(const infra::db::DbQueryResult& r);

private:
  bool m_valid = true;
  bool m_failMode = false;
  bool m_failThenRecoverMode = false;

  bool m_firstFailTriggered = false;

  std::vector<std::string> m_log;

  infra::db::DbQueryResult m_forcedResult;
  bool m_hasForcedResult = false;
};

} // namespace tasqly::p1::s2::fakes::db

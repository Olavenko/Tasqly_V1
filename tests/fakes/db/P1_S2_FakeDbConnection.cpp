/*
 * 🧱 File: P1_S2_FakeDbConnection.cpp
 * -----------------------------------
 * 📌 Purpose   : Fake implementation of IDbConnection for testing.
 * 🧱 Layer     : Test/Fake
 * 🎯 Slice     : Phase 1 — Slice 2.5 (DB Abstraction)
 */

#include "P1_S2_FakeDbConnection.h"

namespace tasqly::p1::s2::fakes::db {

using namespace tasqly::p1::s2::infra::db;
using InfraError = tasqly::p1::infra::runtime::P1_Error;

// ---------------------------------------------------------
//  Constructor
// ---------------------------------------------------------
P1_S2_FakeDbConnection::P1_S2_FakeDbConnection()
{
  m_log.emplace_back("init");
}

// ---------------------------------------------------------
//  Configuration
// ---------------------------------------------------------
void P1_S2_FakeDbConnection::setFailMode(bool enabled)
{
  m_failMode = enabled;
  m_log.emplace_back(enabled ? "config:failMode=on" : "config:failMode=off");
}

void P1_S2_FakeDbConnection::setFailThenRecoverMode(bool enabled)
{
  m_failThenRecoverMode = enabled;
  m_firstFailTriggered = false; // ⭐ IMPORTANT ⭐ reset fail state
  m_log.emplace_back(enabled ? "config:failThenRecover=on" : "config:failThenRecover=off");
}

void P1_S2_FakeDbConnection::setValid(bool v)
{
  m_valid = v;
  m_log.emplace_back(v ? "setValid:true" : "setValid:false");
}

// ---------------------------------------------------------
//  IDbConnection — execute()
// ---------------------------------------------------------
DbResult P1_S2_FakeDbConnection::execute(const std::string& sql)
{
  // Always log call
  m_log.emplace_back("execute-called");
  m_log.emplace_back("execute:" + sql);

  // 1) Invalid connection
  if (!m_valid) {
    m_log.emplace_back("invalid-connection");
    return DbResult::Err(InfraError::makeDbInit("Fake: connection invalid"));
  }

  // 2) Always-fail mode
  if (m_failMode) {
    m_log.emplace_back("fail-mode");
    return DbResult::Err(InfraError::makeDbInit("Fake: forced failure mode"));
  }

  // 3) Fail-then-recover mode
  if (m_failThenRecoverMode && !m_firstFailTriggered) {
    m_firstFailTriggered = true;
    m_log.emplace_back("fail-then-recover:fail");
    return DbResult::Err(InfraError::makeDbInit("Fake: first failure (recoverable)"));
  }

  // 4) Forced result (success with user data)
  if (m_hasForcedResult) {
    m_log.emplace_back("fail-then-recover:recover"); // if relevant
    m_log.emplace_back("forced-result");
    return DbResult::Ok(m_forcedResult);
  }

  // 5) Normal auto success
  m_log.emplace_back("success-normal");
  DbQueryResult res;
  res.columns = {"col1", "col2"};
  res.rows.push_back({"value1", "value2"});
  return DbResult::Ok(res);
}

// ---------------------------------------------------------
//  IDbConnection — reset()
// ---------------------------------------------------------
bool P1_S2_FakeDbConnection::reset()
{
  m_log.emplace_back("reset");

  if (m_failThenRecoverMode && m_firstFailTriggered) {
    m_valid = true;
    m_log.emplace_back("reset:recovered");
    return true;
  }

  m_valid = true;
  return true;
}

// ---------------------------------------------------------
//  IDbConnection — isValid()
// ---------------------------------------------------------
bool P1_S2_FakeDbConnection::isValid() const
{
  return m_valid;
}

// ---------------------------------------------------------
//  Testing Helpers — log()
// ---------------------------------------------------------
const std::vector<std::string>& P1_S2_FakeDbConnection::log() const
{
  return m_log;
}

void P1_S2_FakeDbConnection::setSelectResult(const DbQueryResult& r)
{
  m_forcedResult = r;
  m_hasForcedResult = true;
}

} // namespace tasqly::p1::s2::fakes::db

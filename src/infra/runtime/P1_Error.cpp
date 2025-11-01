/*
 * 🧱 File: P1_Error.cpp
 * ---------------------
 * 📌 Purpose   : Implementation of P1_Error — runtime error abstraction for infrastructure subsystems.
 * 🧱 Layer     : Infrastructure (Runtime)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-21
 * 🔖 Version   : 1.0
 *
 * 🧠 Description:
 * Provides implementations for runtime-to-domain conversion, stringification,
 * and optional masking of sensitive context entries. Used by Logger, Notifier,
 * and Factory subsystems in Phase 1.
 *
 * 🔗 Related:
 *   - P1_Error.h
 *   - P1_DomainError.h
 *   - P1_Logger (consumer)
 *   - P1_Notifier (consumer)
 */

#include "P1_Error.h"
#include <regex>
#include <sstream>

namespace tasqly::p1::infra::runtime {

static std::string maskValue(const std::string& key, const std::string& value)
{
  static const std::regex sensitivePattern("(pass|token|key|secret)", std::regex::icase);
  return std::regex_search(key, sensitivePattern) ? "<masked>" : value;
}

tasqly::p1::s1::domain::core::DomainError P1_Error::toDomainError() const
{
  std::ostringstream oss;
  oss << "[runtime:" << static_cast<int>(code) << "] " << message;

  if (!context.empty()) {
    oss << " [ctx:";
    bool first = true;
    for (const auto& [k, v] : context) {
      if (!first)
        oss << ",";
      first = false;
      oss << k << "=" << maskValue(k, v);
    }
    oss << "]";
  }

  return tasqly::p1::s1::domain::core::DomainError::makeStorage(oss.str());
}

P1_Error P1_Error::fromDomainError(const tasqly::p1::s1::domain::core::DomainError& d)
{
  std::unordered_map<std::string, std::string> ctx;
  ctx["domain_code"] = std::to_string(static_cast<int>(d.code));
  return {RuntimeErrorCode::Config, "[domain] " + d.message, std::move(ctx)};
}

std::string P1_Error::toString(bool masked) const
{
  std::ostringstream oss;
  oss << "[" << codeToString(code) << "] " << message;

  if (!context.empty()) {
    oss << " | context={";
    bool first = true;
    for (const auto& [k, v] : context) {
      if (!first)
        oss << ", ";
      first = false;
      oss << k << "=" << (masked ? maskValue(k, v) : v);
    }
    oss << "}";
  }

  return oss.str();
}

std::string P1_Error::codeToString(RuntimeErrorCode c)
{
  switch (c) {
  case RuntimeErrorCode::None:
    return "None";
  case RuntimeErrorCode::DbInit:
    return "DbInit";
  case RuntimeErrorCode::DbAuth:
    return "DbAuth";
  case RuntimeErrorCode::Timeout:
    return "Timeout";
  case RuntimeErrorCode::Network:
    return "Network";
  case RuntimeErrorCode::Io:
    return "Io";
  case RuntimeErrorCode::Config:
    return "Config";
  default:
    return "Unknown";
  }
}

} // namespace tasqly::p1::infra::runtime

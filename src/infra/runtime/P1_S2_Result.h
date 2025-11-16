#pragma once

/*
 * 🧱 File: P1_S2_Result.h
 * -----------------------
 * 📌 Purpose   : Generic value-or-error wrapper for Infra & DB layers.
 * 🧱 Layer     : Infrastructure (Runtime)
 * 🎯 Slice     : Phase 1 — Slice 2.5 (DB Abstraction)
 *
 * 📄 Notes:
 * - Header-only.
 * - No exceptions.
 * - Integrates with tasqly::p1::infra::runtime::P1_Error.
 */

#include "P1_Error.h"
#include <optional>
#include <utility>

namespace tasqly::p1::s2::infra::runtime {

using InfraError = tasqly::p1::infra::runtime::P1_Error;

template<typename T>
class Result
{
public:
  // 📝 Construct success
  static Result<T> Ok(T value) { return Result<T>(std::move(value), std::nullopt); }

  // 📝 Construct error
  static Result<T> Err(const InfraError& error) { return Result<T>(std::nullopt, error); }

  // 🧪 Check success
  bool isOk() const noexcept { return m_value.has_value(); }

  // ❌ Check error
  bool isErr() const noexcept { return m_error.has_value(); }

  // 🎁 Get value
  const T& value() const { return m_value.value(); }
  T& value() { return m_value.value(); }

  // ⚠️ Get error
  const InfraError& error() const { return m_error.value(); }

private:
  std::optional<T> m_value;
  std::optional<InfraError> m_error;

  // 🔒 Private constructor
  Result(std::optional<T> v, std::optional<InfraError> e)
      : m_value(std::move(v))
      , m_error(std::move(e))
  {}
};

} // namespace tasqly::p1::s2::infra::runtime

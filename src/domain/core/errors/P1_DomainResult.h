/*
 * 🧱 File: P1_DomainResult.h
 * --------------------------
 * 📌 Purpose   : Safe and pure C++ Result<T, DomainError> implementation (Phase 1 isolated version)
 * 🧱 Layer     : Domain (Core) — Errors / Result System
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-12
 * 🔖 Version   : 1.3 (constexpr fix for non-literal DomainError)
 * 🛡️ Stability : Stable
 */

#pragma once
#include "P1_DomainError.h"
#include <cassert>
#include <type_traits>
#include <utility>
#include <variant>

namespace tasqly::p1::s1::domain::core {

/// @brief Represents either a success value (T) or a DomainError.
template<typename T>
class DomainResult
{
public:
  using value_type = T;
  using error_type = DomainError;

  // ✅ Factory functions (move-safe)
  static constexpr DomainResult ok(T v) noexcept(std::is_nothrow_move_constructible_v<T>)
  {
    DomainResult res;
    res._data.template emplace<0>(std::move(v));
    return res;
  }

  // ❌ cannot be constexpr because DomainError is not a literal type
  static DomainResult err(DomainError e) noexcept
  {
    DomainResult res;
    res._data.template emplace<1>(std::move(e));
    return res;
  }

  // ✅ Queries
  [[nodiscard]] constexpr bool isOk() const noexcept { return _data.index() == 0; }
  [[nodiscard]] constexpr bool isErr() const noexcept { return _data.index() == 1; }
  explicit constexpr operator bool() const noexcept { return isOk(); }

  // ✅ Accessors
  constexpr const T& value() const noexcept
  {
    assert(isOk());
    return std::get<0>(_data);
  }

  constexpr T& value() noexcept
  {
    assert(isOk());
    return std::get<0>(_data);
  }

  const DomainError& error() const noexcept
  {
    assert(isErr());
    return std::get<1>(_data);
  }

private:
  std::variant<T, DomainError> _data{};
};

/// @brief Partial specialization for void — for operations with no return value.
template<>
class DomainResult<void>
{
public:
  using error_type = DomainError;

  static constexpr DomainResult ok() noexcept
  {
    DomainResult res;
    res._data.emplace<0>();
    return res;
  }

  // ❌ cannot be constexpr — DomainError not literal
  static DomainResult err(DomainError e) noexcept
  {
    DomainResult res;
    res._data.emplace<1>(std::move(e));
    return res;
  }

  [[nodiscard]] constexpr bool isOk() const noexcept { return _data.index() == 0; }
  [[nodiscard]] constexpr bool isErr() const noexcept { return _data.index() == 1; }
  explicit constexpr operator bool() const noexcept { return isOk(); }

  const DomainError& error() const noexcept
  {
    assert(isErr());
    return std::get<1>(_data);
  }

private:
  std::variant<std::monostate, DomainError> _data{};
};

} // namespace tasqly::p1::s1::domain::core

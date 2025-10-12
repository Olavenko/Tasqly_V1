/*
 * 🧱 File: DomainResult.h
 * -----------------------
 * 📌 Purpose   : Safe and pure C++ Result<T, DomainError> implementation.
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-12
 * 🔖 Version   : 1.1 (Memory-safe + noexcept)
 * 🛡️ Stability : Stable
 */

#pragma once
#include "DomainError.h"
#include <cassert>
#include <utility>
#include <variant>

namespace tasqly::domain::core {

template<typename T>
class DomainResult
{
public:
  using value_type = T;
  using error_type = DomainError;

  // ✅ Factory functions (move-safe)
  static DomainResult ok(T v) noexcept(std::is_nothrow_move_constructible_v<T>)
  {
    DomainResult res;
    res._data.template emplace<0>(std::move(v));
    return res;
  }

  static DomainResult err(DomainError e) noexcept
  {
    DomainResult res;
    res._data.template emplace<1>(std::move(e));
    return res;
  }

  // ✅ Queries
  bool isOk() const noexcept { return _data.index() == 0; }
  bool isErr() const noexcept { return _data.index() == 1; }
  explicit operator bool() const noexcept { return isOk(); }

  // ✅ Accessors (safe with asserts)
  const T& value() const noexcept
  {
    assert(isOk());
    return std::get<0>(_data);
  }

  T& value() noexcept
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

/// ✅ Specialization for void
template<>
class DomainResult<void>
{
public:
  using error_type = DomainError;

  static DomainResult ok() noexcept
  {
    DomainResult res;
    res._data.emplace<0>();
    return res;
  }

  static DomainResult err(DomainError e) noexcept
  {
    DomainResult res;
    res._data.emplace<1>(std::move(e));
    return res;
  }

  bool isOk() const noexcept { return _data.index() == 0; }
  bool isErr() const noexcept { return _data.index() == 1; }
  explicit operator bool() const noexcept { return isOk(); }

  const DomainError& error() const noexcept
  {
    assert(isErr());
    return std::get<1>(_data);
  }

private:
  std::variant<std::monostate, DomainError> _data{};
};

} // namespace tasqly::domain::core

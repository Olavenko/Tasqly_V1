/*
 * 🧱 File: Result.h
 * -----------------
 * 📌 Purpose   : Result<T, Error> monadic type for success/failure propagation
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-02
 *
 * 🧠 Description:
 * Header-only Result<T, Error> built on std::variant. Provides ergonomic APIs:
 * - Construction: ok(...), err(...)
 * - Introspection: isOk(), isErr(), operator bool()
 * - Accessors: value(), error(), valueOr(...)
 * - Transformers: map(...), mapError(...), andThen(...), orElse(...), match(...)
 * Intended to be used across app/use-cases/infra and QML facades.
 */

#pragma once

#include "Error.h"

#include <functional>
#include <type_traits>
#include <utility>
#include <variant>

namespace tasqly::domain::core {

// 🧩 Forward helpers
template<typename T>
class Result;

// ➕ Helper to construct success
template<typename T>
[[nodiscard]] inline Result<std::decay_t<T>> ok(T&& v);

// ➕ Helper to construct failure
[[nodiscard]] inline Result<void> ok(); // deleted (no void specialization in this header)
[[nodiscard]] inline Result<int> err(int) = delete; // guard misuse

// 🧱 Result<T, Error> primary template (T must not be void)
template<typename T>
class Result
{
  static_assert(!std::is_void_v<T>, "Result<void, Error> is not supported in this header.");

public:
  using value_type = T;
  using error_type = Error;

  // 🧱 Ctors
  // ➕ Constructs a successful result
  static Result ok(T v) noexcept(std::is_nothrow_move_constructible_v<T>)
  {
    return Result(std::in_place_index<0>, std::move(v));
  }

  // ➕ Constructs a failed result
  static Result err(Error e) noexcept { return Result(std::in_place_index<1>, std::move(e)); }

  // ✅ State queries
  // 🔎 Returns true if this is a success
  [[nodiscard]] bool isOk() const noexcept { return std::holds_alternative<T>(_data); }

  // 🔎 Returns true if this is a failure
  [[nodiscard]] bool isErr() const noexcept { return std::holds_alternative<Error>(_data); }

  // 👌 Bool conversion (true when ok)
  [[nodiscard]] explicit operator bool() const noexcept { return isOk(); }

  // 📦 Accessors (throw-free; precondition on the caller)
  // ⚠️ These accessors assume state; check isOk()/isErr() first in non-test code.

  // 🟢 Returns reference to value (const lvalue)
  const T& value() const& noexcept { return std::get<0>(_data); }

  // 🟢 Returns reference to value (lvalue)
  T& value() & noexcept { return std::get<0>(_data); }

  // 🟢 Moves out the value (rvalue)
  T&& value() && noexcept { return std::move(std::get<0>(_data)); }

  // 🔴 Returns reference to error (const lvalue)
  const Error& error() const& noexcept { return std::get<1>(_data); }

  // 🔴 Returns reference to error (lvalue)
  Error& error() & noexcept { return std::get<1>(_data); }

  // 🔴 Moves out the error (rvalue)
  Error&& error() && noexcept { return std::move(std::get<1>(_data)); }

  // 🛟 Returns value if ok, otherwise returns provided default (by value or callable)
  template<typename U>
  [[nodiscard]] T valueOr(U&& fallback) const
  {
    if (isOk())
      return std::get<0>(_data);
    if constexpr (std::is_invocable_v<U>) {
      return static_cast<T>(std::invoke(std::forward<U>(fallback)));
    } else {
      return static_cast<T>(std::forward<U>(fallback));
    }
  }

  // 🎨 map: transforms T -> U when ok, keeps error otherwise
  template<typename F>
  [[nodiscard]] auto map(F&& f) const -> Result<std::decay_t<std::invoke_result_t<F, const T&>>>
  {
    using U = std::decay_t<std::invoke_result_t<F, const T&>>;
    if (isOk()) {
      return Result<U>::ok(std::invoke(std::forward<F>(f), std::get<0>(_data)));
    }
    return Result<U>::err(std::get<1>(_data));
  }

  // 🧯 mapError: transforms Error -> Error' when err, keeps value otherwise
  template<typename F>
  [[nodiscard]] Result mapError(F&& f) const
  {
    if (isErr()) {
      return Result::err(std::invoke(std::forward<F>(f), std::get<1>(_data)));
    }
    return *this; // copy elision
  }

  // 🔗 andThen: chains T -> Result<U>
  template<typename F>
  [[nodiscard]] auto andThen(F&& f) const -> std::decay_t<std::invoke_result_t<F, const T&>>
  {
    using Ret = std::decay_t<std::invoke_result_t<F, const T&>>;
    static_assert(std::is_same_v<typename Ret::error_type, Error>,
                  "andThen expects a function returning Result<U, Error>.");
    if (isOk()) {
      return std::invoke(std::forward<F>(f), std::get<0>(_data));
    }
    return Ret::err(std::get<1>(_data));
  }

  // 🛠️ orElse: handles error via F(Error) -> Result<T>
  template<typename F>
  [[nodiscard]] Result orElse(F&& f) const
  {
    if (isErr()) {
      return std::invoke(std::forward<F>(f), std::get<1>(_data));
    }
    return *this;
  }

  // 🔀 match: branch on state → returns whatever the callables return (must have same type)
  template<typename VO, typename VE>
  [[nodiscard]] auto match(VO&& onOk, VE&& onErr) const
      -> std::common_type_t<std::invoke_result_t<VO, const T&>,
                            std::invoke_result_t<VE, const Error&>>
  {
    using ROk = std::invoke_result_t<VO, const T&>;
    using RErr = std::invoke_result_t<VE, const Error&>;
    static_assert(std::is_same_v<ROk, RErr>, "match branches must return the same type");
    if (isOk())
      return std::invoke(std::forward<VO>(onOk), std::get<0>(_data));
    else
      return std::invoke(std::forward<VE>(onErr), std::get<1>(_data));
  }

private:
  // 🧱 Private ctor from variant index
  template<typename... Args>
  explicit Result(std::in_place_index_t<0>, Args&&... args)
      : _data(std::in_place_index<0>, std::forward<Args>(args)...)
  {}

  template<typename... Args>
  explicit Result(std::in_place_index_t<1>, Args&&... args)
      : _data(std::in_place_index<1>, std::forward<Args>(args)...)
  {}

  // 🧱 Storage
  std::variant<T, Error> _data;
};

// 🔧 Free helpers — ergonomic constructors

// ➕ Creates a successful Result<T>
template<typename T>
[[nodiscard]] inline Result<std::decay_t<T>> ok(T&& v)
{
  using U = std::decay_t<T>;
  return Result<U>::ok(std::forward<T>(v));
}

// ➕ Creates a failed Result<T> by specifying T explicitly
template<typename T>
[[nodiscard]] inline Result<T> err(const Error& e)
{
  return Result<T>::err(e);
}

template<typename T>
[[nodiscard]] inline Result<T> err(Error&& e)
{
  return Result<T>::err(std::move(e));
}

} // namespace tasqly::domain::core

/*
 * 🧱 File: test_result.cpp
 * ------------------------
 * 📌 Purpose   : Smoke test for Result<T,Error> type
 * 🧱 Layer     : Tests (GoogleTest)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-08-29
 *
 * 🧠 Description:
 * Provides a basic test case for a simplified Result<T,Error> implementation.
 * Ensures that success and failure states behave as expected.
 */

#include <gtest/gtest.h>
#include <variant>
#include <string>

// ---------------------------------------------------------------
// 🧩 Minimal stub for Result<T,Error>
// ---------------------------------------------------------------
struct Error {
    std::string message;
};

// ➕ Result<T> implementation using std::variant
template <typename T>
class Result {
public:
    // Construct from value
    Result(const T& value) : data(value) {}

    // Construct from error
    Result(const Error& error) : data(error) {}

    // ✅ Check if result is Ok
    bool isOk() const { return std::holds_alternative<T>(data); }

    // ❌ Check if result is Error
    bool isError() const { return std::holds_alternative<Error>(data); }

    // Get value (caller must ensure isOk() first)
    T value() const { return std::get<T>(data); }

    // Get error (caller must ensure isError() first)
    Error error() const { return std::get<Error>(data); }

private:
    std::variant<T, Error> data;
};

// 🧪 Unit tests for Result<T>
TEST(ResultTest, OkCase) {
    Result<int> r(42);
    EXPECT_TRUE(r.isOk());
    EXPECT_FALSE(r.isError());
    EXPECT_EQ(r.value(), 42);
}

TEST(ResultTest, ErrorCase) {
    Error e{"Something went wrong"};
    Result<int> r(e);
    EXPECT_TRUE(r.isError());
    EXPECT_FALSE(r.isOk());
    EXPECT_EQ(r.error().message, "Something went wrong");
}
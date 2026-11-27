#pragma once

#include "unpack_and_build_tests/core/appenders/append_result_enum.hpp"
#include "unpack_and_build_tests/core/appenders/fixed_size.hpp"
#include "unpack_and_build_tests/core/appenders/insert_after.hpp"
#include "unpack_and_build_tests/core/appenders/multi_insert.hpp"
#include "unpack_and_build_tests/core/appenders/push_back.hpp"
#include "unpack_and_build_tests/core/appenders/unique_insert.hpp"
/*!
 * @file
 * @brief Tests for default_appender, appender_c and buildable_sequence_c, with
 * user types. Which appender each std type gets is tested with its support
 * header.
 *
 * All static_asserts, plus one Catch2 test.
 */
#include "unpack_and_build/core/appenders.hpp"
#include "unpack_and_build/core/appenders/multi_insert.hpp"
#include "unpack_and_build/core/appenders/push_back.hpp"

#include <catch2/catch_test_macros.hpp>

#include <deque>
#include <optional>
#include <type_traits>
#include <vector>

// ============================================================ test types

namespace appender_test_types
{
// Has both push_back and insert(value), but no specialisation: nothing is
// detected, so it can't be built.
struct push_back_and_insert
{
    using value_type = int;
    std::vector<int> values;

    constexpr void
        push_back(
            int&& value_arg
        )
    {
        values.push_back(value_arg);
    }

    constexpr void
        insert(
            int&& value_arg
        )
    {
        values.push_back(value_arg);
    }

    constexpr const int*
        begin() const noexcept
    {
        return values.data();
    }

    constexpr const int*
        end() const noexcept
    {
        return values.data() + values.size();
    }
};

// Like push_back_and_insert, but specialises default_appender to use insert.
struct chooses_insert : push_back_and_insert
{};

// A sequence nothing can be added to.
struct read_only
{
    using value_type = int;

    constexpr const int*
        begin() const noexcept
    {
        return nullptr;
    }

    constexpr const int*
        end() const noexcept
    {
        return nullptr;
    }
};

// Has push_back, but the user writes their own appender: elements are
// buffered, and only written (doubled) in finish().
struct doubled
{
    using value_type = int;
    std::vector<int> values;

    constexpr const int*
        begin() const noexcept
    {
        return values.data();
    }

    constexpr const int*
        end() const noexcept
    {
        return values.data() + values.size();
    }

    constexpr void
        push_back(
            int&& value_arg
        )
    {
        values.push_back(value_arg);
    }
};
} // namespace appender_test_types

UAB_NS_BEGIN

template <>
struct default_appender<appender_test_types::chooses_insert>
    : multi_insert_appender<appender_test_types::chooses_insert>
{
    using multi_insert_appender<
        appender_test_types::chooses_insert>::multi_insert_appender;
};

template <>
struct default_appender<appender_test_types::doubled>
{
    constexpr explicit default_appender(
        appender_test_types::doubled& container_arg
    ) noexcept
        : _container(&container_arg)
    {}

    constexpr append_result
        operator()(
            int&& value_arg
        )
    {
        _pending.push_back(value_arg * 2);
        return append_result::added;
    }

    [[nodiscard]]
    constexpr bool
        complete() const noexcept
    {
        return true;
    }

    constexpr void
        finish()
    {
        _container->values = _pending;
    }
private:
    appender_test_types::doubled* _container;
    std::vector<int>              _pending;
};

namespace appenders_tests
{
using namespace ::appender_test_types;

// An appender for std::vector<int> missing finish(): not an appender.
struct no_finish
{
    constexpr explicit no_finish(std::vector<int>&) noexcept {}

    constexpr append_result
        operator()(int&&)
    {
        return append_result::added;
    }

    [[nodiscard]]
    constexpr bool
        complete() const noexcept
    {
        return true;
    }
};

// ============================================================
// default_appender

// The user's specialisations are used.
static_assert(std::is_base_of_v<
              multi_insert_appender<chooses_insert>,
              default_appender<chooses_insert>>);
static_assert(not std::is_base_of_v<
              push_back_appender<doubled>,
              default_appender<doubled>>);

// ============================================================
// buildable_sequence_c

static_assert(buildable_sequence_c<chooses_insert>);
static_assert(buildable_sequence_c<doubled>);

// Nothing is detected: a type with push_back or insert, but no
// specialisation, can't be built.
static_assert(not buildable_sequence_c<push_back_and_insert>);
static_assert(not buildable_sequence_c<read_only>);
static_assert(not buildable_sequence_c<std::optional<int>>);
static_assert(not buildable_sequence_c<int>);

// ============================================================ appender_c

static_assert(appender_c<push_back_appender<std::vector<int>>, std::vector<int>>);
// finish() is part of the contract.
static_assert(not appender_c<no_finish, std::vector<int>>);
// An appender for a different sequence.
static_assert(not appender_c<push_back_appender<std::vector<int>>, std::deque<int>>);

// ============================================================ building

// The user's appender is used, and writes only in finish().
constexpr bool
    uses_user_appender()
{
    doubled                   container;
    default_appender<doubled> appender(container);
    appender(2);
    appender(3);
    const bool unchanged = container.values.empty();
    appender.finish();
    return unchanged && container.values == std::vector<int>{4, 6};
}

static_assert(uses_user_appender());

TEST_CASE("A type with push_back and insert builds as its author chose", "[appenders]")
{
    chooses_insert                   container;
    default_appender<chooses_insert> appender(container);
    CHECK(appender(1) == append_result::added);
    appender.finish();
    CHECK(container.values == std::vector<int>{1});
}
} // namespace appenders_tests
UAB_NS_END

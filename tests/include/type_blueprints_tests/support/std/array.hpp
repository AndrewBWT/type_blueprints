#pragma once
/*!
 * @file
 * @brief Tests for support/std/array.hpp: std::array's fixed count, its
 * appender (fixed_size_appender), and placeholders for arrays of elements
 * with no default constructor.
 *
 * All static_asserts: the file only compiles if they hold.
 */
#include "type_blueprints/support/std/array.hpp"

#include <array>
#include <type_traits>

// ============================================================ test types

namespace array_test_types
{
// No default constructor, but the user gives it a placeholder.
struct no_default
{
    int value;

    constexpr explicit no_default(
        int value_arg
    ) noexcept
        : value(value_arg)
    {}
};

// No default constructor, and no placeholder.
struct unmakeable
{
    constexpr explicit unmakeable(int) noexcept {}
};
} // namespace array_test_types

UAB_NS_BEGIN

template <>
struct placeholder_value<array_test_types::no_default>
{
    static constexpr array_test_types::no_default
        value() noexcept
    {
        return array_test_types::no_default(7);
    }
};

namespace support_std_array_tests
{
using namespace ::array_test_types;

// ============================================================ fixed count

static_assert(fixed_count_sequence_c<std::array<int, 4>>);
static_assert(sequence_fixed_count_v<std::array<int, 4>> == 4);
static_assert(sequence_fixed_count_v<std::array<int, 0>> == 0);

// ============================================================ appender

static_assert(std::is_base_of_v<
              fixed_size_appender<std::array<int, 3>>,
              default_appender<std::array<int, 3>>>);
static_assert(buildable_sequence_c<std::array<int, 3>>);
static_assert(buildable_sequence_c<std::array<int, 0>>);

// Reports full past its length, and is complete only once every element is
// filled.
constexpr bool
    fills_array()
{
    std::array<int, 3>                   container{};
    default_appender<std::array<int, 3>> appender(container);
    if (appender(1) != append_result::added || appender.complete())
    {
        return false;
    }
    appender(2);
    if (appender.complete())
    {
        return false;
    }
    appender(3);
    const bool full = appender(4) == append_result::full;
    appender.finish();
    return full && appender.complete() && container == std::array<int, 3>{1, 2, 3};
}

static_assert(fills_array());

// A zero-length array is complete from the start, and full.
constexpr bool
    zero_length_is_complete()
{
    std::array<int, 0>                   container{};
    default_appender<std::array<int, 0>> appender(container);
    return appender.complete() && appender(1) == append_result::full;
}

static_assert(zero_length_is_complete());

// ============================================================ placeholders

static_assert(has_placeholder_value_c<std::array<no_default, 2>>);
// Default-constructible elements need none: std::array<int, 2>{} works.
static_assert(not has_placeholder_value_c<std::array<int, 2>>);
static_assert(not has_placeholder_value_c<std::array<std::array<int, 4>, 4>>);
// Elements with neither get none.
static_assert(not has_placeholder_value_c<std::array<unmakeable, 2>>);
static_assert(not placeholder_constructible_c<std::array<unmakeable, 2>>);
// Built-in arrays as elements can't be initialised from a placeholder.
static_assert(not placeholder_constructible_c<std::array<no_default[2], 2>>);

constexpr bool
    makes_array_of_placeholders()
{
    const auto values = make_placeholder<std::array<no_default, 3>>();
    return values[0].value == 7 && values[1].value == 7 && values[2].value == 7;
}

static_assert(makes_array_of_placeholders());

// Nested std::arrays: each level uses the placeholder of the level inside.
static_assert(make_placeholder<std::array<std::array<int, 4>, 4>>()[3][3] == 0);
static_assert(
    make_placeholder<std::array<std::array<no_default, 2>, 2>>()[1][1].value == 7
);
static_assert(make_placeholder<std::array<std::array<std::array<no_default, 2>, 2>, 2>>()
                  [1][1][1]
                      .value
              == 7);

// Building into a placeholder.
constexpr bool
    builds_array_of_no_default()
{
    auto result = make_placeholder<std::array<no_default, 2>>();
    default_appender<std::array<no_default, 2>> appender(result);
    appender(no_default(1));
    appender(no_default(2));
    appender.finish();
    return appender.complete() && result[0].value == 1 && result[1].value == 2;
}

static_assert(builds_array_of_no_default());
} // namespace support_std_array_tests
UAB_NS_END

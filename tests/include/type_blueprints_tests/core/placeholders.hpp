#pragma once
/*!
 * @file
 * @brief Tests for placeholder_value, has_placeholder_value_c,
 * placeholder_constructible_c, make_placeholder and placeholder_object, with
 * user types. The std placeholders are tested with their support headers.
 *
 * All static_asserts: the file only compiles if they hold.
 */
#include "type_blueprints/core/appenders/fixed_size.hpp"
#include "type_blueprints/core/placeholders.hpp"

#include <cstddef>
#include <type_traits>
#include <utility>

// ============================================================ test types

namespace placeholder_test_types
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

// Default-constructible, but its placeholder is used instead.
struct both
{
    int value = 1;
};
} // namespace placeholder_test_types

UAB_NS_BEGIN

template <>
struct placeholder_value<placeholder_test_types::no_default>
{
    static constexpr placeholder_test_types::no_default
        value() noexcept
    {
        return placeholder_test_types::no_default(7);
    }
};

template <>
struct placeholder_value<placeholder_test_types::both>
{
    static constexpr placeholder_test_types::both
        value() noexcept
    {
        return placeholder_test_types::both{42};
    }
};

namespace placeholders_tests
{
using namespace ::placeholder_test_types;

// ============================================================ concepts

static_assert(has_placeholder_value_c<no_default>);
static_assert(has_placeholder_value_c<both>);
static_assert(not has_placeholder_value_c<int>);
static_assert(not has_placeholder_value_c<unmakeable>);

static_assert(placeholder_constructible_c<int>);
static_assert(placeholder_constructible_c<int[3]>);
static_assert(placeholder_constructible_c<no_default>);
static_assert(placeholder_constructible_c<no_default[2][3]>);
static_assert(not placeholder_constructible_c<unmakeable>);
static_assert(not placeholder_constructible_c<unmakeable[2]>);
static_assert(not placeholder_constructible_c<int[]>);

// ============================================================
// make_placeholder

static_assert(make_placeholder<int>() == 0);
static_assert(make_placeholder<no_default>().value == 7);
// The placeholder wins over the default constructor.
static_assert(make_placeholder<both>().value == 42);

// ============================================================
// placeholder_object

static_assert(placeholder_object<no_default>().value.value == 7);

// An array of elements with no placeholder is value-initialised.
constexpr bool
    value_initialises_array()
{
    const placeholder_object<int[2][2]> object;
    return object.value[0][0] == 0 && object.value[1][1] == 0;
}

static_assert(value_initialises_array());

// An array of elements with a placeholder makes each one from it, nested
// arrays included.
constexpr bool
    fills_nested_array_with_placeholders()
{
    const placeholder_object<no_default[2][3]> object;
    // This version fails in MSVC. It shouldn't.
    /*for (const auto& row : object.value)
    {
        for (const no_default& element : row)
        {
            if (element.value != 7)
            {
                return false;
            }
        }
    }*/
    for (std::size_t row_idx = 0; row_idx < 2; ++row_idx)
    {
        for (std::size_t column_idx = 0; column_idx < 3; ++column_idx)
        {
            if (object.value[row_idx][column_idx].value != 7)
            {
                return false;
            }
        }
    }
    return true;
}

static_assert(fills_nested_array_with_placeholders());

// Even default-constructible elements are made from their placeholder.
constexpr bool
    prefers_placeholder_in_array()
{
    const placeholder_object<both[2]> object;
    return object.value[0].value == 42 && object.value[1].value == 42;
}

static_assert(prefers_placeholder_in_array());

// ============================================================ building into
// a placeholder: what this is for

// A built-in array of elements with no default constructor.
constexpr bool
    builds_built_in_array_of_no_default()
{
    placeholder_object<no_default[2]>   object;
    fixed_size_appender<no_default[2]> appender(object.value);
    appender(no_default(1));
    appender(no_default(2));
    appender.finish();
    return appender.complete() && object.value[0].value == 1
           && object.value[1].value == 2;
}

static_assert(builds_built_in_array_of_no_default());
} // namespace placeholders_tests
UAB_NS_END

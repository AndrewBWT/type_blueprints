#pragma once
/*!
 * @file
 * @brief Tests for support/std/compare.hpp: placeholders for the comparison
 * categories, which have no default constructor.
 *
 * All static_asserts: the file only compiles if they hold.
 */
#include "unpack_and_build/core/appenders/fixed_size.hpp"
#include "unpack_and_build/support/std/compare.hpp"

#include <compare>
#include <type_traits>

UAB_NS_BEGIN
namespace support_std_compare_tests
{
static_assert(not std::is_default_constructible_v<std::strong_ordering>);

static_assert(has_placeholder_value_c<std::strong_ordering>);
static_assert(has_placeholder_value_c<std::weak_ordering>);
static_assert(has_placeholder_value_c<std::partial_ordering>);

static_assert(make_placeholder<std::strong_ordering>() == std::strong_ordering::equal);
static_assert(make_placeholder<std::weak_ordering>() == std::weak_ordering::equivalent);
static_assert(make_placeholder<std::partial_ordering>()
              == std::partial_ordering::equivalent);

// Built-in arrays of them, nested too.
static_assert(placeholder_constructible_c<std::strong_ordering[2][3]>);

constexpr bool
    fills_nested_array()
{
    const placeholder_object<std::weak_ordering[2][2]> object;
    return object.value[0][0] == std::weak_ordering::equivalent
           && object.value[1][1] == std::weak_ordering::equivalent;
}

static_assert(fills_nested_array());

// Building into a placeholder.
constexpr bool
    builds_built_in_array_of_orderings()
{
    placeholder_object<std::strong_ordering[2]>  object;
    fixed_size_appender<std::strong_ordering[2]> appender(object.value);
    appender(std::strong_ordering(std::strong_ordering::less));
    appender(std::strong_ordering(std::strong_ordering::greater));
    appender.finish();
    return appender.complete() && object.value[0] == std::strong_ordering::less
           && object.value[1] == std::strong_ordering::greater;
}

static_assert(builds_built_in_array_of_orderings());
} // namespace support_std_compare_tests
UAB_NS_END

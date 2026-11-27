#pragma once
/*!
 * @file
 * @brief Tests for support/built_in_array.hpp: built-in arrays are built with
 * fixed_size_appender.
 *
 * All static_asserts: the file only compiles if they hold.
 */
#include "type_blueprints/support/built_in_array.hpp"

#include <type_traits>
#include <utility>

TYPE_BLUEPRINTS_NS_BEGIN
namespace support_built_in_array_tests
{
static_assert(std::is_base_of_v<fixed_size_appender<int[3]>, default_appender<int[3]>>);
static_assert(std::is_base_of_v<
              fixed_size_appender<int[2][2]>,
              default_appender<int[2][2]>>);
static_assert(buildable_sequence_c<int[3]>);
static_assert(buildable_sequence_c<int[2][2]>);
// An array of unknown bound has no length to fill.
static_assert(not buildable_sequence_c<int[]>);

constexpr bool
    builds()
{
    int                      container[2] = {};
    default_appender<int[2]> appender(container);
    appender(1);
    appender(2);
    const bool full = appender(3) == append_result::full;
    appender.finish();
    return full && appender.complete() && container[0] == 1 && container[1] == 2;
}

static_assert(builds());
} // namespace support_built_in_array_tests
TYPE_BLUEPRINTS_NS_END

#pragma once
/*!
 * @file
 * @brief Tests for fixed_size_appender, on built-in arrays (whose fixed
 * count is detected). std::array is tested with its support header.
 *
 * All static_asserts: the file only compiles if they hold.
 */
#include "type_blueprints/core/appenders.hpp"
#include "type_blueprints/core/appenders/fixed_size.hpp"

#include <utility>

TYPE_BLUEPRINTS_NS_BEGIN
namespace fixed_size_tests
{
static_assert(appender_c<fixed_size_appender<int[3]>, int[3]>);

// Reports full past its length, and is complete only once every element is
// filled.
constexpr bool
    fills_array()
{
    int                         container[3] = {};
    fixed_size_appender<int[3]> appender(container);
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
    return full && appender.complete() && container[0] == 1
           && container[1] == 2 && container[2] == 3;
}

static_assert(fills_array());

// Elements of a built-in array of arrays are assigned element by element.
constexpr bool
    fills_nested_array()
{
    int                            container[2][2] = {};
    fixed_size_appender<int[2][2]> appender(container);
    int                            first[2]  = {1, 2};
    int                            second[2] = {3, 4};
    appender(std::move(first));
    appender(std::move(second));
    appender.finish();
    return appender.complete() && container[0][0] == 1 && container[0][1] == 2
           && container[1][0] == 3 && container[1][1] == 4;
}

static_assert(fills_nested_array());
} // namespace fixed_size_tests
TYPE_BLUEPRINTS_NS_END

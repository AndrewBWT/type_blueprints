#pragma once
/*!
 * @file
 * @brief Tests for support/std/set.hpp: std::set is built with
 * unique_insert_appender.
 *
 * std::set isn't constexpr in C++23, so building is tested at runtime.
 */
#include "type_blueprints/support/std/set.hpp"

#include <catch2/catch_test_macros.hpp>

#include <set>
#include <type_traits>

TYPE_BLUEPRINTS_NS_BEGIN
namespace support_std_set_tests
{
using container_t = std::set<int>;

static_assert(std::is_base_of_v<
              unique_insert_appender<container_t>,
              default_appender<container_t>>);
static_assert(buildable_sequence_c<container_t>);

TEST_CASE("default_appender builds a std::set", "[support][set]")
{
    container_t                   container;
    default_appender<container_t> appender(container);
    appender(2);
    appender(1);
    appender.finish();
    CHECK(appender.complete());
    CHECK(container == std::set<int>{1, 2});
}
} // namespace support_std_set_tests
TYPE_BLUEPRINTS_NS_END

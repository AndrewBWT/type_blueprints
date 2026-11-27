#pragma once
/*!
 * @file
 * @brief Tests for support/std/list.hpp: std::list is built with
 * push_back_appender.
 *
 * std::list isn't constexpr in C++23, so building is tested at runtime.
 */
#include "type_blueprints/support/std/list.hpp"

#include <catch2/catch_test_macros.hpp>

#include <list>
#include <type_traits>

UAB_NS_BEGIN
namespace support_std_list_tests
{
using container_t = std::list<int>;

static_assert(std::is_base_of_v<
              push_back_appender<container_t>,
              default_appender<container_t>>);
static_assert(buildable_sequence_c<container_t>);

TEST_CASE("default_appender builds a std::list", "[support][list]")
{
    container_t                   container;
    default_appender<container_t> appender(container);
    appender(1);
    appender(2);
    appender.finish();
    CHECK(appender.complete());
    CHECK(container == std::list<int>{1, 2});
}
} // namespace support_std_list_tests
UAB_NS_END

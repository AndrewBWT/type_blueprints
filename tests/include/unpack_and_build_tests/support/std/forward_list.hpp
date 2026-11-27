#pragma once
/*!
 * @file
 * @brief Tests for support/std/forward_list.hpp: std::forward_list is built
 * with insert_after_appender.
 *
 * std::forward_list isn't constexpr in C++23, so building is tested at
 * runtime.
 */
#include "unpack_and_build/support/std/forward_list.hpp"

#include <catch2/catch_test_macros.hpp>

#include <forward_list>
#include <type_traits>

UAB_NS_BEGIN
namespace support_std_forward_list_tests
{
using container_t = std::forward_list<int>;

static_assert(std::is_base_of_v<
              insert_after_appender<container_t>,
              default_appender<container_t>>);
static_assert(buildable_sequence_c<container_t>);

TEST_CASE("default_appender builds a std::forward_list", "[support][forward_list]")
{
    container_t                   container;
    default_appender<container_t> appender(container);
    appender(1);
    appender(2);
    appender.finish();
    CHECK(appender.complete());
    CHECK(container == std::forward_list<int>{1, 2});
}
} // namespace support_std_forward_list_tests
UAB_NS_END

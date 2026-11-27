#pragma once
/*!
 * @file
 * @brief Tests for support/std/map.hpp: std::map is built with
 * unique_insert_appender.
 *
 * std::map isn't constexpr in C++23, so building is tested at runtime.
 */
#include "unpack_and_build/support/std/map.hpp"

#include <catch2/catch_test_macros.hpp>

#include <map>
#include <type_traits>
#include <utility>

UAB_NS_BEGIN
namespace support_std_map_tests
{
using container_t = std::map<int, char>;

static_assert(std::is_base_of_v<
              unique_insert_appender<container_t>,
              default_appender<container_t>>);
static_assert(buildable_sequence_c<container_t>);
// Built from std::pair<int, char>, not its value_type, std::pair<const int,
// char>.
static_assert(std::is_same_v<
              sequence_build_value_t<container_t>,
              std::pair<int, char>>);

TEST_CASE("default_appender builds a std::map", "[support][map]")
{
    container_t                   container;
    default_appender<container_t> appender(container);
    appender({1, 'a'});
    appender({2, 'b'});
    appender.finish();
    CHECK(appender.complete());
    CHECK(container == std::map<int, char>{{1, 'a'}, {2, 'b'}});
}
} // namespace support_std_map_tests
UAB_NS_END

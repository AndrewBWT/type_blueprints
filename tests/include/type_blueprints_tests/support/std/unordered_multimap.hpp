#pragma once
/*!
 * @file
 * @brief Tests for support/std/unordered_multimap.hpp: std::unordered_multimap
 * is built with multi_insert_appender.
 *
 * std::unordered_multimap isn't constexpr in C++23, so building is tested at
 * runtime.
 */
#include "type_blueprints/support/std/unordered_multimap.hpp"

#include <catch2/catch_test_macros.hpp>

#include <type_traits>
#include <unordered_map>
#include <utility>

TYPE_BLUEPRINTS_NS_BEGIN
namespace support_std_unordered_multimap_tests
{
using container_t = std::unordered_multimap<int, char>;

static_assert(std::is_base_of_v<
              multi_insert_appender<container_t>,
              default_appender<container_t>>);
static_assert(buildable_sequence_c<container_t>);
// Built from std::pair<int, char>, not its value_type, std::pair<const int,
// char>.
static_assert(std::is_same_v<
              sequence_build_value_t<container_t>,
              std::pair<int, char>>);

TEST_CASE("default_appender builds a std::unordered_multimap", "[support][unordered_multimap]")
{
    container_t                   container;
    default_appender<container_t> appender(container);
    appender({1, 'a'});
    appender({1, 'b'});
    appender.finish();
    CHECK(appender.complete());
    CHECK(container == std::unordered_multimap<int, char>{{1, 'a'}, {1, 'b'}});
}
} // namespace support_std_unordered_multimap_tests
TYPE_BLUEPRINTS_NS_END

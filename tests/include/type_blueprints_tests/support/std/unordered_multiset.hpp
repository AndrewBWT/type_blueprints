#pragma once
/*!
 * @file
 * @brief Tests for support/std/unordered_multiset.hpp: std::unordered_multiset
 * is built with multi_insert_appender.
 *
 * std::unordered_multiset isn't constexpr in C++23, so building is tested at
 * runtime.
 */
#include "type_blueprints/support/std/unordered_multiset.hpp"

#include <catch2/catch_test_macros.hpp>

#include <type_traits>
#include <unordered_set>

UAB_NS_BEGIN
namespace support_std_unordered_multiset_tests
{
using container_t = std::unordered_multiset<int>;

static_assert(std::is_base_of_v<
              multi_insert_appender<container_t>,
              default_appender<container_t>>);
static_assert(buildable_sequence_c<container_t>);

TEST_CASE("default_appender builds a std::unordered_multiset", "[support][unordered_multiset]")
{
    container_t                   container;
    default_appender<container_t> appender(container);
    appender(1);
    appender(1);
    appender.finish();
    CHECK(appender.complete());
    CHECK(container == std::unordered_multiset<int>{1, 1});
}
} // namespace support_std_unordered_multiset_tests
UAB_NS_END

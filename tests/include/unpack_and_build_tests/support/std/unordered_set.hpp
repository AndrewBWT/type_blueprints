#pragma once
/*!
 * @file
 * @brief Tests for support/std/unordered_set.hpp: std::unordered_set is built
 * with unique_insert_appender.
 *
 * std::unordered_set isn't constexpr in C++23, so building is tested at
 * runtime.
 */
#include "unpack_and_build/support/std/unordered_set.hpp"

#include <catch2/catch_test_macros.hpp>

#include <type_traits>
#include <unordered_set>

UAB_NS_BEGIN
namespace support_std_unordered_set_tests
{
using container_t = std::unordered_set<int>;

static_assert(std::is_base_of_v<
              unique_insert_appender<container_t>,
              default_appender<container_t>>);
static_assert(buildable_sequence_c<container_t>);

TEST_CASE("default_appender builds a std::unordered_set", "[support][unordered_set]")
{
    container_t                   container;
    default_appender<container_t> appender(container);
    appender(2);
    appender(1);
    appender.finish();
    CHECK(appender.complete());
    CHECK(container == std::unordered_set<int>{1, 2});
}
} // namespace support_std_unordered_set_tests
UAB_NS_END

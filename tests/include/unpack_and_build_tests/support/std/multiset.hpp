#pragma once
/*!
 * @file
 * @brief Tests for support/std/multiset.hpp: std::multiset is built with
 * multi_insert_appender.
 *
 * std::multiset isn't constexpr in C++23, so building is tested at runtime.
 */
#include "unpack_and_build/support/std/multiset.hpp"

#include <catch2/catch_test_macros.hpp>

#include <set>
#include <type_traits>

UAB_NS_BEGIN
namespace support_std_multiset_tests
{
using container_t = std::multiset<int>;

static_assert(std::is_base_of_v<
              multi_insert_appender<container_t>,
              default_appender<container_t>>);
static_assert(buildable_sequence_c<container_t>);

TEST_CASE("default_appender builds a std::multiset", "[support][multiset]")
{
    container_t                   container;
    default_appender<container_t> appender(container);
    appender(1);
    appender(1);
    appender.finish();
    CHECK(appender.complete());
    CHECK(container == std::multiset<int>{1, 1});
}
} // namespace support_std_multiset_tests
UAB_NS_END

#pragma once
/*!
 * @file
 * @brief Tests for support/std/valarray.hpp: std::valarray is built with
 * valarray_appender, which only builds it in finish().
 *
 * std::valarray isn't constexpr, so building is tested at runtime.
 */
#include "unpack_and_build/support/std/valarray.hpp"

#include <catch2/catch_test_macros.hpp>

#include <type_traits>
#include <valarray>

UAB_NS_BEGIN
namespace support_std_valarray_tests
{
static_assert(std::is_base_of_v<
              valarray_appender<int>,
              default_appender<std::valarray<int>>>);
static_assert(buildable_sequence_c<std::valarray<int>>);

TEST_CASE("std::valarray is only built in finish()", "[support][valarray]")
{
    std::valarray<int>                   values;
    default_appender<std::valarray<int>> appender(values);
    appender(1);
    appender(2);
    CHECK(values.size() == 0);
    appender.finish();
    REQUIRE(values.size() == 2);
    CHECK(values[0] == 1);
    CHECK(values[1] == 2);

    // finish() can be called again after more elements.
    appender(3);
    appender.finish();
    REQUIRE(values.size() == 3);
    CHECK(values[2] == 3);
}

TEST_CASE("std::valarray keeps elements already there, in front", "[support][valarray]")
{
    std::valarray<int>                   existing{7, 8};
    default_appender<std::valarray<int>> appender(existing);
    appender(9);
    appender.finish();
    REQUIRE(existing.size() == 3);
    CHECK(existing[0] == 7);
    CHECK(existing[2] == 9);
}
} // namespace support_std_valarray_tests
UAB_NS_END

#pragma once
/*!
 * @file
 * @brief Tests for support/std/bitset.hpp: std::bitset as a sequence of bools,
 * most significant bit first, built with bitset_appender. Its sequence traits
 * are also checked in the sequence_traits tests.
 *
 * All static_asserts: the file only compiles if they hold.
 */
#include "type_blueprints/support/std/bitset.hpp"

#include <bitset>
#include <cstddef>
#include <type_traits>
#include <utility>

UAB_NS_BEGIN
namespace support_std_bitset_tests
{
static_assert(std::is_same_v<sequence_value_t<std::bitset<4>>, bool>);
static_assert(sequence_fixed_count_v<std::bitset<8>> == 8);
static_assert(sequence_c<std::bitset<4>>);

static_assert(std::is_base_of_v<bitset_appender<4>, default_appender<std::bitset<4>>>);
static_assert(buildable_sequence_c<std::bitset<4>>);

// Bits are read most significant first, the order to_string() writes them.
constexpr bool
    reads_most_significant_first()
{
    const std::bitset<4> object{0b0011};
    bool                 expected[4] = {false, false, true, true};
    std::size_t          index       = 0;
    for (const bool bit : sequence_elements<std::bitset<4>>::view(object))
    {
        if (index == 4 || bit != expected[index])
        {
            return false;
        }
        ++index;
    }
    return index == 4;
}

static_assert(reads_most_significant_first());

// Bits are given most significant first, and land in to_string() order.
constexpr bool
    fills_bitset()
{
    std::bitset<4>                   bits;
    default_appender<std::bitset<4>> appender(bits);
    for (bool bit : {true, false, true, true})
    {
        appender(std::move(bit));
    }
    appender.finish();
    return appender.complete() && bits == std::bitset<4>(0b1011)
           && appender(true) == append_result::full;
}

static_assert(fills_bitset());

// Fewer than N bits fill the low bits, and the bitset isn't complete.
constexpr bool
    short_bitset_is_incomplete()
{
    std::bitset<4>                   bits;
    default_appender<std::bitset<4>> appender(bits);
    appender(true);
    appender(false);
    appender.finish();
    return not appender.complete() && bits == std::bitset<4>(0b10);
}

static_assert(short_bitset_is_incomplete());
} // namespace support_std_bitset_tests
UAB_NS_END

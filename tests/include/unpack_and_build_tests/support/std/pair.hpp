#pragma once
/*!
 * @file
 * @brief Tests for support/std/pair.hpp: a placeholder for a std::pair with an
 * element that has no default constructor.
 *
 * All static_asserts: the file only compiles if they hold.
 */
#include "unpack_and_build/support/std/compare.hpp"
#include "unpack_and_build/support/std/pair.hpp"

#include <compare>
#include <utility>

// ============================================================ test types

namespace pair_test_types
{
// No default constructor, and no placeholder.
struct unmakeable
{
    constexpr explicit unmakeable(int) noexcept {}
};
} // namespace pair_test_types

UAB_NS_BEGIN
namespace support_std_pair_tests
{
using namespace ::pair_test_types;

constexpr bool
    makes_pair()
{
    const auto result = make_placeholder<std::pair<int, std::strong_ordering>>();
    return result.first == 0 && result.second == std::strong_ordering::equal;
}

static_assert(makes_pair());
// Default-constructible: no placeholder needed.
static_assert(not has_placeholder_value_c<std::pair<int, int>>);
static_assert(not placeholder_constructible_c<std::pair<int, unmakeable>>);
} // namespace support_std_pair_tests
UAB_NS_END

#pragma once
/*!
 * @file
 * @brief Tests for support/std/expected.hpp: a placeholder for a std::expected
 * whose value type has no default constructor.
 *
 * All static_asserts: the file only compiles if they hold.
 */
#include "type_blueprints/support/std/compare.hpp"
#include "type_blueprints/support/std/expected.hpp"

#include <compare>
#include <expected>

// ============================================================ test types

namespace expected_test_types
{
// No default constructor, and no placeholder.
struct unmakeable
{
    constexpr explicit unmakeable(int) noexcept {}
};
} // namespace expected_test_types

TYPE_BLUEPRINTS_NS_BEGIN
namespace support_std_expected_tests
{
using namespace ::expected_test_types;

// Holds a value, made from the value type's placeholder.
constexpr bool
    makes_expected_with_value()
{
    const auto result = make_placeholder<std::expected<std::strong_ordering, int>>();
    return result.has_value() && *result == std::strong_ordering::equal;
}

static_assert(makes_expected_with_value());
// Default-constructible: no placeholder needed.
static_assert(not has_placeholder_value_c<std::expected<int, std::strong_ordering>>);
static_assert(not placeholder_constructible_c<std::expected<unmakeable, int>>);
} // namespace support_std_expected_tests
TYPE_BLUEPRINTS_NS_END

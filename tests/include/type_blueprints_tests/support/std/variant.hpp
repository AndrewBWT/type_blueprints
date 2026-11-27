#pragma once
/*!
 * @file
 * @brief Tests for support/std/variant.hpp: a placeholder for a std::variant
 * whose first alternative has no default constructor.
 *
 * All static_asserts: the file only compiles if they hold.
 */
#include "type_blueprints/support/std/compare.hpp"
#include "type_blueprints/support/std/variant.hpp"

#include <compare>
#include <variant>

// ============================================================ test types

namespace variant_test_types
{
// No default constructor, and no placeholder.
struct unmakeable
{
    constexpr explicit unmakeable(int) noexcept {}
};
} // namespace variant_test_types

UAB_NS_BEGIN
namespace support_std_variant_tests
{
using namespace ::variant_test_types;

// Holds the first alternative's placeholder.
constexpr bool
    makes_variant_from_first_alternative()
{
    const auto result = make_placeholder<std::variant<std::strong_ordering, int>>();
    return result.index() == 0
           && std::get<0>(result) == std::strong_ordering::equal;
}

static_assert(makes_variant_from_first_alternative());
// Default-constructible: no placeholder needed.
static_assert(not has_placeholder_value_c<std::variant<int, std::strong_ordering>>);
// Only the first alternative is tried.
static_assert(not placeholder_constructible_c<std::variant<unmakeable, int>>);
} // namespace support_std_variant_tests
UAB_NS_END

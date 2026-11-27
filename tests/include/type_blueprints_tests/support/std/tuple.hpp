#pragma once
/*!
 * @file
 * @brief Tests for support/std/tuple.hpp: a placeholder for a std::tuple with
 * an element that has no default constructor.
 *
 * All static_asserts: the file only compiles if they hold.
 */
#include "type_blueprints/support/std/array.hpp"
#include "type_blueprints/support/std/compare.hpp"
#include "type_blueprints/support/std/pair.hpp"
#include "type_blueprints/support/std/tuple.hpp"

#include <array>
#include <compare>
#include <tuple>
#include <utility>

// ============================================================ test types

namespace tuple_test_types
{
// No default constructor, and no placeholder.
struct unmakeable
{
    constexpr explicit unmakeable(int) noexcept {}
};
} // namespace tuple_test_types

TYPE_BLUEPRINTS_NS_BEGIN
namespace support_std_tuple_tests
{
using namespace ::tuple_test_types;

constexpr bool
    makes_tuple()
{
    const auto result = make_placeholder<std::tuple<int, std::strong_ordering>>();
    return std::get<0>(result) == 0
           && std::get<1>(result) == std::strong_ordering::equal;
}

static_assert(makes_tuple());
// Default-constructible: no placeholder needed.
static_assert(not has_placeholder_value_c<std::tuple<>>);
static_assert(not has_placeholder_value_c<std::tuple<int, int>>);
static_assert(not placeholder_constructible_c<std::tuple<int, unmakeable>>);

// Nested: each wrapper uses the placeholders of what it holds.
constexpr bool
    makes_nested()
{
    const auto result = make_placeholder<std::pair<
        std::tuple<std::strong_ordering>,
        std::array<std::weak_ordering, 2>>>();
    return std::get<0>(result.first) == std::strong_ordering::equal
           && result.second[1] == std::weak_ordering::equivalent;
}

static_assert(makes_nested());
} // namespace support_std_tuple_tests
TYPE_BLUEPRINTS_NS_END

#pragma once
/*!
 * @file
 * @brief Tests for support/std/unexpected.hpp: a placeholder for
 * std::unexpected, which never has a default constructor.
 *
 * All static_asserts: the file only compiles if they hold.
 */
#include "type_blueprints/support/std/compare.hpp"
#include "type_blueprints/support/std/unexpected.hpp"

#include <compare>
#include <expected>

// ============================================================ test types

namespace unexpected_test_types
{
// No default constructor, and no placeholder.
struct unmakeable
{
    constexpr explicit unmakeable(int) noexcept {}
};
} // namespace unexpected_test_types

UAB_NS_BEGIN
namespace support_std_unexpected_tests
{
using namespace ::unexpected_test_types;

// Holds its error's placeholder.
static_assert(make_placeholder<std::unexpected<int>>().error() == 0);
static_assert(make_placeholder<std::unexpected<std::strong_ordering>>().error()
              == std::strong_ordering::equal);
static_assert(not placeholder_constructible_c<std::unexpected<unmakeable>>);
} // namespace support_std_unexpected_tests
UAB_NS_END

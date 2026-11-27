#pragma once
/*!
 * @file
 * @brief Tests for build_from and buildable_from_c. Static_asserts where the
 * container is constexpr; Catch2 for the rest (the node-based containers
 * aren't constexpr until C++26, and std::valarray never is).
 */
// The specialisations for every type used below.
#include "type_blueprints/support/built_in_array.hpp"
#include "type_blueprints/support/std/array.hpp"
#include "type_blueprints/support/std/bitset.hpp"
#include "type_blueprints/support/std/forward_list.hpp"
#include "type_blueprints/support/std/map.hpp"
#include "type_blueprints/support/std/multiset.hpp"
#include "type_blueprints/support/std/set.hpp"
#include "type_blueprints/support/std/string.hpp"
#include "type_blueprints/support/std/valarray.hpp"
#include "type_blueprints/support/std/vector.hpp"

#include "type_blueprints/core/build_from.hpp"

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <bitset>
#include <compare>
#include <forward_list>
#include <initializer_list>
#include <map>
#include <optional>
#include <set>
#include <stack>
#include <string>
#include <string_view>
#include <utility>
#include <valarray>
#include <vector>

// ============================================================ test types

namespace build_from_test_types
{
// No default constructor; its placeholder is used to start a std::array.
struct no_default
{
    int value;

    constexpr explicit no_default(
        int value_arg
    ) noexcept
        : value(value_arg)
    {}

    constexpr bool
        operator==(const no_default&) const
        = default;
};
} // namespace build_from_test_types

TYPE_BLUEPRINTS_NS_BEGIN

template <>
struct placeholder_value<build_from_test_types::no_default>
{
    static constexpr build_from_test_types::no_default
        value() noexcept
    {
        return build_from_test_types::no_default(0);
    }
};

namespace build_from_tests
{
using namespace ::build_from_test_types;

// ============================================================
// buildable_from_c

static_assert(buildable_from_c<std::vector<int>, std::vector<int>>);
static_assert(buildable_from_c<std::vector<long>, int[3]>);
static_assert(buildable_from_c<std::set<int>, std::initializer_list<int>>);
static_assert(buildable_from_c<std::map<int, char>, std::vector<std::pair<int, char>>>);
// A map's own elements, std::pair<const K, V>, become its build type.
static_assert(buildable_from_c<std::map<int, char>, std::map<int, char>>);
static_assert(buildable_from_c<std::array<no_default, 2>, std::vector<no_default>>);
static_assert(buildable_from_c<std::vector<bool>, std::bitset<4>>);

// A built-in array can't be returned.
static_assert(not buildable_from_c<int[3], std::vector<int>>);
// Not buildable.
static_assert(not buildable_from_c<std::stack<int>, std::vector<int>>);
// Not a sequence.
static_assert(not buildable_from_c<std::vector<int>, int>);
// Elements that don't convert.
static_assert(not buildable_from_c<std::vector<int>, std::vector<std::u8string>>);

// ============================================================ building

constexpr bool
    builds_vector()
{
    return build_from<std::vector<int>>(std::vector<int>{1, 2, 3})
           == std::vector<int>{1, 2, 3};
}

static_assert(builds_vector());

// Elements are converted to the build type.
constexpr bool
    converts_elements()
{
    constexpr int source[3] = {4, 5, 6};
    return build_from<std::vector<long>>(source) == std::vector<long>{4, 5, 6};
}

static_assert(converts_elements());

constexpr bool
    builds_string()
{
    return build_from<std::u8string>(std::u8string_view(u8"hi")) == u8"hi";
}

static_assert(builds_string());

// A fixed-length sequence needs exactly its length.
constexpr bool
    builds_array_of_exact_length()
{
    return build_from<std::array<int, 3>>(std::vector<int>{1, 2, 3})
           == std::array<int, 3>{1, 2, 3};
}

static_assert(builds_array_of_exact_length());

constexpr bool
    rejects_too_few_for_array()
{
    return not build_from<std::array<int, 3>>(std::vector<int>{1, 2})
                   .has_value();
}

static_assert(rejects_too_few_for_array());

constexpr bool
    rejects_too_many_for_array()
{
    return not build_from<std::array<int, 3>>(std::vector<int>{1, 2, 3, 4})
                   .has_value();
}

static_assert(rejects_too_many_for_array());

// Elements with no default constructor: the array starts from placeholders.
constexpr bool
    builds_array_of_no_default()
{
    const std::vector<no_default> source{no_default(1), no_default(2)};
    const std::optional<std::array<no_default, 2>> result
        = build_from<std::array<no_default, 2>>(source);
    return result.has_value() && (*result)[0].value == 1
           && (*result)[1].value == 2;
}

static_assert(builds_array_of_no_default());

// A std::bitset, as source and as result, most significant bit first.
constexpr bool
    reads_bitset()
{
    return build_from<std::vector<bool>>(std::bitset<4>(0b1011))
           == std::vector<bool>{true, false, true, true};
}

static_assert(reads_bitset());

constexpr bool
    builds_bitset()
{
    return build_from<std::bitset<4>>(std::vector<bool>{true, false, true, true})
           == std::bitset<4>(0b1011);
}

static_assert(builds_bitset());

// Empty source: an empty container, but no fixed-length one.
constexpr bool
    handles_empty()
{
    const std::vector<int> empty;
    return build_from<std::vector<int>>(empty) == std::vector<int>{}
           && not build_from<std::array<int, 1>>(empty).has_value()
           && build_from<std::array<int, 0>>(empty).has_value();
}

static_assert(handles_empty());

// ============================================================ runtime

TEST_CASE("build_from builds associative containers", "[build_from]")
{
    SECTION("Unique keys")
    {
        CHECK(build_from<std::set<int>>(std::initializer_list<int>{3, 1, 2})
              == std::set<int>{1, 2, 3});
    }

    SECTION("A duplicate key isn't added, so nothing is built")
    {
        CHECK(not build_from<std::set<int>>(std::initializer_list<int>{1, 1})
                      .has_value());
    }

    SECTION("A multi container adds a duplicate")
    {
        CHECK(build_from<std::multiset<int>>(std::initializer_list<int>{1, 1})
              == std::multiset<int>{1, 1});
    }

    SECTION("A map, from pairs")
    {
        const std::vector<std::pair<int, char>> source{{1, 'a'}, {2, 'b'}};
        CHECK(build_from<std::map<int, char>>(source)
              == std::map<int, char>{{1, 'a'}, {2, 'b'}});
    }

    SECTION("A map, rebuilt from its own elements")
    {
        const std::map<int, char> original{{1, 'a'}, {2, 'b'}};
        CHECK(build_from<std::map<int, char>>(original) == original);
    }
}

TEST_CASE("build_from keeps order in a std::forward_list", "[build_from]")
{
    CHECK(build_from<std::forward_list<int>>(std::vector<int>{1, 2, 3})
          == std::forward_list<int>{1, 2, 3});
}

TEST_CASE("build_from builds a std::valarray", "[build_from]")
{
    const std::optional<std::valarray<int>> result
        = build_from<std::valarray<int>>(std::vector<int>{1, 2, 3});
    REQUIRE(result.has_value());
    REQUIRE(result->size() == 3);
    CHECK((*result)[0] == 1);
    CHECK((*result)[2] == 3);
}
} // namespace build_from_tests
TYPE_BLUEPRINTS_NS_END

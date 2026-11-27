/*!
 * @file
 * @brief Tests for enum_element, enum_list, enum_list_view, has_enum_list_c
 * and enumerators_of.
 *
 * Compile-time behaviour is checked with static_assert, so this file only
 * compiles if it holds. Runtime behaviour (runtime lists, and the exceptions
 * the constructors throw) is checked with Catch2.
 *
 * An invalid compile-time enum_list is a compile error by design, so it can't
 * be checked here; see the note at the end of the file.
 */
#include "unpack_and_build/core/enum.hpp"

#include <catch2/catch_test_macros.hpp>
 // REQUIRE_THROWS_WITH.
#include <catch2/matchers/catch_matchers.hpp>

#include <array>
#include <cstddef>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

// ============================================================ test types

// Listed with a built-in array.
enum class colour
{
    red,
    green,
    blue
};

// Listed with a std::array.
enum class size
{
    small,
    large
};

// Never listed: used for runtime lists.
enum class plain
{
    a,
    b,
    c
};

// enum_list specialisations that don't fit has_enum_list_c.
enum class wrong_element_type
{
    a
};

enum class wrong_member_name
{
    a
};

// Not an enum.
struct not_an_enum
{
};

UAB_NS_BEGIN

template <>
struct enum_list<colour>
{
    static constexpr enum_element<colour> values[] = {
        {colour::red,   u8"red"  },
        {colour::green, u8"green"},
        {colour::blue,  u8"blue" },
    };
};

template <>
struct enum_list<size>
{
    static constexpr std::array<enum_element<size>, 2> values{
        {{size::small, u8"small"}, {size::large, u8"large"}}
    };
};

template <>
struct enum_list<wrong_element_type>
{
    static constexpr int values[] = { 1, 2 };
};

template <>
struct enum_list<wrong_member_name>
{
    static constexpr enum_element<wrong_member_name> entries[] = {
        {wrong_member_name::a, u8"a"},
    };
};

// The tests sit inside the library's namespace, so they don't depend on what
// UAB_NS_BEGIN names it.
namespace enums_tests
{
    // A type derived from enum_element: a container of these must not be viewed
    // as enum_elements, as its elements are larger.
    struct derived_element : enum_element<plain>
    {
        int extra;
    };

    // ============================================================ enum_element

    static_assert(std::is_same_v<enum_element<colour>::enum_type, colour>);

    // ============================================================
    // has_enum_list_c

    static_assert(has_enum_list_c<colour>);
    static_assert(has_enum_list_c<size>);

    // No enum_list specialisation.
    static_assert(not has_enum_list_c<plain>);
    // values holds the wrong type.
    static_assert(not has_enum_list_c<wrong_element_type>);
    // The list isn't called values.
    static_assert(not has_enum_list_c<wrong_member_name>);
    // Not an enum.
    static_assert(not has_enum_list_c<not_an_enum>);
    static_assert(not has_enum_list_c<int>);

    // ============================================================ enumerators_of

    // Size, from a built-in array and from a std::array.
    static_assert(enumerators_of<colour>().size() == 3);
    static_assert(enumerators_of<size>().size() == 2);

    // Elements in order, through operator[], begin and end.
    static_assert(enumerators_of<colour>()[0].value == colour::red);
    static_assert(enumerators_of<colour>()[2].name == u8"blue");
    static_assert(enumerators_of<colour>().begin()->value == colour::red);
    static_assert(enumerators_of<colour>().end() - enumerators_of<colour>().begin() == 3);

    // find by value and by name.
    static_assert(enumerators_of<colour>().find(colour::green)->name == u8"green");
    static_assert(enumerators_of<size>().find(size::large)->name == u8"large");
    static_assert(enumerators_of<colour>().find(u8"blue")->value == colour::blue);
    static_assert(enumerators_of<size>().find(u8"small")->value == size::small);

    // Misses return nullptr; names are compared exactly.
    static_assert(enumerators_of<colour>().find(u8"purple") == nullptr);
    static_assert(enumerators_of<colour>().find(u8"Red") == nullptr);
    static_assert(enumerators_of<colour>().find(u8"red ") == nullptr);
    static_assert(enumerators_of<colour>().find(static_cast<colour>(42)) == nullptr);

    // find returns a pointer into the list itself.
    static_assert(enumerators_of<colour>().find(colour::blue)
        == &enum_list<colour>::values[2]);

    // The view enumerators_of returns can be kept, and used at runtime.
    constexpr enum_list_view<colour> stored_colours = enumerators_of<colour>();
    static_assert(stored_colours.size() == 3);

    // ============================================================ lists built in
    // constant evaluation, for an enum with no enum_list

    constexpr bool
        builds_from_vector_in_constant_evaluation()
    {
        std::vector<enum_element<plain>> list{
            {plain::a, u8"a"},
            {plain::b, u8"b"},
        };
        const enum_list_view view(list);
        return view.size() == 2 && view.find(u8"b")->value == plain::b
            && view.find(plain::c) == nullptr;
    }

    static_assert(builds_from_vector_in_constant_evaluation());

    constexpr bool
        builds_from_pointer_and_size_in_constant_evaluation()
    {
        const enum_element<plain> list[] = {
            {plain::a, u8"a"},
            {plain::b, u8"b"},
            {plain::c, u8"c"},
        };
        // Only the first two.
        const enum_list_view<plain> view(list, 2);
        return view.size() == 2 && view.find(plain::c) == nullptr;
    }

    static_assert(builds_from_pointer_and_size_in_constant_evaluation());

    // ============================================================ deduction

    constexpr enum_element<plain> plain_array[] = {
        {plain::a, u8"a"},
    };
    constexpr std::array<enum_element<plain>, 1> plain_std_array{
        {{plain::a, u8"a"}}
    };

    static_assert(std::is_same_v<
        decltype(enum_list_view(plain_array)),
        enum_list_view<plain>>);
    static_assert(std::is_same_v<
        decltype(enum_list_view(plain_std_array)),
        enum_list_view<plain>>);
    static_assert(std::is_same_v<
        decltype(enum_list_view(std::declval<std::vector<enum_element<plain>>&>())),
        enum_list_view<plain>>);
    // A span, including one of non-const elements.
    static_assert(std::is_same_v<
        decltype(enum_list_view(std::declval<std::span<enum_element<plain>>&>())),
        enum_list_view<plain>>);
    static_assert(std::is_same_v<
        decltype(enum_list_view(std::declval<std::span<const enum_element<plain>>&>())),
        enum_list_view<plain>>);

    // ============================================================ which
    // constructors exist

    using plain_view = enum_list_view<plain>;

    // From lvalue containers and arrays.
    static_assert(std::is_constructible_v<plain_view, std::vector<enum_element<plain>>&>);
    static_assert(std::is_constructible_v<plain_view, const std::vector<enum_element<plain>>&>);
    static_assert(std::is_constructible_v<plain_view, std::array<enum_element<plain>, 1>&>);
    static_assert(std::is_constructible_v<plain_view, const enum_element<plain>(&)[1]>);
    static_assert(std::is_constructible_v<plain_view, const enum_element<plain>*, std::size_t>);

    // Not from temporaries: the view would dangle.
    static_assert(not std::is_constructible_v<plain_view, std::vector<enum_element<plain>>&&>);
    // A temporary span is rejected too, though a span doesn't own its elements.
    // Strict but safe; pass the span as an lvalue.
    static_assert(not std::is_constructible_v<plain_view, std::span<const enum_element<plain>>&&>);
    static_assert(std::is_constructible_v<plain_view, std::span<const enum_element<plain>>&>);
    static_assert(not std::is_constructible_v<plain_view, const std::vector<enum_element<plain>>&&>);
    static_assert(not std::is_constructible_v<plain_view, std::array<enum_element<plain>, 1>&&>);

    // Not from containers of the wrong element type.
    static_assert(not std::is_constructible_v<plain_view, std::vector<derived_element>&>);
    static_assert(not std::is_constructible_v<plain_view, std::vector<enum_element<colour>>&>);
    static_assert(not std::is_constructible_v<plain_view, std::vector<int>&>);

    // Copies and moves, including from a const temporary view.
    static_assert(std::is_copy_constructible_v<plain_view>);
    static_assert(std::is_move_constructible_v<plain_view>);
    static_assert(std::is_constructible_v<plain_view, const plain_view&&>);
    static_assert(std::is_copy_assignable_v<plain_view>);

    // Constructors check the list, so may throw; the queries can't.
    static_assert(not std::is_nothrow_constructible_v<
        plain_view,
        const enum_element<plain>*,
        std::size_t>);
    static_assert(not std::is_nothrow_constructible_v<
        plain_view,
        std::vector<enum_element<plain>>&>);
    static_assert(noexcept(stored_colours.size()));
    static_assert(noexcept(stored_colours.begin()));
    static_assert(noexcept(stored_colours.end()));
    static_assert(noexcept(stored_colours[0]));
    static_assert(noexcept(stored_colours.find(colour::red)));
    static_assert(noexcept(stored_colours.find(u8"red")));

    // ============================================================ runtime

    TEST_CASE("A runtime list can be viewed and searched", "[enums]")
    {
        // Names built at runtime, kept in storage that outlives the view.
        std::vector<std::u8string> names{ u8"rouge", u8"vert" };
        names[0] += u8"!";
        const std::vector<enum_element<colour>> list{
            {colour::red,   names[0]},
            {colour::green, names[1]},
        };
        const enum_list_view view(list);

        REQUIRE(view.size() == 2);
        REQUIRE(view[1].value == colour::green);
        REQUIRE(view.find(u8"rouge!") != nullptr);
        CHECK(view.find(u8"rouge!")->value == colour::red);
        CHECK(view.find(colour::green)->name == u8"vert");
        CHECK(view.find(colour::blue) == nullptr);
        CHECK(view.find(u8"rouge") == nullptr);
    }

    TEST_CASE("Every constructor accepts a valid runtime list", "[enums]")
    {
        const enum_element<plain> array[] = {
            {plain::a, u8"a"},
            {plain::b, u8"b"},
        };
        const std::array<enum_element<plain>, 2> std_array{
            {{plain::a, u8"a"}, {plain::b, u8"b"}}
        };
        const std::vector<enum_element<plain>> vector{
            {plain::a, u8"a"},
            {plain::b, u8"b"},
        };

        CHECK(enum_list_view(array).size() == 2);
        CHECK(enum_list_view(std_array).size() == 2);
        CHECK(enum_list_view(vector).size() == 2);
        CHECK(enum_list_view<plain>(vector.data(), vector.size()).size() == 2);
        const std::span<const enum_element<plain>> span(vector);
        CHECK(enum_list_view(span).size() == 2);
    }

    TEST_CASE("A copied view refers to the same list", "[enums]")
    {
        const std::vector<enum_element<plain>> list{
            {plain::a, u8"a"},
        };
        const enum_list_view original(list);
        const enum_list_view copy = original;

        CHECK(copy.begin() == original.begin());
        CHECK(copy.size() == original.size());
    }

    TEST_CASE("The stored compile-time view works at runtime", "[enums]")
    {
        CHECK(stored_colours.size() == 3);
        REQUIRE(stored_colours.find(u8"green") != nullptr);
        CHECK(stored_colours.find(u8"green")->value == colour::green);
    }

    TEST_CASE("An invalid runtime list throws std::invalid_argument", "[enums]")
    {
        SECTION("An empty list")
        {
            const std::vector<enum_element<plain>> list;
            REQUIRE_THROWS_AS(enum_list_view<plain>(list), std::invalid_argument);
            REQUIRE_THROWS_WITH(
                enum_list_view<plain>(list),
                "enum_list must contain at least one entry"
            );
        }

        SECTION("A pointer with size zero")
        {
            const enum_element<plain> list[] = {
                {plain::a, u8"a"},
            };
            REQUIRE_THROWS_WITH(
                enum_list_view<plain>(list, 0),
                "enum_list must contain at least one entry"
            );
        }

        SECTION("An empty name")
        {
            const std::vector<enum_element<plain>> list{
                {plain::a, u8"a"},
                {plain::b, u8"" },
            };
            REQUIRE_THROWS_AS(enum_list_view(list), std::invalid_argument);
            REQUIRE_THROWS_WITH(
                enum_list_view(list), "enum_list names must be non-empty"
            );
        }

        SECTION("A repeated value")
        {
            const std::vector<enum_element<plain>> list{
                {plain::a, u8"a"},
                {plain::b, u8"b"},
                {plain::a, u8"c"},
            };
            REQUIRE_THROWS_AS(enum_list_view(list), std::invalid_argument);
            REQUIRE_THROWS_WITH(
                enum_list_view(list), "enum_list values must be unique"
            );
        }

        SECTION("A repeated name")
        {
            const std::vector<enum_element<plain>> list{
                {plain::a, u8"x"},
                {plain::b, u8"y"},
                {plain::c, u8"x"},
            };
            REQUIRE_THROWS_AS(enum_list_view(list), std::invalid_argument);
            REQUIRE_THROWS_WITH(
                enum_list_view(list), "enum_list names must be unique"
            );
        }

        SECTION("Every constructor checks")
        {
            const enum_element<plain> array[] = {
                {plain::a, u8"a"},
                {plain::a, u8"b"},
            };
            const std::array<enum_element<plain>, 2> std_array{
                {{plain::a, u8"a"}, {plain::a, u8"b"}}
            };
            CHECK_THROWS_AS(enum_list_view(array), std::invalid_argument);
            CHECK_THROWS_AS(enum_list_view(std_array), std::invalid_argument);
            CHECK_THROWS_AS(enum_list_view<plain>(array, 2), std::invalid_argument);
        }
    }
} // namespace enums_tests

UAB_NS_END

/*
 * Not tested here: an invalid compile-time enum_list. Using
 * enumerators_of<E>() on one is a compile error by design, so a test that
 * includes one can't build. To check it, compile a separate file that should
 * fail, e.g. with CMake:
 *
 *   try_compile(invalid_list_compiles ${CMAKE_BINARY_DIR}/invalid_list
 *       SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/should_not_compile/invalid_list.cpp
 *       CXX_STANDARD 23)
 *   if (invalid_list_compiles)
 *       message(FATAL_ERROR "An invalid enum_list compiled")
 *   endif ()
 */
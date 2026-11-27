/*!
 * @file
 * @brief Tests for enum_element, enum_list (base lists and versions),
 * enum_list_view, has_enum_list_c and enumerators_of.
 *
 * Compile-time behaviour is checked with static_assert, so this file only
 * compiles if it holds. Runtime behaviour (runtime lists, and the exceptions
 * the constructors throw) is checked with Catch2.
 *
 * Using a broken compile-time enum_list is a compile error by design. The
 * broken lists below are never passed to enumerators_of; instead the function
 * that checks each list, make_checked_view, is called at runtime, and what it
 * throws is checked. enumerators_of only reaches a list through that
 * function, during constant evaluation, where a throw is a compile error.
 */
#include "type_blueprints/core/enum.hpp"

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
#include <utility>
#include <vector>

// ============================================================ test types

// Listed with a built-in array, with two versions.
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

// Sparse, very large and negative values, listed out of numeric order.
enum class sparse : long long
{
    negative = -9'000'000'000'000'000'000LL,
    zero = 0,
    huge = 9'000'000'000'000'000'000LL,
    middle = 1LL << 40,
};

// An unscoped enumeration.
enum unscoped
{
    unscoped_first,
    unscoped_second
};

// Has an alias: only one of the two names can be listed.
enum class with_alias
{
    first,
    last,
    final_one = last
};

// Has a version but no base list.
enum class orphan
{
    a
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

// Version tags.
struct french
{
};

struct symbols
{
};

// Never used as a version.
struct unused_version
{
};

// Broken lists: one enumeration per broken rule. Never passed to
// enumerators_of, which would be a compile error.
enum class broken_empty_list
{
    a
};

enum class broken_empty_name
{
    a,
    b
};

enum class broken_repeated_value
{
    a,
    b
};

enum class broken_repeated_name
{
    a,
    b
};

enum class broken_name_with_space
{
    a
};

enum class broken_name_starting_with_digit
{
    a
};

enum class broken_non_ascii_name
{
    a
};

// Tags for versions of colour (whose base list is fine) that are broken.
struct missing_value_version
{
};

struct wrong_order_version
{
};

struct repeated_name_version
{
};

TYPE_BLUEPRINTS_NS_BEGIN

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
struct enum_list<colour, french>
{
    static constexpr enum_element<colour> values[] = {
        {colour::red,   u8"rouge"},
        {colour::green, u8"vert" },
        {colour::blue,  u8"bleu" },
    };
};

// Non-ASCII names are fine in a version.
template <>
struct enum_list<colour, symbols>
{
    static constexpr enum_element<colour> values[] = {
        {colour::red,   u8"●"},
        {colour::green, u8"✓"},
        {colour::blue,  u8"✗"},
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
struct enum_list<sparse>
{
    static constexpr enum_element<sparse> values[] = {
        {sparse::negative, u8"negative"},
        {sparse::zero,     u8"zero"    },
        {sparse::huge,     u8"huge"    },
        {sparse::middle,   u8"middle"  },
    };
};

template <>
struct enum_list<unscoped>
{
    static constexpr enum_element<unscoped> values[] = {
        {unscoped_first,  u8"unscoped_first" },
        {unscoped_second, u8"unscoped_second"},
    };
};

template <>
struct enum_list<with_alias>
{
    static constexpr enum_element<with_alias> values[] = {
        {with_alias::first, u8"first"},
        {with_alias::last,  u8"last" },
    };
};

template <>
struct enum_list<orphan, french>
{
    static constexpr enum_element<orphan> values[] = {
        {orphan::a, u8"a"},
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

// ------------------------------------------------------------ broken lists

template <>
struct enum_list<broken_empty_list>
{
    // A built-in array can't be empty, so this uses a std::array.
    static constexpr std::array<enum_element<broken_empty_list>, 0> values{};
};

template <>
struct enum_list<broken_empty_name>
{
    static constexpr enum_element<broken_empty_name> values[] = {
        {broken_empty_name::a, u8"a"},
        {broken_empty_name::b, u8"" },
    };
};

template <>
struct enum_list<broken_repeated_value>
{
    static constexpr enum_element<broken_repeated_value> values[] = {
        {broken_repeated_value::a, u8"a"},
        {broken_repeated_value::a, u8"b"},
    };
};

template <>
struct enum_list<broken_repeated_name>
{
    static constexpr enum_element<broken_repeated_name> values[] = {
        {broken_repeated_name::a, u8"a"},
        {broken_repeated_name::b, u8"a"},
    };
};

template <>
struct enum_list<broken_name_with_space>
{
    static constexpr enum_element<broken_name_with_space> values[] = {
        {broken_name_with_space::a, u8"a b"},
    };
};

template <>
struct enum_list<broken_name_starting_with_digit>
{
    static constexpr enum_element<broken_name_starting_with_digit> values[] = {
        {broken_name_starting_with_digit::a, u8"2a"},
    };
};

// C++ allows some non-ASCII identifiers, but base names must be ASCII.
template <>
struct enum_list<broken_non_ascii_name>
{
    static constexpr enum_element<broken_non_ascii_name> values[] = {
        {broken_non_ascii_name::a, u8"été"},
    };
};

template <>
struct enum_list<colour, missing_value_version>
{
    static constexpr enum_element<colour> values[] = {
        {colour::red,   u8"rouge"},
        {colour::green, u8"vert" },
    };
};

template <>
struct enum_list<colour, wrong_order_version>
{
    static constexpr enum_element<colour> values[] = {
        {colour::green, u8"vert" },
        {colour::red,   u8"rouge"},
        {colour::blue,  u8"bleu" },
    };
};

// A version can still break the general rules.
template <>
struct enum_list<colour, repeated_name_version>
{
    static constexpr enum_element<colour> values[] = {
        {colour::red,   u8"x"},
        {colour::green, u8"y"},
        {colour::blue,  u8"x"},
    };
};

// The tests sit inside the library's namespace, so they don't depend on what
// TYPE_BLUEPRINTS_NS_BEGIN names it.
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
    static_assert(has_enum_list_c<sparse>);
    static_assert(has_enum_list_c<unscoped>);
    // void is the base list.
    static_assert(has_enum_list_c<colour, void>);

    // Versions.
    static_assert(has_enum_list_c<colour, french>);
    static_assert(has_enum_list_c<colour, symbols>);
    static_assert(not has_enum_list_c<colour, unused_version>);
    // A version needs a base list.
    static_assert(not has_enum_list_c<orphan, french>);
    static_assert(not has_enum_list_c<orphan>);

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

    // index_of by value and by name; npos on a miss.
    static_assert(enumerators_of<colour>().index_of(colour::red) == 0);
    static_assert(enumerators_of<colour>().index_of(colour::blue) == 2);
    static_assert(enumerators_of<colour>().index_of(u8"green") == 1);
    static_assert(enumerators_of<colour>().index_of(static_cast<colour>(42))
        == enum_list_view<colour>::npos);
    static_assert(enumerators_of<colour>().index_of(u8"purple")
        == enum_list_view<colour>::npos);

    // name_of; empty on a miss.
    static_assert(enumerators_of<colour>().name_of(colour::green) == u8"green");
    static_assert(enumerators_of<colour>().name_of(static_cast<colour>(42)).empty());

    // The view enumerators_of returns can be kept, and used at runtime.
    constexpr enum_list_view<colour> stored_colours = enumerators_of<colour>();
    static_assert(stored_colours.size() == 3);

    // ============================================================ sparse, large
    // and negative values, and unscoped enumerations

    // Order is the list's order, not numeric order.
    static_assert(enumerators_of<sparse>()[2].value == sparse::huge);
    static_assert(enumerators_of<sparse>()[3].value == sparse::middle);
    static_assert(enumerators_of<sparse>().index_of(sparse::negative) == 0);
    static_assert(enumerators_of<sparse>().name_of(sparse::huge) == u8"huge");
    static_assert(enumerators_of<sparse>().find(u8"middle")->value == sparse::middle);
    // A value between listed values isn't found.
    static_assert(enumerators_of<sparse>().find(static_cast<sparse>(1)) == nullptr);

    static_assert(enumerators_of<unscoped>().find(unscoped_second)->name
        == u8"unscoped_second");

    // The alias's value is found under the listed name.
    static_assert(enumerators_of<with_alias>().name_of(with_alias::final_one)
        == u8"last");

    // ============================================================ versions

    // A version is the same type of view as the base list.
    static_assert(std::is_same_v<
        decltype(enumerators_of<colour, french>()),
        enum_list_view<colour>>);

    // Same values, same order, different names.
    static_assert(enumerators_of<colour, french>().size() == 3);
    static_assert(enumerators_of<colour, french>()[1].value == colour::green);
    static_assert(enumerators_of<colour, french>()[1].name == u8"vert");
    static_assert(enumerators_of<colour, french>().name_of(colour::blue) == u8"bleu");
    static_assert(enumerators_of<colour, french>().find(u8"rouge")->value == colour::red);
    // Base names aren't in the version.
    static_assert(enumerators_of<colour, french>().find(u8"red") == nullptr);

    // An index means the same enumerator in every version.
    constexpr bool
        indices_agree_across_versions()
    {
        constexpr auto base{ enumerators_of<colour>() };
        constexpr auto in_french{ enumerators_of<colour, french>() };
        constexpr auto in_symbols{ enumerators_of<colour, symbols>() };
        for (std::size_t idx{ 0 }; idx < base.size(); ++idx)
        {
            const colour value{ base[idx].value };
            if (in_french.index_of(value) != idx
                || in_symbols.index_of(value) != idx)
            {
                return false;
            }
        }
        return true;
    }

    static_assert(indices_agree_across_versions());

    // Non-ASCII names in a version.
    static_assert(enumerators_of<colour, symbols>().name_of(colour::green) == u8"✓");
    static_assert(enumerators_of<colour, symbols>().find(u8"✗")->value == colour::blue);

    // ============================================================ ASCII
    // identifier check, used on base names

    static_assert(TYPE_BLUEPRINTS_INTERNAL_NS::is_ascii_identifier(u8"red"));
    static_assert(TYPE_BLUEPRINTS_INTERNAL_NS::is_ascii_identifier(u8"_red"));
    static_assert(TYPE_BLUEPRINTS_INTERNAL_NS::is_ascii_identifier(u8"Red_2"));
    static_assert(TYPE_BLUEPRINTS_INTERNAL_NS::is_ascii_identifier(u8"_"));
    static_assert(not TYPE_BLUEPRINTS_INTERNAL_NS::is_ascii_identifier(u8""));
    static_assert(not TYPE_BLUEPRINTS_INTERNAL_NS::is_ascii_identifier(u8"2red"));
    static_assert(not TYPE_BLUEPRINTS_INTERNAL_NS::is_ascii_identifier(u8"dark green"));
    static_assert(not TYPE_BLUEPRINTS_INTERNAL_NS::is_ascii_identifier(u8"red-ish"));
    static_assert(not TYPE_BLUEPRINTS_INTERNAL_NS::is_ascii_identifier(u8"geo::red"));
    static_assert(not TYPE_BLUEPRINTS_INTERNAL_NS::is_ascii_identifier(u8"été"));
    // An embedded null; built with a length, as a literal would stop at it.
    static_assert(not TYPE_BLUEPRINTS_INTERNAL_NS::is_ascii_identifier(std::u8string_view(u8"red\0", 4)));

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
            && view.find(plain::c) == nullptr && view.index_of(plain::b) == 1;
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
    static_assert(std::is_same_v<
        decltype(enum_list_view(plain_array, 1)),
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
    static_assert(noexcept(stored_colours.index_of(colour::red)));
    static_assert(noexcept(stored_colours.index_of(u8"red")));
    static_assert(noexcept(stored_colours.name_of(colour::red)));

    // ============================================================ runtime

    TEST_CASE("A runtime list can be viewed and searched", "[enums]")
    {
        // Names built at runtime, kept in storage that outlives the view. A
        // runtime list's names needn't be identifiers.
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
        CHECK(view.index_of(u8"vert") == 1);
        CHECK(view.index_of(colour::blue) == enum_list_view<colour>::npos);
        CHECK(view.name_of(colour::red) == u8"rouge!");
        CHECK(view.name_of(colour::blue).empty());
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

    TEST_CASE("Versions can be used at runtime", "[enums]")
    {
        // A runtime value, so the lookups can't be folded at compile time.
        std::vector<colour> picked{ colour::blue };
        const colour value{ picked.front() };
        const enum_list_view<colour> in_symbols{ enumerators_of<colour, symbols>() };

        CHECK(in_symbols.name_of(value) == u8"✗");
        CHECK(enumerators_of<colour, french>().name_of(value) == u8"bleu");
        CHECK(enumerators_of<colour>().index_of(value) == in_symbols.index_of(value));
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

    TEST_CASE("A broken compile-time list is rejected", "[enums]")
    {
        // What enumerators_of would do at compile time, done at runtime so the
        // exception can be checked. At compile time each of these is an error.
        using TYPE_BLUEPRINTS_INTERNAL_NS::make_checked_view;

        SECTION("An empty list")
        {
            REQUIRE_THROWS_WITH(
                (make_checked_view<broken_empty_list, void>()),
                "enum_list must contain at least one entry"
            );
        }

        SECTION("An empty name")
        {
            REQUIRE_THROWS_WITH(
                (make_checked_view<broken_empty_name, void>()),
                "enum_list names must be non-empty"
            );
        }

        SECTION("A repeated value")
        {
            REQUIRE_THROWS_WITH(
                (make_checked_view<broken_repeated_value, void>()),
                "enum_list values must be unique"
            );
        }

        SECTION("A repeated name")
        {
            REQUIRE_THROWS_WITH(
                (make_checked_view<broken_repeated_name, void>()),
                "enum_list names must be unique"
            );
        }

        SECTION("A base name that isn't an identifier")
        {
            REQUIRE_THROWS_WITH(
                (make_checked_view<broken_name_with_space, void>()),
                "enum_list base names must be ASCII identifiers"
            );
            REQUIRE_THROWS_WITH(
                (make_checked_view<broken_name_starting_with_digit, void>()),
                "enum_list base names must be ASCII identifiers"
            );
        }

        SECTION("A base name that isn't ASCII")
        {
            REQUIRE_THROWS_WITH(
                (make_checked_view<broken_non_ascii_name, void>()),
                "enum_list base names must be ASCII identifiers"
            );
        }

        SECTION("A version missing a value")
        {
            REQUIRE_THROWS_WITH(
                (make_checked_view<colour, missing_value_version>()),
                "enum_list version must list every value in the base list"
            );
        }

        SECTION("A version in the wrong order")
        {
            REQUIRE_THROWS_WITH(
                (make_checked_view<colour, wrong_order_version>()),
                "enum_list version must list values in the base list's order"
            );
        }

        SECTION("A version breaking a general rule")
        {
            REQUIRE_THROWS_WITH(
                (make_checked_view<colour, repeated_name_version>()),
                "enum_list names must be unique"
            );
        }

        SECTION("Valid lists pass")
        {
            CHECK_NOTHROW(make_checked_view<colour, void>());
            CHECK_NOTHROW(make_checked_view<colour, french>());
            CHECK_NOTHROW(make_checked_view<colour, symbols>());
        }
    }
} // namespace enums_tests

TYPE_BLUEPRINTS_NS_END
#pragma once
/*!
 * @file
 * @brief Tests for object_traits, to_tuple, to_value_tuple and from_tuple.
 *
 * All static_asserts: the file only compiles if they hold.
 */
#include "unpack_and_build/core/object.hpp"

#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

// ============================================================ test types

namespace object_test_types
{
// Fields read by reference (std::tie): the usual case.
struct point
{
    int x;
    int y;

    constexpr bool
        operator==(const point&) const
        = default;
};

// A field that allocates.
struct person
{
    std::u8string name;
    int           age;

    constexpr bool
        operator==(const person&) const
        = default;
};

// Fields only reachable by value, through getters (like std::complex).
class by_value
{
public:
    constexpr by_value(
        const int first_arg,
        const int second_arg
    ) noexcept
        : _first(first_arg), _second(second_arg)
    {}

    constexpr int
        first() const noexcept
    {
        return _first;
    }

    constexpr int
        second() const noexcept
    {
        return _second;
    }
private:
    int _first;
    int _second;
};

// One field by reference, one by value: still dangles on a temporary.
struct mixed
{
    int x;
    int y;
};

// No fields.
struct empty
{};

// A built-in array field: readable, but not as values.
struct with_array
{
    int values[2];
};

// Not an object.
struct unlisted
{
    int x;
};
} // namespace object_test_types

UAB_NS_BEGIN

template <>
struct object_traits<object_test_types::point>
{
    static constexpr auto
        to_tuple(
            const object_test_types::point& point_arg
        ) noexcept
    {
        return std::tie(point_arg.x, point_arg.y);
    }
};

template <>
struct object_traits<object_test_types::person>
{
    static constexpr auto
        to_tuple(
            const object_test_types::person& person_arg
        ) noexcept
    {
        return std::tie(person_arg.name, person_arg.age);
    }
};

template <>
struct object_traits<object_test_types::by_value>
{
    static constexpr std::tuple<int, int>
        to_tuple(
            const object_test_types::by_value& object_arg
        ) noexcept
    {
        return {object_arg.first(), object_arg.second()};
    }
};

template <>
struct object_traits<object_test_types::mixed>
{
    static constexpr std::tuple<const int&, int>
        to_tuple(
            const object_test_types::mixed& object_arg
        ) noexcept
    {
        return {object_arg.x, object_arg.y};
    }
};

template <>
struct object_traits<object_test_types::empty>
{
    static constexpr std::tuple<>
        to_tuple(
            const object_test_types::empty&
        ) noexcept
    {
        return {};
    }
};

template <>
struct object_traits<object_test_types::with_array>
{
    static constexpr auto
        to_tuple(
            const object_test_types::with_array& object_arg
        ) noexcept
    {
        return std::tie(object_arg.values);
    }
};

namespace object_tests
{
using namespace ::object_test_types;

// ============================================================ concepts

static_assert(object_c<point>);
static_assert(object_c<by_value>);
static_assert(object_c<with_array>);
static_assert(not object_c<unlisted>);
static_assert(not object_c<const point>);

static_assert(valued_object_c<point>);
static_assert(valued_object_c<empty>);
static_assert(not valued_object_c<with_array>);

static_assert(buildable_object_c<point>);
static_assert(buildable_object_c<person>);
static_assert(buildable_object_c<by_value>);
static_assert(buildable_object_c<empty>);
static_assert(not buildable_object_c<with_array>);
static_assert(not buildable_object_c<unlisted>);

// ============================================================ types

static_assert(std::is_same_v<object_fields_t<point>, std::tuple<const int&, const int&>>);
static_assert(std::is_same_v<object_values_t<point>, std::tuple<int, int>>);
static_assert(std::is_same_v<object_values_t<person>, std::tuple<std::u8string, int>>);
static_assert(std::is_same_v<object_fields_t<by_value>, std::tuple<int, int>>);
static_assert(std::is_same_v<object_values_t<mixed>, std::tuple<int, int>>);

// ============================================================ to_tuple on
// temporaries: deleted when a field is a reference; to_value_tuple is for
// temporaries

template <typename T>
concept to_tuple_of_temporary_c
    = requires { to_tuple(std::declval<T>()); };

template <typename T>
concept to_tuple_of_lvalue_c
    = requires (const T& object_arg) { to_tuple(object_arg); };

// Rejected on a temporary when any field is a reference into it...
static_assert(not to_tuple_of_temporary_c<point>);
static_assert(not to_tuple_of_temporary_c<person>);
static_assert(not to_tuple_of_temporary_c<with_array>);
static_assert(not to_tuple_of_temporary_c<mixed>);
// ...but allowed when every field is a value, as nothing can dangle.
static_assert(to_tuple_of_temporary_c<by_value>);
static_assert(to_tuple_of_temporary_c<empty>);
// Lvalues always work.
static_assert(to_tuple_of_lvalue_c<point>);
static_assert(to_tuple_of_lvalue_c<by_value>);
static_assert(to_tuple_of_lvalue_c<with_array>);

// to_tuple passes on the traits' noexcept.
static_assert(noexcept(to_tuple(std::declval<const point&>())));

// ============================================================ to_tuple

// The fields refer to the object itself.
constexpr bool
    to_tuple_refers_to_object()
{
    const point original{1, 2};
    return &std::get<0>(to_tuple(original)) == &original.x
           && std::get<1>(to_tuple(original)) == 2;
}

static_assert(to_tuple_refers_to_object());

// ============================================================ to_value_tuple

template <typename T>
concept to_value_tuple_of_temporary_c
    = requires { to_value_tuple(std::declval<T>()); };

// Allowed on temporaries, whatever to_tuple returns.
static_assert(to_value_tuple_of_temporary_c<point>);
static_assert(to_value_tuple_of_temporary_c<by_value>);
static_assert(to_value_tuple_of_temporary_c<mixed>);
static_assert(to_value_tuple_of_temporary_c<empty>);
// Not without values.
static_assert(not to_value_tuple_of_temporary_c<with_array>);

static_assert(std::is_same_v<
              decltype(to_value_tuple(std::declval<const point&>())),
              std::tuple<int, int>>);

constexpr bool
    to_value_tuple_copies()
{
    point original{1, 2};
    auto  values = to_value_tuple(original);
    original.x   = 10;
    return values == std::tuple<int, int>{1, 2};
}

static_assert(to_value_tuple_copies());

constexpr bool
    to_value_tuple_of_temporary()
{
    return to_value_tuple(point{3, 4}) == std::tuple<int, int>{3, 4}
           && to_value_tuple(person{u8"Ada", 36})
                  == std::tuple<std::u8string, int>{u8"Ada", 36}
           && to_value_tuple(mixed{5, 6}) == std::tuple<int, int>{5, 6};
}

static_assert(to_value_tuple_of_temporary());

// ============================================================ from_tuple

constexpr bool
    round_trips()
{
    const point original_point{1, 2};
    // Not const: GCC 13 rejects a const std::u8string in constant evaluation.
    person      original_person{u8"Ada", 36};
    auto        values = to_value_tuple(original_person);
    const by_value rebuilt_by_value
        = from_tuple<by_value>(to_value_tuple(by_value(7, 8)));
    return from_tuple<point>(to_value_tuple(original_point)) == original_point
           // From an lvalue tuple: copied, so values is unchanged.
           && from_tuple<person>(values) == original_person
           && std::get<0>(values) == u8"Ada"
           && rebuilt_by_value.first() == 7 && rebuilt_by_value.second() == 8;
}

static_assert(round_trips());

constexpr bool
    builds_empty()
{
    [[maybe_unused]] const empty rebuilt = from_tuple<empty>({});
    return true;
}

static_assert(builds_empty());
} // namespace object_tests

UAB_NS_END

/*!
 * @file
 * @brief Tests for tuples.hpp. All static_asserts: the file only compiles if
 * they hold.
 */
#include "type_blueprints/core/tuples.hpp"

#include <array>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

 // ============================================================ test types

namespace user
{
    // A tuple-like with a member get.
    struct member_get_pair
    {
        int  first;
        long second;

        template <std::size_t I>
        constexpr const auto&
            get() const noexcept
        {
            if constexpr (I == 0)
            {
                return first;
            }
            else
            {
                return second;
            }
        }
    };

    // A tuple-like with a free get, found by argument-dependent lookup.
    struct free_get_pair
    {
        int  first;
        char second;
    };

    template <std::size_t I>
    constexpr auto&
        get(
            free_get_pair& pair_arg
        ) noexcept
    {
        if constexpr (I == 0)
        {
            return pair_arg.first;
        }
        else
        {
            return pair_arg.second;
        }
    }

    template <std::size_t I>
    constexpr const auto&
        get(
            const free_get_pair& pair_arg
        ) noexcept
    {
        if constexpr (I == 0)
        {
            return pair_arg.first;
        }
        else
        {
            return pair_arg.second;
        }
    }

    // Both a member and a free get, which disagree: the member is preferred.
    struct both_gets
    {
        template <std::size_t I>
        constexpr int
            get() const noexcept
        {
            return 1;
        }
    };

    template <std::size_t I>
    constexpr int
        get(
            const both_gets&
        ) noexcept
    {
        return 2;
    }

    // tuple_size and tuple_element, but get only for index 0.
    struct partial_get
    {
    };

    template <std::size_t I>
        requires (I == 0)
    constexpr int
        get(
            const partial_get&
        ) noexcept
    {
        return 0;
    }

    // tuple_size and get, but no tuple_element.
    struct no_tuple_element
    {
    };

    template <std::size_t I>
    constexpr int
        get(
            const no_tuple_element&
        ) noexcept
    {
        return 0;
    }

    // tuple_size and tuple_element, but no get at all.
    struct no_get
    {
    };
} // namespace user

template <>
struct std::tuple_size<user::member_get_pair>
    : std::integral_constant<std::size_t, 2>
{
};

template <>
struct std::tuple_element<0, user::member_get_pair>
{
    using type = int;
};

template <>
struct std::tuple_element<1, user::member_get_pair>
{
    using type = long;
};

template <>
struct std::tuple_size<user::free_get_pair>
    : std::integral_constant<std::size_t, 2>
{
};

template <>
struct std::tuple_element<0, user::free_get_pair>
{
    using type = int;
};

template <>
struct std::tuple_element<1, user::free_get_pair>
{
    using type = char;
};

template <>
struct std::tuple_size<user::both_gets> : std::integral_constant<std::size_t, 1>
{
};

template <>
struct std::tuple_element<0, user::both_gets>
{
    using type = int;
};

template <>
struct std::tuple_size<user::partial_get> : std::integral_constant<std::size_t, 2>
{
};

template <std::size_t I>
struct std::tuple_element<I, user::partial_get>
{
    using type = int;
};

template <>
struct std::tuple_size<user::no_tuple_element>
    : std::integral_constant<std::size_t, 1>
{
};

template <>
struct std::tuple_size<user::no_get> : std::integral_constant<std::size_t, 1>
{
};

template <>
struct std::tuple_element<0, user::no_get>
{
    using type = int;
};

TYPE_BLUEPRINTS_NS_BEGIN
namespace tuples_tests
{
    // ============================================================
    // index_function_c and index_predicate_c

    // Takes any index.
    inline constexpr auto any_index = [](auto) {};

    // Takes only index 0.
    struct only_zero
    {
        constexpr void
            operator()(std::integral_constant<std::size_t, 0>) const noexcept
        {
        }
    };

    struct not_a_bool
    {
    };

    inline constexpr auto returns_true = [](auto) { return true; };
    inline constexpr auto returns_int = [](auto) { return 1; };
    inline constexpr auto returns_void = [](auto) {};
    inline constexpr auto returns_not_a_bool = [](auto) { return not_a_bool{}; };

    static_assert(index_function_c<decltype(any_index), 3>);
    static_assert(index_function_c<only_zero, 1>);
    // Every index must be accepted, not just the first.
    static_assert(not index_function_c<only_zero, 2>);
    static_assert(not index_function_c<int, 1>);
    // With no indices, nothing is called, so the check passes trivially.
    static_assert(index_function_c<only_zero, 0>);

    static_assert(index_predicate_c<decltype(returns_true), 3>);
    // Anything explicitly convertible to bool.
    static_assert(index_predicate_c<decltype(returns_int), 2>);
    static_assert(not index_predicate_c<decltype(returns_void), 2>);
    static_assert(not index_predicate_c<decltype(returns_not_a_bool), 2>);
    static_assert(not index_predicate_c<only_zero, 2>);

    // ============================================================ for_each_index

    // Indices arrive in order, as constant expressions.
    constexpr bool
        visits_in_order()
    {
        std::size_t digits = 0;
        for_each_index<4>(
            [&](const auto index_arg)
            {
                static_assert(decltype(index_arg)::value < 4);
                digits = digits * 10 + index_arg;
            }
        );
        return digits == 123; // 0, 1, 2, 3
    }

    static_assert(visits_in_order());

    constexpr bool
        zero_indices_calls_nothing()
    {
        int calls = 0;
        for_each_index<0>([&](auto) { ++calls; });
        return calls == 0;
    }

    static_assert(zero_indices_calls_nothing());

    // The index can be used where a constant is needed.
    constexpr bool
        index_is_a_constant()
    {
        const std::tuple<int, int, int> tuple{ 1, 2, 3 };
        int                             total = 0;
        for_each_index<3>([&](auto index_arg) { total += std::get<index_arg>(tuple); });
        return total == 6;
    }

    static_assert(index_is_a_constant());

    // ============================================================ all_of_indices

    // Stops at the first false.
    constexpr bool
        all_of_stops_early()
    {
        int        calls = 0;
        const bool result = all_of_indices<5>(
            [&](const auto index_arg)
            {
                ++calls;
                return index_arg < 2;
            }
        );
        return not result && calls == 3; // 0, 1, then 2 fails
    }

    static_assert(all_of_stops_early());
    static_assert(all_of_indices<3>(returns_true));
    static_assert(all_of_indices<2>(returns_int));
    // True for no indices.
    static_assert(all_of_indices<0>([](auto) { return false; }));

    // ============================================================ tuple_like_c

    // Standard tuple-likes.
    static_assert(tuple_like_c<std::tuple<>>);
    static_assert(tuple_like_c<std::tuple<int, char>>);
    static_assert(tuple_like_c<std::pair<int, char>>);
    static_assert(tuple_like_c<std::array<int, 3>>);
    static_assert(tuple_like_c<std::array<int, 0>>);

    // User tuple-likes, with either kind of get.
    static_assert(tuple_like_c<user::member_get_pair>);
    static_assert(tuple_like_c<user::free_get_pair>);
    static_assert(tuple_like_c<user::both_gets>);

    // Each part of the protocol is required.
    static_assert(not tuple_like_c<user::partial_get>);      // get missing for an index
    static_assert(not tuple_like_c<user::no_tuple_element>); // no tuple_element
    static_assert(not tuple_like_c<user::no_get>);           // no get at all

    // Not tuple-likes.
    static_assert(not tuple_like_c<int>);
    static_assert(not tuple_like_c<int[3]>);
    static_assert(not tuple_like_c<std::vector<int>>);

    // ============================================================ get_element

    constexpr bool
        gets_std_elements()
    {
        const std::tuple<int, char> tuple{ 1, 'a' };
        const std::pair<int, long>  pair{ 2, 3L };
        const std::array<int, 2>    array{ 4, 5 };
        return get_element<0>(tuple) == 1 && get_element<1>(tuple) == 'a'
            && get_element<0>(pair) == 2 && get_element<1>(pair) == 3L
            && get_element<0>(array) == 4 && get_element<1>(array) == 5;
    }

    static_assert(gets_std_elements());

    constexpr bool
        gets_user_elements()
    {
        const user::member_get_pair member{ 6, 7L };
        const user::free_get_pair   free{ 8, 'b' };
        return get_element<0>(member) == 6 && get_element<1>(member) == 7L
            && get_element<0>(free) == 8 && get_element<1>(free) == 'b';
    }

    static_assert(gets_user_elements());

    // A member get is preferred to a free one.
    static_assert(get_element<0>(user::both_gets{}) == 1);

    // Elements keep the tuple's value category.
    static_assert(std::is_same_v<
        decltype(get_element<0>(std::declval<std::tuple<int>&>())),
        int&>);
    static_assert(std::is_same_v<
        decltype(get_element<0>(std::declval<const std::tuple<int>&>())),
        const int&>);
    static_assert(std::is_same_v<
        decltype(get_element<0>(std::declval<std::tuple<int>>())),
        int&&>);

    // Elements can be modified through an lvalue.
    constexpr bool
        modifies_through_get_element()
    {
        std::tuple<int, int> tuple{ 1, 2 };
        get_element<1>(tuple) = 20;
        user::free_get_pair free{ 3, 'c' };
        get_element<0>(free) = 30;
        return std::get<1>(tuple) == 20 && free.first == 30;
    }

    static_assert(modifies_through_get_element());

    // Only for tuple-likes.
    template <typename T>
    concept get_element_callable_c
        = requires (T & tuple_arg) { get_element<0>(tuple_arg); };

    static_assert(get_element_callable_c<std::tuple<int>>);
    static_assert(not get_element_callable_c<user::no_get>);
    static_assert(not get_element_callable_c<int>);

    // ============================================================
    // element_function_c

    inline constexpr auto takes_int_ref = [](int&) {};

    static_assert(element_function_c<decltype(any_index), std::tuple<int, char>>);
    static_assert(element_function_c<decltype(any_index), user::free_get_pair&>);
    static_assert(element_function_c<
        decltype(any_index),
        const user::member_get_pair&>);
    // The tuple keeps its value category: an lvalue gives int&...
    static_assert(element_function_c<decltype(takes_int_ref), std::tuple<int, int>&>);
    // ...an rvalue gives int&&, which an int& can't take...
    static_assert(not element_function_c<
        decltype(takes_int_ref),
        std::tuple<int, int>>);
    // ...and a const lvalue gives const int&.
    static_assert(not element_function_c<
        decltype(takes_int_ref),
        const std::tuple<int, int>&>);
    // Every element: char& isn't an int&.
    static_assert(not element_function_c<
        decltype(takes_int_ref),
        std::tuple<int, char>&>);
    // Not tuple-likes.
    static_assert(not element_function_c<decltype(any_index), int>);
    static_assert(not element_function_c<decltype(any_index), std::vector<int>&>);

    // ============================================================
    // for_each_element

    constexpr bool
        visits_every_element()
    {
        long total = 0;
        for_each_element(
            std::tuple<int, long, short>{1, 20L, 300},
            [&](const auto element_arg) { total += element_arg; }
        );
        const user::member_get_pair member{ 4000, 0L };
        for_each_element(member, [&](const auto element_arg) { total += element_arg; });
        return total == 4321;
    }

    static_assert(visits_every_element());

    // Elements are visited in order.
    constexpr bool
        visits_in_element_order()
    {
        int digits = 0;
        for_each_element(
            std::tuple<int, int, int>{1, 2, 3},
            [&](const int element_arg) { digits = digits * 10 + element_arg; }
        );
        return digits == 123;
    }

    static_assert(visits_in_element_order());

    // Elements can be modified through an lvalue tuple, std or user.
    constexpr bool
        modifies_elements()
    {
        std::tuple<int, int> tuple{ 1, 2 };
        for_each_element(tuple, [](int& element_arg) { element_arg *= 10; });
        user::free_get_pair free{ 3, 'a' };
        for_each_element(free, [](auto& element_arg) { ++element_arg; });
        return tuple == std::tuple<int, int>{10, 20}&& free.first == 4
            && free.second == 'b';
    }

    static_assert(modifies_elements());

    // An empty tuple calls nothing.
    constexpr bool
        empty_tuple_calls_nothing()
    {
        int calls = 0;
        for_each_element(std::tuple<>{}, [&](auto) { ++calls; });
        return calls == 0;
    }

    static_assert(empty_tuple_calls_nothing());

    // ============================================================ all_elements_c

    static_assert(all_elements_c<std::tuple<int, long>, std::is_integral>);
    static_assert(not all_elements_c<std::tuple<int, double>, std::is_integral>);
    // True for no elements.
    static_assert(all_elements_c<std::tuple<>, std::is_integral>);
    static_assert(all_elements_c<std::array<int, 2>, std::is_integral>);
    static_assert(all_elements_c<user::member_get_pair, std::is_integral>);
    static_assert(all_elements_c<std::pair<float, double>, std::is_floating_point>);
    // Not tuple-likes: false, rather than an error.
    static_assert(not all_elements_c<int, std::is_integral>);
    static_assert(not all_elements_c<std::vector<int>, std::is_integral>);
    static_assert(not all_elements_c<user::no_get, std::is_integral>);

    // Usable as a type constraint.
    template <all_elements_c<std::is_integral> T>
    constexpr bool
        takes_integral_tuple(T)
    {
        return true;
    }

    static_assert(takes_integral_tuple(std::tuple<int, char>{}));
} // namespace tuples_tests
TYPE_BLUEPRINTS_NS_END
#pragma once
// Namespaces.
#include "type_blueprints/core/internal/macros.hpp"

// std::constructible_from.
#include <concepts>
// std::size_t.
#include <cstddef>
// std::integral_constant, std::remove_cvref_t.
#include <type_traits>
// std::tuple_size, std::tuple_element, std::get, std::forward,
// std::index_sequence, std::make_index_sequence.
#include <utility>

/*!
 * @file
 * @brief Provides additional functionality for tuples and tuple-like objects,
 * and functions for working over ranges of integral constants.
 *
 */

TYPE_BLUEPRINTS_NS_BEGIN
TYPE_BLUEPRINTS_INTERNAL_NS_BEGIN
/*!
 * @brief Checks a function f is callable and valid for each integer I in
 * Indices (a std::index_sequence).
 *
 * Note std::integral_constant is used instead of an integral template
 * parameter.
 *
 * @tparam F The function to check.
 * @tparam Indices The index sequence to check against.
 */
template <typename F, typename Indices>
inline constexpr bool callable_with_indices_v = false;

template <typename F, std::size_t... Is>
inline constexpr bool callable_with_indices_v<F, std::index_sequence<Is...>>
    = (requires (F& function_arg) {
          function_arg(std::integral_constant<std::size_t, Is>{});
      } && ...);

/*!
 * @brief Checks that a function of type F can be called with the integral
 * type parameter I, and that the return type of F be constructed into a
 * bool.
 *
 * Note that we use integral_constant instead of an integral template parameter.
 */
template <typename F, std::size_t I>
concept testable_with_index_c
    = requires (F& function_arg
      ) { function_arg(std::integral_constant<std::size_t, I>{}); }
      && std::constructible_from<
          bool,
          decltype(std::declval<F&>()(std::integral_constant<std::size_t, I>{})
          )>;

/*!
 * @brief Checks that testable_with_index_c is true for a function type F and
 * all integer elements in a std::index_sequence.
 * @tparam F The function type to test.
 * @tparam Indices The index sequence to test the function on.
 */
template <typename F, typename Indices>
inline constexpr bool testable_with_indices_v = false;

template <typename F, std::size_t... Is>
inline constexpr bool testable_with_indices_v<F, std::index_sequence<Is...>>
    = (testable_with_index_c<F, Is> && ...);
TYPE_BLUEPRINTS_INTERNAL_NS_END
/*!
 * @brief Checks that the function type F can be called with all integer
 * constants from 0 to N - 1.
 *
 * Note, it expects F to take an std::integral_constant as an argument.
 *
 * @tparam F The function type.
 * @tparam N The limit on the integer parameter to check.
 */
template <typename F, std::size_t N>
concept index_function_c = TYPE_BLUEPRINTS_INTERNAL_NS::
    callable_with_indices_v<F, std::make_index_sequence<N>>;
/*!
 * @brief Checks that the function type F can be called with all integer
 * constants from 0 to N - 1, and that the return type is convertible to a bool.
 *
 * Note, it expects F to take an std::integral_constant as an argument.
 *
 * @tparam F The function type.
 * @tparam N The limit on the integer parameter to check.
 */
template <typename F, std::size_t N>
concept index_predicate_c = TYPE_BLUEPRINTS_INTERNAL_NS::
    testable_with_indices_v<F, std::make_index_sequence<N>>;
/*!
 * @brief Calls the function function_arg on all integers from 0 to N-1 in
 * order.
 *
 * Note, this function expects F to take an std::integral_constant as an
 * argument.
 *
 * @code
 * for_each_index<2>([&](auto index) { use(std::get<index>(tuple)); });
 * @endcode
 *
 * @tparam N The limit on the integer parameter to check.
 * @tparam F The function type of function_arg.
 * @param function_arg The function argument.
 */
template <std::size_t N, typename F>
requires index_function_c<F, N>
constexpr void
    for_each_index(F&& function_arg);
/*!
 * @brief Calls the function function_arg on all integers from 0 to N-1 in
 * order, stopping at the first call that returns false. If N == 0, returns
 * true.
 *
 * Note, this function expects F to take an std::integral_constant as an
 * argument.
 *
 * @tparam N The limit on the integer parameter to check.
 * @tparam F The function type of function_arg.
 * @param function_arg The function argument.
 * @return True if every call returned true, false otherwise.
 */
template <std::size_t N, typename F>
requires index_predicate_c<F, N>
constexpr bool
    all_of_indices(F&& function_arg);

TYPE_BLUEPRINTS_INTERNAL_NS_BEGIN
/*!
 * @brief Concept to check whether a tuple-like entity has a member get<I>()
 * function callable on it - as some tuple-like objects provide it instead of a
 * free function.
 *
 * @tparam Tuple The tuple-like object to check.
 * @tparam I The integer parameter to check.
 */
template <typename Tuple, std::size_t I>
concept member_get_c = requires (Tuple&& tuple_arg) {
    std::forward<Tuple>(tuple_arg).template get<I>();
};
/*!
 * @brief Concept to check whether a tuple-like entity has a free function
 * get<I>(arg) callable on it.
 *
 * The function is checked via ADL - (so std::get will work for
 * std::tuple/std::array/std::pair etc).
 *
 * @tparam Tuple The tuple-like object to check.
 * @tparam I The integer parameter to check.
 */
template <typename Tuple, std::size_t I>
concept free_get_c
    = requires (Tuple&& tuple_arg) { get<I>(std::forward<Tuple>(tuple_arg)); };
/*!
 * @brief Checks whether element I of T has a get function for the given
 * integer, and whether std::tuple_element<I,T>::type has a definition (the std
 * allows specialisation of this for a user's own tuple types).
 *
 * @tparam T The tuple-like element to check.
 * @tparam I The template integer argument index to check.
 */
template <typename T, std::size_t I>
concept element_gettable_c
    = (member_get_c<T&, I> || free_get_c<T&, I>)
      && requires { typename std::tuple_element<I, T>::type; };
TYPE_BLUEPRINTS_INTERNAL_NS_END
/*!
 * @brief Concept for checking that a type T is tuple-like.
 *
 * It does this by checking the three components of a tuple:
 * - An available get function.
 * - tuple_size is defined (part of std that can be specialised)
 * - tuple_element is defined (part of the std that can be specialised).
 *
 * @tparam T The type to check is tuple-like.
 */
template <typename T>
concept tuple_like_c = requires {
    typename std::integral_constant<std::size_t, std::tuple_size<T>::value>;
}&& all_of_indices<std::tuple_size<T>::value>(
    [](auto index) { return TYPE_BLUEPRINTS_INTERNAL_NS::element_gettable_c<T, decltype(index)::value>; }
);
/*!
 * @brief Generic function for getting an element from a tuple-like object.
 *
 * Designed to be a generic getter that works with an ADL free function get, and
 * a member function get.
 *
 * @tparam I The integer argument given to get, referring to the I-th element in
 * the tuple.
 * @tparam Tuple The type of tuple_arg.
 * @param tuple_arg The tuple-like object.
 * @return The I-th element in tuple_arg.
 */
template <std::size_t I, typename Tuple>
requires tuple_like_c<std::remove_cvref_t<Tuple>>
constexpr decltype(auto)
    get_element(Tuple&& tuple_arg);
/*!
 * @brief Concept that checks a function of type F can be called with every
 * member of some tuple-like object of type Tuple.
 *
 * @tparam F The function type.
 * @tparam Tuple The tuple-like object.
 */
template <typename F, typename Tuple>
concept element_function_c
    = tuple_like_c<std::remove_cvref_t<Tuple>>
      && all_of_indices<std::tuple_size<std::remove_cvref_t<Tuple>>::value>(
          [](auto index)
          {
              return requires (F& function_arg, Tuple&& tuple_arg) {
                  function_arg(get_element<decltype(index)::value>(
                      std::forward<Tuple>(tuple_arg)
                  ));
              };
          }
      );

/*!
 * @brief Calls function_arg with each element of a tuple-like in order.
 *
 * @tparam Tuple The type of the tuple-like object.
 * @tparam F The type of the function.
 * @param tuple_arg The tuple-like object. When it's an rvalue, each element is
 * passed as an rvalue; each element is passed once.
 * @param function_arg Called once per element.
 */
template <typename Tuple, typename F>
requires element_function_c<F, Tuple>
constexpr void
    for_each_element(Tuple&& tuple_arg, F&& function_arg);
/*!
 * @brief Satisfied when Tuple is a tuple-like and Trait<E>::value is true for
 * every element type E; true when it has no elements.
 *
 * @code
 * static_assert(all_elements_c<std::tuple<int, long>, std::is_integral>);
 * @endcode
 *
 * @tparam Tuple The type to check.
 * @tparam Trait A class template with a bool value member, such as a
 * standard type trait.
 */
template <typename Tuple, template <typename> typename Trait>
concept all_elements_c = tuple_like_c<Tuple>
                         && all_of_indices<std::tuple_size<Tuple>::value>(
                             [](auto index)
                             {
                                 return Trait<typename std::tuple_element<
                                     decltype(index)::value,
                                     Tuple>::type>::value;
                             }
                         );

TYPE_BLUEPRINTS_NS_END

TYPE_BLUEPRINTS_NS_BEGIN
template <std::size_t N, typename F>
requires index_function_c<F, N>
constexpr void
    for_each_index(
        F&& function_arg
    )
{
    [&]<std::size_t... Is>(std::index_sequence<Is...>)
    {
        (static_cast<void>(function_arg(std::integral_constant<std::size_t, Is>{
         })),
         ...);
    }(std::make_index_sequence<N>{});
}

template <std::size_t N, typename F>
requires index_predicate_c<F, N>
constexpr bool
    all_of_indices(
        F&& function_arg
    )
{
    return [&]<std::size_t... Is>(std::index_sequence<Is...>)
    {
        return (
            static_cast<bool>(
                function_arg(std::integral_constant<std::size_t, Is>{})
            )
            && ...
        );
    }(std::make_index_sequence<N>{});
}

template <std::size_t I, typename Tuple>
requires tuple_like_c<std::remove_cvref_t<Tuple>>
constexpr decltype(auto)
    get_element(
        Tuple&& tuple_arg
    )
{
    // Favours calling the member parameter.
    if constexpr (TYPE_BLUEPRINTS_INTERNAL_NS::member_get_c<Tuple, I>)
    {
        return std::forward<Tuple>(tuple_arg).template get<I>();
    }
    else if constexpr (TYPE_BLUEPRINTS_INTERNAL_NS::free_get_c<Tuple, I>)
    {
        return get<I>(std::forward<Tuple>(tuple_arg));
    }
    else
    {
        static_assert(
            false,
            "get_element: no get<I> callable on this tuple-like with this "
            "value category"
        );
    }
}

template <typename Tuple, typename F>
requires element_function_c<F, Tuple>
constexpr void
    for_each_element(
        Tuple&& tuple_arg,
        F&&     function_arg
    )
{
    for_each_index<std::tuple_size<std::remove_cvref_t<Tuple>>::value>(
        [&](const auto index_arg)
        {
            function_arg(get_element<decltype(index_arg)::value>(
                std::forward<Tuple>(tuple_arg)
            ));
        }
    );
}

TYPE_BLUEPRINTS_NS_END
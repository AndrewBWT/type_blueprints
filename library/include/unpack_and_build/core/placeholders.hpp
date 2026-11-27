#pragma once
/*!
 * @file
 * @brief Placeholders: a starting value for a type, for code that needs an
 * object to fill in before it has the real one.
 *
 * Appenders fill a sequence that already exists, and parsers write into an
 * object that already exists, so both need to make one first. Most types are
 * default-constructible; placeholder_value covers the ones that aren't (as
 * std::strong_ordering isn't), and so arrays of them.
 */
#include "unpack_and_build/core/internal/macros.hpp"

 // std::size_t.
#include <cstddef>
// std::is_array_v, std::is_default_constructible_v, std::is_same_v,
// std::is_unbounded_array_v, std::remove_all_extents_t.
#include <type_traits>
// std::index_sequence, std::make_index_sequence.
#include <utility>

UAB_NS_BEGIN

/*!
 * @brief Specialise to give T a starting value: static constexpr T value().
 *
 * The primary template has no definition; only specialisations count. A
 * specialisation is used in preference to T's default constructor, if it has
 * one.
 *
 * @code
 * template <>
 * struct type_extensions::placeholder_value<my_type>
 * {
 *     static constexpr my_type value() { return my_type(0); }
 * };
 * @endcode
 *
 * @tparam T The type of the value.
 */
    template <typename T>
struct placeholder_value;

/*!
 * @brief Satisfied when placeholder_value<T> is specialised, with a value()
 * returning a T.
 */
template <typename T>
concept has_placeholder_value_c = requires {
    requires std::is_same_v<decltype(placeholder_value<T>::value()), T>;
};

/*!
 * @brief Satisfied when a starting object of type T can be made: T, or for a
 * built-in array its innermost element type, has a placeholder_value or a
 * default constructor. An array of unknown bound can't be made at all.
 */
template <typename T>
concept placeholder_constructible_c
= (not std::is_unbounded_array_v<T>)
&& (has_placeholder_value_c<std::remove_all_extents_t<T>>
    || std::is_default_constructible_v<std::remove_all_extents_t<T>>);

/*!
 * @brief A starting value of type T: placeholder_value<T>::value() if it's
 * specialised, otherwise T{}.
 *
 * Not for built-in arrays, which can't be returned; use placeholder_object.
 */
template <typename T>
    requires placeholder_constructible_c<T> && (not std::is_array_v<T>)
constexpr T
make_placeholder()
{
    if constexpr (has_placeholder_value_c<T>)
    {
        return placeholder_value<T>::value();
    }
    else
    {
        return T{};
    }
}

/*!
 * @brief Holds a starting object of type T, in value. Works for built-in
 * arrays too, which make_placeholder can't return.
 *
 * - Not an array: value is make_placeholder<T>().
 * - An array whose innermost element type has a placeholder_value: each
 *   element is made from it. Expensive at compile time for large arrays.
 * - Any other array: value-initialised.
 */
template <typename T>
    requires placeholder_constructible_c<T>
struct placeholder_object
{
    T value;

    constexpr placeholder_object()
        requires (not std::is_array_v<T>)
    : value(make_placeholder<T>())
    {
    }

    constexpr placeholder_object()
        requires std::is_array_v<T>
    && (not has_placeholder_value_c<std::remove_all_extents_t<T>>)
        : value()
    {
    }

    constexpr placeholder_object()
        requires std::is_array_v<T>
    && has_placeholder_value_c<std::remove_all_extents_t<T>>
        // One index per innermost element: an array has no padding between
        // elements, so its size divides exactly.
        : placeholder_object(
            std::make_index_sequence<
            sizeof(T) / sizeof(std::remove_all_extents_t<T>)>{}
        )
    {
    }
private:
    // Fills every innermost element in order; brace elision spreads them over
    // nested arrays. Clang warns about brace elision, but there's no way to
    // write the inner braces for an array of any rank.
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif
    template <std::size_t... Indices>
    constexpr explicit placeholder_object(
        std::index_sequence<Indices...>
    )
        : value{ (static_cast<void>(Indices),
                 make_placeholder<std::remove_all_extents_t<T>>())... }
    {
    }
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
};

UAB_NS_END

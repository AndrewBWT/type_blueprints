#pragma once
/*!
 * @file
 * @brief Sequences: types made of a run of elements of one type.
 *
 * Four traits describe a sequence:
 * - sequence_value, the element type.
 * - sequence_build_value, the type of element added when building one.
 * - sequence_elements, an iterable view of the elements.
 * - sequence_fixed_count, the number of elements, when the type fixes it.
 *
 * Each is a primary template whose default is detected from the type's
 * members, where that generalises well (any iterable type with a value_type;
 * any map-like type). Types it doesn't generalise to are named explicitly in
 * support/std/ (std::array's fixed count, std::bitset's elements). The
 * library never partially specialises these traits itself, outside
 * support/std/, so a user's specialisation, full or partial, always wins
 * without ambiguity.
 *
 * Anything that uses a trait on a type must see that type's specialisations
 * first, or translation units disagree about the type (an ODR violation with
 * no diagnostic). Include type_extensions/support/std.hpp from your own
 * library's core.
 *
 * Building a sequence is in appenders.hpp.
 */
#include "type_blueprints/core/extended_concepts.hpp"
#include "type_blueprints/core/internal/macros.hpp"

// std::size_t.
#include <cstddef>
// std::is_bounded_array_v, std::is_array_v, std::remove_extent_t,
// std::extent_v, std::is_same_v, std::remove_cv_t, std::remove_cvref_t.
#include <type_traits>
// std::pair.
#include <utility>

UAB_NS_BEGIN

UAB_INTERNAL_NS_BEGIN

// No element type, unless one of the cases below applies. They're exclusive
// (array or not), and internal, so users never collide with them.
template <typename T>
struct detected_sequence_value
{};

// A built-in array's element type.
template <typename T>
requires std::is_bounded_array_v<T>
struct detected_sequence_value<T>
{
    using type = std::remove_extent_t<T>;
};

// The value_type of anything a range-for can iterate.
template <typename T>
requires (not std::is_array_v<T>)
         && iterable_c<T> && requires { typename T::value_type; }
struct detected_sequence_value<T>
{
    using type = typename T::value_type;
};

UAB_INTERNAL_NS_END

/*!
 * @brief The element type of a sequence, as a member type named type.
 *
 * Detected: for a built-in array, its element type; for anything a range-for
 * can iterate (iterable_c) with a value_type member, that value_type;
 * otherwise no type.
 *
 * Specialise for a type that holds a sequence but isn't iterable, along with
 * sequence_elements (as support/std/bitset.hpp does for std::bitset).
 */
template <typename T>
struct sequence_value : UAB_INTERNAL_NS::detected_sequence_value<T>
{};

/*!
 * @brief The element type of sequence T. See sequence_value.
 */
template <typename T>
using sequence_value_t = typename sequence_value<T>::type;

// ============================================================ build type

UAB_INTERNAL_NS_BEGIN
// The build type, given element type Value: Value itself, unless T is
// map-like.
template <typename T, typename Value>
struct build_value_from
{
    using type = Value;
};

// Map-like: key_type and mapped_type members, and an element type of exactly
// std::pair<const key_type, mapped_type>, which can't be assigned.
template <typename T, typename Value>
requires requires {
    typename T::key_type;
    typename T::mapped_type;
}
         && std::is_same_v<
             Value,
             std::pair<const typename T::key_type, typename T::mapped_type>>
struct build_value_from<T, Value>
{
    using type = std::pair<typename T::key_type, typename T::mapped_type>;
};

// No build type when there's no element type. Uses the sequence_value trait,
// so a specialised element type is respected.
template <typename T>
struct detected_sequence_build_value
{};

template <typename T>
requires requires { typename sequence_value<T>::type; }
struct detected_sequence_build_value<T>
    : build_value_from<T, typename sequence_value<T>::type>
{};
UAB_INTERNAL_NS_END

/*!
 * @brief The type of element added when building a sequence, as a member
 * type named type.
 *
 * Detected: sequence_value<T>'s type, except for map-like types, whose element
 * type std::pair<const K, V> can't be assigned. A type is map-like when it has
 * key_type and mapped_type members and its element type is exactly
 * std::pair<const key_type, mapped_type>; it's built from
 * std::pair<key_type, mapped_type>.
 */
template <typename T>
struct sequence_build_value : UAB_INTERNAL_NS::detected_sequence_build_value<T>
{};

/*!
 * @brief The type of element added when building sequence T. See
 * sequence_build_value.
 */
template <typename T>
using sequence_build_value_t = typename sequence_build_value<T>::type;

// ============================================================ elements

/*!
 * @brief The elements of a sequence, in order, as something a range-for can
 * iterate: static view(const T&).
 *
 * Default: the object itself. Specialise for a type that holds a sequence but
 * isn't iterable, returning an iterable view of it.
 */
template <typename T>
struct sequence_elements
{
    static constexpr const T&
        view(
            const T& object_arg
        ) noexcept
    {
        return object_arg;
    }
};

// ============================================================ fixed count

UAB_INTERNAL_NS_BEGIN
// No fixed count, unless T is a built-in array.
template <typename T>
struct detected_sequence_fixed_count
{};

template <typename T>
requires std::is_bounded_array_v<T>
struct detected_sequence_fixed_count<T>
{
    static constexpr std::size_t value = std::extent_v<T>;
};
UAB_INTERNAL_NS_END

/*!
 * @brief The number of elements in a sequence whose length is part of its
 * type: static constexpr std::size_t value. Absent when the length varies.
 *
 * Detected: the extent of a built-in array; otherwise absent. Specialised in
 * support/std/ for std::array and std::bitset.
 */
template <typename T>
struct sequence_fixed_count : UAB_INTERNAL_NS::detected_sequence_fixed_count<T>
{};

// ============================================================ concepts

/*!
 * @brief Satisfied when T is a sequence: it has an element type, and its
 * elements view can be iterated by a range-for.
 *
 * Being a sequence doesn't mean T can be built; see buildable_sequence_c in
 * appenders.hpp.
 *
 * @tparam T The type to check, unqualified.
 */
template <typename T>
concept sequence_c = requires (const T& object_arg) {
    typename sequence_value<T>::type;
    sequence_elements<T>::view(object_arg);
    requires iterable_c<
        std::remove_cvref_t<decltype(sequence_elements<T>::view(object_arg))>>;
};

/*!
 * @brief Satisfied when sequence_fixed_count<T> has a std::size_t value: T's
 * length is part of its type.
 *
 * @tparam T The type to check, unqualified.
 */
template <typename T>
concept fixed_count_sequence_c = requires {
    sequence_fixed_count<T>::value;
    requires std::is_same_v<
        std::remove_cv_t<decltype(sequence_fixed_count<T>::value)>,
        std::size_t>;
};

/*!
 * @brief The number of elements in T, when its length is part of its type.
 */
template <typename T>
requires fixed_count_sequence_c<T>
inline constexpr std::size_t sequence_fixed_count_v
    = sequence_fixed_count<T>::value;

UAB_NS_END

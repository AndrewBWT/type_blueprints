#pragma once
/*!
 * @file
 * @brief Objects: types made of fields, described by object_traits.
 *
 * Users describe an object once, with object_traits<T>::to_tuple. Consumers
 * then see it through:
 * - to_tuple(object): its fields as a std::tuple, usually of references
 *   into the object (std::tie).
 * - to_value_tuple(object): copies of its fields, as object_values_t<T>.
 *   Safe on a temporary.
 * - from_tuple<T>(values): a T built from values of its fields, with
 *   std::make_from_tuple.
 *
 * Printing needs only to_tuple. Parsing and generation build the values
 * first, then the object.
 */
#include "type_blueprints/core/extended_concepts.hpp"
#include "type_blueprints/core/internal/macros.hpp"

// std::tuple, std::make_from_tuple.
#include <tuple>
// std::remove_cvref_t, std::is_array_v, std::is_constructible_v,
// std::is_reference_v.
#include <type_traits>
// std::declval, std::move.
#include <utility>

UAB_NS_BEGIN

/*!
 * @brief Describes T as an object: a type made of fields.
 *
 * The primary template is empty, so no type is an object until object_traits
 * is specialised for it. A specialisation provides static constexpr
 * to_tuple(const T&), returning T's fields as a std::tuple, in the order T's
 * constructor takes them (from_tuple builds T in that order).
 *
 * Usually to_tuple returns references (std::tie). It may return values
 * instead, for fields only reachable through getters that return by value.
 *
 * @code
 * UAB_NS_BEGIN
 * template <>
 * struct object_traits<geo::point>
 * {
 *     static constexpr auto
 *         to_tuple(const geo::point& point_arg) noexcept
 *     {
 *         return std::tie(point_arg.x, point_arg.y);
 *     }
 * };
 * UAB_NS_END
 * @endcode
 */
template <typename T>
struct object_traits
{};

/*!
 * @brief Satisfied when object_traits<T> describes T as an object: it has a
 * static to_tuple(const T&) returning a std::tuple.
 *
 * T must be unqualified; object_c<const T> is false, as object_traits is
 * specialised for T itself.
 *
 * @tparam T The type to check.
 */
template <typename T>
concept object_c = requires (const T& object_arg) {
    object_traits<T>::to_tuple(object_arg);
    requires specialization_of_c<
        std::remove_cvref_t<decltype(object_traits<T>::to_tuple(object_arg))>,
        std::tuple>;
};

/*!
 * @brief The tuple object_traits<T>::to_tuple returns, such as
 * std::tuple<const int&, const int&> when it uses std::tie.
 */
template <typename T>
requires object_c<T>
using object_fields_t = std::remove_cvref_t<
    decltype(object_traits<T>::to_tuple(std::declval<const T&>()))>;

// Swap for your internal namespace macro.
UAB_INTERNAL_NS_BEGIN

/*!
 * @brief The values of a tuple of fields: each element with references,
 * const and volatile removed. Has no type member when a field is a built-in
 * array, which a std::tuple can't hold by value.
 */
template <typename Fields>
struct values_of
{};

template <typename... Fields>
requires (not std::is_array_v<std::remove_cvref_t<Fields>> && ...)
struct values_of<std::tuple<Fields...>>
{
    using type = std::tuple<std::remove_cvref_t<Fields>...>;
};

/*!
 * @brief Whether any element of a tuple of fields is a reference: whether a
 * tuple from to_tuple would dangle once the object is gone.
 */
template <typename Fields>
inline constexpr bool holds_references_v = false;

template <typename... Fields>
inline constexpr bool holds_references_v<std::tuple<Fields...>>
    = (std::is_reference_v<Fields> || ...);

/*!
 * @brief Whether std::make_from_tuple<T> can build a T from Values: whether T
 * is constructible from Values' elements, as rvalues, in order.
 */
template <typename T, typename Values>
inline constexpr bool constructible_from_values_v = false;

template <typename T, typename... Values>
inline constexpr bool constructible_from_values_v<T, std::tuple<Values...>>
    = std::is_constructible_v<T, Values&&...>;
UAB_INTERNAL_NS_END

/*!
 * @brief A std::tuple holding values of T's fields: object_fields_t<T> with
 * references, const and volatile removed from every element.
 *
 * Not defined when a field is a built-in array; such an object can be read
 * with to_tuple, but not copied with to_value_tuple or built with from_tuple.
 */
template <typename T>
requires object_c<T>
using object_values_t = typename UAB_INTERNAL_NS::values_of<object_fields_t<T>>::type;

/*!
 * @brief Satisfied when T is an object whose fields can be held as values:
 * object_values_t<T> exists (no field is a built-in array).
 *
 * @tparam T The type to check.
 */
template <typename T>
concept valued_object_c
    = object_c<T> && requires { typename object_values_t<T>; };

/*!
 * @brief Satisfied when from_tuple can build T from object_values_t<T>: T is
 * constructible from its values in to_tuple order (aggregates included,
 * through parenthesised initialisation).
 *
 * If T's constructor takes several fields of the same type in a different
 * order from to_tuple, this is still satisfied, and from_tuple builds T with
 * those fields swapped. Write to_tuple in the constructor's order.
 *
 * @tparam T The type to check.
 */
template <typename T>
concept buildable_object_c
    = valued_object_c<T>
      && UAB_INTERNAL_NS::constructible_from_values_v<T, object_values_t<T>>;

/*!
 * @brief T's fields, as object_traits<T>::to_tuple returns them.
 *
 * noexcept exactly when object_traits<T>::to_tuple is.
 *
 * @tparam T The object's type.
 * @param object_arg The object.
 * @return The fields as a std::tuple; references into object_arg when
 * to_tuple uses std::tie.
 */
template <typename T>
requires object_c<T>
constexpr object_fields_t<T>
    to_tuple(
        const T& object_arg
    )
        noexcept(
            noexcept(object_traits<T>::to_tuple(object_arg))
        )
{
    return object_traits<T>::to_tuple(object_arg);
}

/*!
 * @brief Not for a temporary when the fields are references: they would
 * refer into the temporary after it's destroyed. Use to_value_tuple.
 *
 * Only deleted when to_tuple returns at least one reference; when it returns
 * only values, to_tuple on a temporary is safe and allowed.
 */
template <typename T>
requires object_c<T> && UAB_INTERNAL_NS::holds_references_v<object_fields_t<T>>
void
    to_tuple(const T&&)
    = delete;

/*!
 * @brief Copies of T's fields, as object_values_t<T>.
 *
 * Made from to_tuple, so it always agrees with it. Safe on a temporary: the
 * fields are copied before the temporary is destroyed.
 *
 * @tparam T The object's type.
 * @param object_arg The object.
 * @return The fields' values.
 */
template <typename T>
requires valued_object_c<T>
constexpr object_values_t<T>
    to_value_tuple(
        const T& object_arg
    )
{
    return object_values_t<T>(object_traits<T>::to_tuple(object_arg));
}

/*!
 * @brief Builds a T from values of its fields, with std::make_from_tuple.
 *
 * Takes the values by value: an lvalue tuple is copied in, an rvalue tuple
 * moved in. T can't be deduced, so call it as from_tuple<T>(values).
 *
 * @tparam T The object's type.
 * @param values_arg The fields' values, in to_tuple order.
 * @return The object.
 */
template <typename T>
requires buildable_object_c<T>
constexpr T
    from_tuple(
        object_values_t<T> values_arg
    )
{
    return std::make_from_tuple<T>(std::move(values_arg));
}

UAB_NS_END
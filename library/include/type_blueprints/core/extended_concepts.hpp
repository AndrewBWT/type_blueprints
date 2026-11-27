#pragma once

#include "type_blueprints/core/internal/macros.hpp"
// std::convertible_to and std::same_as.
#include <concepts>
// std::size_t.
#include <cstddef>
// std::is_pointer_v, std::remove_cv_t, std::remove_pointer_t.
#include <type_traits>


TYPE_BLUEPRINTS_NS_BEGIN
/*!
 * @brief Describes a contiguous container. Requires:
 * - member type "value_type".
 * - Function data(), which returns a pointer. Return type, after stripping
 * pointer and cv, must be equal to value_type.
 * - Function size(), which returns the size of the container.
 *
 * Designed to match std::vector, std::array, std::string, std::span etc.
 *
 * Does not match to built in C arrays.
 *
 * A stand in for std::ranges::contiguous_range &&
 * std::ranges::sized_range.
 */
template <typename Container>
concept contiguous_container_c = requires (const Container& container_arg) {
    typename Container::value_type;
    requires std::is_pointer_v<decltype(container_arg.data())>;
    requires std::same_as<
        std::remove_cv_t<std::remove_pointer_t<decltype(container_arg.data())>>,
        typename Container::value_type>;
    { container_arg.size() } -> std::convertible_to<std::size_t>;
};
/*!
 * @brief Checks that Container is a contiguous container (see
 * contiguous_container_c), and that its value_type is equal to T.
 *
 *
 * @tparam Container The type to check.
 * @tparam T The element type, unqualified.
 */
template <typename Container, typename T>
concept contiguous_container_of_c
    = contiguous_container_c<Container>
      && std::same_as<typename Container::value_type, T>;
TYPE_BLUEPRINTS_INTERNAL_NS_BEGIN
/*!
 * @brief Whether T is a specialization of the class template Template: true
 * for is_specialization_of_v<std::tuple<int, char>, std::tuple>, false for
 * is_specialization_of_v<std::pair<int, char>, std::tuple>.
 *
 * This primary template gives false; the partial specialisation below gives
 * true when T has the form Template<Ts...>.
 *
 * Only templates whose parameters are all types can be matched:
 * std::array<int, 3> isn't recognised as a specialization of std::array,
 * because its size is a value. cv-qualifiers and references aren't removed,
 * so is_specialization_of_v<const std::tuple<>, std::tuple> is false; apply
 * std::remove_cvref_t first if needed.
 *
 * @tparam T The type to check.
 * @tparam Template The class template to check against.
 */
template <typename T, template <typename...> typename Template>
inline constexpr bool is_specialization_of_v = false;

/*!
 * @brief True when T has the form Template<Ts...>: T is a specialization of
 * Template.
 *
 * Matching deduces Template and Ts from T. Default template arguments are
 * part of Ts, so std::vector<int> matches std::vector.
 *
 * @tparam Template The class template.
 * @tparam Ts The template arguments T was formed with.
 */
template <template <typename...> typename Template, typename... Ts>
inline constexpr bool is_specialization_of_v<Template<Ts...>, Template> = true;
TYPE_BLUEPRINTS_INTERNAL_NS_END
// public
/*!
 * @brief Satisfied when T is a specialisation of Template, such as
 * specialization_of_c<std::tuple<int>, std::tuple>.
 *
 * Only matches templates whose parameters are all types: std::array<T, N>
 * can't be matched, because N is a value. cv-qualifiers and references are
 * not removed.
 */
template <typename T, template <typename...> typename Template>
concept specialization_of_c = TYPE_BLUEPRINTS_INTERNAL_NS::is_specialization_of_v<T, Template>;
/*!
 * @brief Satisfied when a range-for can iterate a const T, by the rules a
 * range-for uses: T is a built-in array of known bound; or T has begin() and
 * end() members; or begin(t) and end(t) are found by argument-dependent
 * lookup (as for std::valarray).
 *
 * @tparam T The type to check.
 */
template <typename T>
concept iterable_c
    = std::is_bounded_array_v<T> || requires (const T& object_arg) {
          object_arg.begin();
          object_arg.end();
      } || requires (const T& object_arg) {
          begin(object_arg);
          end(object_arg);
      };
TYPE_BLUEPRINTS_NS_END
#pragma once
/*!
 * @file
 * @brief Building a whole sequence from the elements of another, in one call.
 *
 * For when all the elements are already to hand (rebuilding a container from
 * shrunk elements, or in tests). Code that produces elements one at a time,
 * or decides for itself what each append_result means, drives
 * default_appender directly instead.
 */
#include "unpack_and_build/core/appenders.hpp"
#include "unpack_and_build/core/internal/macros.hpp"
#include "unpack_and_build/core/placeholders.hpp"
#include "unpack_and_build/core/sequence_traits.hpp"

 // std::nullopt, std::optional.
#include <optional>
// std::is_array_v, std::is_constructible_v.
#include <type_traits>

UAB_NS_BEGIN

/*!
 * @brief Satisfied when build_from<T> can build a T from a Source: T is
 * buildable and has a placeholder (and isn't a built-in array, which can't be
 * returned); Source is a sequence; and T's build type can be constructed from
 * Source's elements.
 *
 * @tparam T The sequence to build, unqualified.
 * @tparam Source The sequence the elements come from, unqualified.
 */
    template <typename T, typename Source>
concept buildable_from_c
= buildable_sequence_c<T> && placeholder_constructible_c<T>
&& (not std::is_array_v<T>) && sequence_c<Source>
&& std::is_constructible_v<
    sequence_build_value_t<T>,
    const sequence_value_t<Source>&>;

/*!
 * @brief Builds a T holding exactly source_arg's elements, in order, or
 * std::nullopt when it can't.
 *
 * Starts from make_placeholder<T>(), then gives each element, copied as T's
 * build type, to default_appender<T>. Strict: it fails if any element isn't
 * added (a duplicate key in a unique container, or too many for a fixed-length
 * one), or if too few are given for a fixed-length one. So a result always
 * holds every element of source_arg.
 *
 * The elements are read through sequence_elements, so any sequence works as a
 * source, std::bitset included.
 *
 * @tparam T The sequence to build.
 * @tparam Source The sequence the elements come from.
 * @param source_arg The elements.
 * @return The built sequence, or std::nullopt.
 */
template <typename T, typename Source>
    requires buildable_from_c<T, Source>
constexpr std::optional<T>
build_from(
    const Source& source_arg
)
{
    T                   result = make_placeholder<T>();
    default_appender<T> appender(result);
    for (const auto& element : sequence_elements<Source>::view(source_arg))
    {
        if (appender(sequence_build_value_t<T>(element)) != append_result::added)
        {
            return std::nullopt;
        }
    }
    appender.finish();
    if (not appender.complete())
    {
        return std::nullopt;
    }
    return result;
}

UAB_NS_END

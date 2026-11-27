#pragma once
/*!
 * @file
 * @brief std::variant: a placeholder when its first alternative has no default
 * constructor (otherwise std::variant<T, Ts...>{} already works).
 *
 * Specialises placeholder_value.
 */
#include "unpack_and_build/core/internal/macros.hpp"
#include "unpack_and_build/core/placeholders.hpp"

 // std::is_default_constructible_v.
#include <type_traits>
// std::in_place_index.
#include <utility>
// std::variant.
#include <variant>

UAB_NS_BEGIN

// Holds its first alternative's placeholder, as a default-constructed variant
// holds a default-constructed first alternative. Only the first alternative
// is tried, so a variant whose first alternative has no placeholder gets none.
template <typename T, typename... Ts>
    requires (not std::is_default_constructible_v<std::variant<T, Ts...>>)
&& placeholder_constructible_c<T>
struct placeholder_value<std::variant<T, Ts...>>
{
    static constexpr std::variant<T, Ts...>
        value()
    {
        return std::variant<T, Ts...>(
            std::in_place_index<0>, make_placeholder<T>()
        );
    }
};

UAB_NS_END

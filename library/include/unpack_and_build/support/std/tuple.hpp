#pragma once
/*!
 * @file
 * @brief std::tuple: a placeholder when an element has no default constructor
 * (otherwise std::tuple<Ts...>{} already works).
 *
 * Specialises placeholder_value.
 */
#include "unpack_and_build/core/internal/macros.hpp"
#include "unpack_and_build/core/placeholders.hpp"

 // std::tuple.
#include <tuple>
// std::is_default_constructible_v.
#include <type_traits>

UAB_NS_BEGIN

// Each element is made from its own placeholder.
template <typename... Ts>
    requires (not std::is_default_constructible_v<std::tuple<Ts...>>)
&& (placeholder_constructible_c<Ts> && ...)
struct placeholder_value<std::tuple<Ts...>>
{
    static constexpr std::tuple<Ts...>
        value()
    {
        return std::tuple<Ts...>(make_placeholder<Ts>()...);
    }
};

UAB_NS_END

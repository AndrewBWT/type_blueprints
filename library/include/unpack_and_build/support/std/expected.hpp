#pragma once
/*!
 * @file
 * @brief std::expected: a placeholder when its value type has no default
 * constructor (otherwise std::expected<T, E>{} already works).
 *
 * Specialises placeholder_value.
 */
#include "unpack_and_build/core/internal/macros.hpp"
#include "unpack_and_build/core/placeholders.hpp"

 // std::expected.
#include <expected>
// std::is_default_constructible_v.
#include <type_traits>
// std::in_place.
#include <utility>

UAB_NS_BEGIN

// Holds its value type's placeholder, as a default-constructed one would hold
// a default-constructed value.
template <typename T, typename E>
    requires (not std::is_default_constructible_v<std::expected<T, E>>)
&& placeholder_constructible_c<T>
struct placeholder_value<std::expected<T, E>>
{
    static constexpr std::expected<T, E>
        value()
    {
        return std::expected<T, E>(
            std::in_place, make_placeholder<T>()
        );
    }
};

UAB_NS_END

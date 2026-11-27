#pragma once
/*!
 * @file
 * @brief std::pair: a placeholder when an element has no default constructor
 * (otherwise std::pair<T1, T2>{} already works).
 *
 * Specialises placeholder_value.
 */
#include "unpack_and_build/core/internal/macros.hpp"
#include "unpack_and_build/core/placeholders.hpp"

 // std::is_default_constructible_v.
#include <type_traits>
// std::pair.
#include <utility>

UAB_NS_BEGIN

// Each element is made from its own placeholder.
template <typename T1, typename T2>
    requires (not std::is_default_constructible_v<std::pair<T1, T2>>)
&& placeholder_constructible_c<T1>&& placeholder_constructible_c<T2>
struct placeholder_value<std::pair<T1, T2>>
{
    static constexpr std::pair<T1, T2>
        value()
    {
        return std::pair<T1, T2>(
            make_placeholder<T1>(), make_placeholder<T2>()
        );
    }
};

UAB_NS_END

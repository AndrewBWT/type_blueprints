#pragma once
/*!
 * @file
 * @brief std::array: a sequence with a fixed count, filled by index.
 *
 * Its element type is detected (it's iterable, with a value_type); this
 * header specialises sequence_fixed_count, default_appender, and
 * placeholder_value (for elements with no default constructor). Include it
 * wherever those traits are used on a std::array (see sequence_traits.hpp).
 */
#include "unpack_and_build/core/appenders.hpp"
#include "unpack_and_build/core/appenders/fixed_size.hpp"
#include "unpack_and_build/core/internal/macros.hpp"
#include "unpack_and_build/core/sequence_traits.hpp"
#include "unpack_and_build/core/placeholders.hpp"

 // std::array.
#include <array>
// std::size_t.
#include <cstddef>
// std::is_array_v, std::is_default_constructible_v.
#include <type_traits>
// std::index_sequence, std::make_index_sequence.
#include <utility>

UAB_NS_BEGIN

template <typename T, std::size_t N>
struct sequence_fixed_count<std::array<T, N>>
{
    static constexpr std::size_t value = N;
};

template <typename T, std::size_t N>
struct default_appender<std::array<T, N>> : fixed_size_appender<std::array<T, N>>
{
    using fixed_size_appender<std::array<T, N>>::fixed_size_appender;
};

// Only for elements with no default constructor: otherwise std::array<T, N>{}
// already works. Each element is made from its own placeholder.
template <typename T, std::size_t N>
    requires (not std::is_default_constructible_v<T>)
&& (not std::is_array_v<T>) && placeholder_constructible_c<T>
struct placeholder_value<std::array<T, N>>
{
    static constexpr std::array<T, N>
        value()
    {
        return[]<std::size_t... Indices>(std::index_sequence<Indices...>)
        {
            return std::array<T, N>{
                (static_cast<void>(Indices), make_placeholder<T>())...
            };
        }(std::make_index_sequence<N>{});
    }
};

UAB_NS_END

#pragma once
/*!
 * @file
 * @brief The appender for sequences of a fixed length: built-in arrays and
 * std::array.
 */

// append_result.
#include "type_blueprints/core/appenders/append_result_enum.hpp"
// Namespace macros.
#include "type_blueprints/core/internal/macros.hpp"
// sequence_build_value_t, fixed_count_sequence_c, sequence_fixed_count_v.
#include "type_blueprints/core/sequence_traits.hpp"

// std::size_t.
#include <cstddef>
// std::move.
#include <utility>

UAB_NS_BEGIN
// Whether T isn't an array and can be move-assigned.
template <typename T>
concept move_assignable_value_c
    = (not std::is_array_v<T>) && std::is_move_assignable_v<T>;

// Whether T is a built-in array (of any depth) whose innermost elements can
// be move-assigned. remove_all_extents_t strips every level at once, as a
// concept can't recurse into itself.
template <typename T>
concept move_assignable_array_c
    = std::is_bounded_array_v<T>
      && move_assignable_value_c<std::remove_all_extents_t<T>>;

UAB_INTERNAL_NS_BEGIN
template <typename T>
requires move_assignable_array_c<T> || move_assignable_value_c<T>
constexpr void
    move_assign(
        T&  destination_arg,
        T&& source_arg
    )
{
    if constexpr (move_assignable_array_c<T>)
    {
        for (std::size_t idx = 0; idx < std::extent_v<T>; ++idx)
        {
            move_assign(destination_arg[idx], std::move(source_arg[idx]));
        }
    }
    else if constexpr (move_assignable_value_c<T>)
    {
        destination_arg = std::move(source_arg);
    }
    else
    {
        static_assert(
            false,
            "move_assign: T is neither move-assignable nor an array of "
            "move-assignable elements"
        );
    }
}

UAB_INTERNAL_NS_END

/*!
 * @brief Fills a fixed-length sequence by index, from the first element.
 *
 * Reports full once every element has been filled; complete() is true only
 * when every element has been filled.
 *
 * @tparam T The sequence. Its elements must be reachable with operator[].
 */
template <typename T>
requires fixed_count_sequence_c<T>
class fixed_size_appender
{
public:
    constexpr explicit fixed_size_appender(
        T& container_arg
    ) noexcept
        : _container(&container_arg)
    {}

    constexpr append_result
        operator()(
            sequence_build_value_t<T>&& value_arg
        )
    {
        if (_count == count)
        {
            return append_result::full;
        }
        UAB_INTERNAL_NS::move_assign(
            (*_container)[_count], std::move(value_arg)
        );
        ++_count;
        return append_result::added;
    }

    // Whether every element has been filled.
    [[nodiscard]]
    constexpr bool
        complete() const noexcept
    {
        return _count == count;
    }

    // Nothing to do: each element is written as it's appended.
    constexpr void
        finish() noexcept
    {}
private:
    static constexpr std::size_t count = sequence_fixed_count_v<T>;
    T*                           _container;
    std::size_t                  _count = 0;
};

UAB_NS_END

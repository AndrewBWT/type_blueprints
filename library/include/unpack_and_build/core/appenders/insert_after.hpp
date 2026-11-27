#pragma once
/*!
 * @file
 * @brief The appender for singly linked lists, such as std::forward_list
 * (support/std/forward_list.hpp uses it).
 */

 // append_result.
#include "unpack_and_build/core/appenders/append_result_enum.hpp"
// Namespace macros.
#include "unpack_and_build/core/internal/macros.hpp"
// sequence_build_value_t.
#include "unpack_and_build/core/sequence_traits.hpp"

// std::move.
#include <utility>

UAB_NS_BEGIN

/*!
 * @brief Adds elements at the end with insert_after, so they keep the order
 * they're given in. Every element is added.
 *
 * Construction walks to the container's last element, so elements already
 * there stay in front.
 *
 * @tparam T The list. Needs before_begin(), begin(), end(), insert_after
 * and an iterator member type.
 */
    template <typename T>
class insert_after_appender
{
public:
    constexpr explicit insert_after_appender(
        T& container_arg
    )
        : _container(&container_arg), _last(container_arg.before_begin())
    {
        for (auto it = container_arg.begin(); it != container_arg.end(); ++it)
        {
            _last = it;
        }
    }

    constexpr append_result
        operator()(
            sequence_build_value_t<T>&& value_arg
            )
    {
        _last = _container->insert_after(_last, std::move(value_arg));
        return append_result::added;
    }

    // Always true: any number of elements is a whole list.
    [[nodiscard]]
    constexpr bool
        complete() const noexcept
    {
        return true;
    }

    // Nothing to do: each element is added as it's appended.
    constexpr void
        finish() noexcept
    {
    }
private:
    T* _container;
    // The element to insert after: before_begin() while the list is empty.
    typename T::iterator _last;
};

UAB_NS_END

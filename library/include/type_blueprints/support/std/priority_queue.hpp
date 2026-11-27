#pragma once
/*!
 * @file
 * @brief std::priority_queue as an adaptor. Its container holds the elements
 * in heap order. Include this wherever adaptor_traits is used on a
 * std::priority_queue (see sequence_traits.hpp).
 */
#include "type_blueprints/core/adaptors.hpp"
#include "type_blueprints/core/internal/macros.hpp"

 // std::priority_queue (in <queue>).
#include <queue>
// std::is_default_constructible_v.
#include <type_traits>
// std::move.
#include <utility>

UAB_NS_BEGIN

template <typename T, typename Container, typename Compare>
struct adaptor_traits<std::priority_queue<T, Container, Compare>>
{
    using container_type = Container;
    using comparator_type = Compare;

    static constexpr const container_type&
        underlying(
            const std::priority_queue<T, Container, Compare>& queue_arg
        ) noexcept
    {
        return UAB_INTERNAL_NS::standard_adaptor_access<
            std::priority_queue<T, Container, Compare>>::container(queue_arg);
    }

    /*!
     * @brief Builds the queue with a default-constructed comparator. Only
     * available when the comparator can be default-constructed.
     */
    static constexpr std::priority_queue<T, Container, Compare>
        from_container(
            container_type&& container_arg
        )
        requires std::is_default_constructible_v<Compare>
    {
        return std::priority_queue<T, Container, Compare>(
            Compare{}, std::move(container_arg)
        );
    }

    /*!
     * @brief Builds the queue with the given comparator.
     */
    static constexpr std::priority_queue<T, Container, Compare>
        from_container(
            container_type&& container_arg,
            const comparator_type& comparator_arg
        )
    {
        return std::priority_queue<T, Container, Compare>(
            comparator_arg, std::move(container_arg)
        );
    }
};

UAB_NS_END

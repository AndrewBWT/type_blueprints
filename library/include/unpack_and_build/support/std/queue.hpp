#pragma once
/*!
 * @file
 * @brief std::queue as an adaptor. Its container holds the elements front to
 * back. Include this wherever adaptor_traits is used on a std::queue (see
 * sequence_traits.hpp).
 */
#include "unpack_and_build/core/adaptors.hpp"
#include "unpack_and_build/core/internal/macros.hpp"

 // std::queue.
#include <queue>
// std::move.
#include <utility>

UAB_NS_BEGIN

template <typename T, typename Container>
struct adaptor_traits<std::queue<T, Container>>
{
    using container_type = Container;

    static constexpr const container_type&
        underlying(
            const std::queue<T, Container>& queue_arg
        ) noexcept
    {
        return UAB_INTERNAL_NS::standard_adaptor_access<
            std::queue<T, Container>>::container(queue_arg);
    }

    static constexpr std::queue<T, Container>
        from_container(
            container_type&& container_arg
        )
    {
        return std::queue<T, Container>(std::move(container_arg));
    }
};

UAB_NS_END

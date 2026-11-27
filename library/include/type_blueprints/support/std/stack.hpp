#pragma once
/*!
 * @file
 * @brief std::stack as an adaptor. Its container holds the elements bottom to
 * top. Include this wherever adaptor_traits is used on a std::stack (see
 * sequence_traits.hpp).
 */
#include "type_blueprints/core/adaptors.hpp"
#include "type_blueprints/core/internal/macros.hpp"

 // std::stack.
#include <stack>
// std::move.
#include <utility>

UAB_NS_BEGIN

template <typename T, typename Container>
struct adaptor_traits<std::stack<T, Container>>
{
    using container_type = Container;

    static constexpr const container_type&
        underlying(
            const std::stack<T, Container>& stack_arg
        ) noexcept
    {
        return UAB_INTERNAL_NS::standard_adaptor_access<
            std::stack<T, Container>>::container(stack_arg);
    }

    static constexpr std::stack<T, Container>
        from_container(
            container_type&& container_arg
        )
    {
        return std::stack<T, Container>(std::move(container_arg));
    }
};

UAB_NS_END

#pragma once
/*!
 * @file
 * @brief Adaptors: types that wrap a container, described by adaptor_traits.
 *
 * Consumers handle an adaptor through its container: read it with
 * adaptor_traits<A>::underlying, and build an adaptor from one with
 * adaptor_traits<A>::from_container.
 *
 * Nothing is detected: the standard adaptors (std::stack, std::queue,
 * std::priority_queue) are specialised in support/std/, and other adaptors
 * specialise adaptor_traits themselves.
 */
#include "unpack_and_build/core/internal/macros.hpp"

// std::is_same_v.
#include <type_traits>
// std::move.
#include <utility>

UAB_NS_BEGIN

/*!
 * @brief Describes A as an adaptor: a type that wraps a container.
 *
 * The primary template is empty, so no type is an adaptor until
 * adaptor_traits is specialised for it. A specialisation needs:
 * - container_type, the wrapped container.
 * - static constexpr underlying(const A&), returning a
 *   const container_type&.
 * - static constexpr from_container(container_type&&), returning an A.
 *
 * It may add more, such as std::priority_queue's from_container taking a
 * comparator as well.
 */
template <typename A>
struct adaptor_traits
{};

/*!
 * @brief Satisfied when adaptor_traits<A> describes A as an adaptor: it has a
 * container_type, underlying(const A&) returning a const container_type&,
 * and from_container(container_type&&) returning an A.
 *
 * @tparam A The type to check, unqualified.
 */
template <typename A>
concept adaptor_c = requires (
    const A&                                     adaptor_arg,
    typename adaptor_traits<A>::container_type&& container_arg
) {
    requires std::is_same_v<
        decltype(adaptor_traits<A>::underlying(adaptor_arg)),
        const typename adaptor_traits<A>::container_type&>;
    requires std::is_same_v<
        decltype(adaptor_traits<A>::from_container(std::move(container_arg))),
        A>;
};

/*!
 * @brief The container adaptor A wraps.
 */
template <typename A>
requires adaptor_c<A>
using adaptor_container_t = typename adaptor_traits<A>::container_type;

UAB_INTERNAL_NS_BEGIN

/*!
 * @brief Reaches the protected container member c of a standard adaptor
 * (std::stack, std::queue and std::priority_queue all declare it). A derived
 * class may form a pointer to a protected member it inherits, and that
 * pointer can then be applied to any A. Used by support/std/.
 */
template <typename A>
struct standard_adaptor_access : A
{
    static constexpr const typename A::container_type&
        container(
            const A& adaptor_arg
        ) noexcept
    {
        return adaptor_arg.*(&standard_adaptor_access::c);
    }
};

UAB_INTERNAL_NS_END

UAB_NS_END

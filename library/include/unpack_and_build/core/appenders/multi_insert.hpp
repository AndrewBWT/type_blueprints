#pragma once
/*!
 * @file
 * @brief The appender for multi-key associative containers: std::multiset,
 * std::multimap, std::unordered_multiset and std::unordered_multimap.
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
 * @brief Adds elements with insert(value). Every element is added, even when
 * an equivalent one is already there.
 *
 * @tparam T The container.
 */
    template <typename T>
class multi_insert_appender
{
public:
    constexpr explicit multi_insert_appender(
        T& container_arg
    ) noexcept
        : _container(&container_arg)
    {
    }

    constexpr append_result
        operator()(
            sequence_build_value_t<T>&& value_arg
            )
    {
        _container->insert(std::move(value_arg));
        return append_result::added;
    }

    // Always true: any number of elements is a whole container.
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
};

UAB_NS_END

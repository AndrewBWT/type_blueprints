#pragma once
/*!
 * @file
 * @brief The appender for sequences with push_back: std::vector, std::deque,
 * std::list, std::basic_string and similar.
 */

 // append_result.
#include "type_blueprints/core/appenders/append_result_enum.hpp"
// Namespace macros.
#include "type_blueprints/core/internal/macros.hpp"
// sequence_build_value_t.
#include "type_blueprints/core/sequence_traits.hpp"

// std::move.
#include <utility>

UAB_NS_BEGIN

/*!
 * @brief Adds elements at the end with push_back. Every element is added.
 *
 * @tparam T The sequence.
 */
    template <typename T>
class push_back_appender
{
public:
    constexpr explicit push_back_appender(
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
        _container->push_back(std::move(value_arg));
        return append_result::added;
    }

    // Always true: any number of elements is a whole sequence.
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

#pragma once
/*!
 * @file
 * @brief The appender for unique-key associative containers: std::set,
 * std::map, std::unordered_set and std::unordered_map.
 */

 // append_result.
#include "type_blueprints/core/appenders/append_result_enum.hpp"
// Namespace macros.
#include "type_blueprints/core/internal/macros.hpp"
// sequence_build_value_t.
#include "type_blueprints/core/sequence_traits.hpp"

// std::move.
#include <utility>

TYPE_BLUEPRINTS_NS_BEGIN

/*!
 * @brief Adds elements with insert(value), which returns a pair whose second
 * member says whether the element was added.
 *
 * Reports already_present when an element with an equivalent key is already
 * there, and keeps the existing one.
 *
 * @tparam T The container.
 */
    template <typename T>
class unique_insert_appender
{
public:
    constexpr explicit unique_insert_appender(
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
        if (not _container->insert(std::move(value_arg)).second)
        {
            return append_result::already_present;
        }
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

TYPE_BLUEPRINTS_NS_END

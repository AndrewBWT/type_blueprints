#pragma once
/*!
 * @file
 * @brief std::unexpected: a placeholder, as it has no default constructor.
 *
 * Specialises placeholder_value.
 */
#include "type_blueprints/core/internal/macros.hpp"
#include "type_blueprints/core/placeholders.hpp"

 // std::unexpected.
#include <expected>

UAB_NS_BEGIN

// Holds its error's placeholder.
template <typename E>
    requires placeholder_constructible_c<E>
struct placeholder_value<std::unexpected<E>>
{
    static constexpr std::unexpected<E>
        value()
    {
        return std::unexpected<E>(make_placeholder<E>());
    }
};

UAB_NS_END

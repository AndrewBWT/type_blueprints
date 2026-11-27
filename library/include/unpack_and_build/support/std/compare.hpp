#pragma once
/*!
 * @file
 * @brief The comparison categories, std::strong_ordering, std::weak_ordering
 * and std::partial_ordering: placeholders, as they have no default
 * constructor.
 *
 * Specialises placeholder_value.
 */
#include "unpack_and_build/core/internal/macros.hpp"
#include "unpack_and_build/core/placeholders.hpp"

 // std::strong_ordering, std::weak_ordering, std::partial_ordering.
#include <compare>

UAB_NS_BEGIN

template <>
struct placeholder_value<std::strong_ordering>
{
    static constexpr std::strong_ordering
        value() noexcept
    {
        return std::strong_ordering::equal;
    }
};

template <>
struct placeholder_value<std::weak_ordering>
{
    static constexpr std::weak_ordering
        value() noexcept
    {
        return std::weak_ordering::equivalent;
    }
};

template <>
struct placeholder_value<std::partial_ordering>
{
    static constexpr std::partial_ordering
        value() noexcept
    {
        return std::partial_ordering::equivalent;
    }
};

UAB_NS_END

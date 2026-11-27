#pragma once
/*!
 * @file
 * @brief Built-in arrays: filled by index.
 *
 * Specialises default_appender. Their sequence traits, including the fixed
 * count, are detected.
 */
#include "unpack_and_build/core/appenders.hpp"
#include "unpack_and_build/core/appenders/fixed_size.hpp"
#include "unpack_and_build/core/internal/macros.hpp"

 // std::size_t.
#include <cstddef>

UAB_NS_BEGIN

template <typename T, std::size_t N>
struct default_appender<T[N]> : fixed_size_appender<T[N]>
{
    using fixed_size_appender<T[N]>::fixed_size_appender;
};

UAB_NS_END

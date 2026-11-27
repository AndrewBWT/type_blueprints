#pragma once
/*!
 * @file
 * @brief std::vector: built with push_back.
 *
 * Specialises default_appender. Its sequence traits are detected (it's
 * iterable, with a value_type).
 */
#include "unpack_and_build/core/appenders.hpp"
#include "unpack_and_build/core/appenders/push_back.hpp"
#include "unpack_and_build/core/internal/macros.hpp"

 // std::vector.
#include <vector>

UAB_NS_BEGIN

template <typename T, typename Allocator>
struct default_appender<std::vector<T, Allocator>>
    : push_back_appender<std::vector<T, Allocator>>
{
    using push_back_appender<std::vector<T, Allocator>>::push_back_appender;
};

UAB_NS_END

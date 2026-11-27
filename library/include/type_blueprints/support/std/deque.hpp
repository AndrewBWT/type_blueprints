#pragma once
/*!
 * @file
 * @brief std::deque: built with push_back.
 *
 * Specialises default_appender. Its sequence traits are detected (it's
 * iterable, with a value_type).
 */
#include "type_blueprints/core/appenders.hpp"
#include "type_blueprints/core/appenders/push_back.hpp"
#include "type_blueprints/core/internal/macros.hpp"

 // std::deque.
#include <deque>

UAB_NS_BEGIN

template <typename T, typename Allocator>
struct default_appender<std::deque<T, Allocator>>
    : push_back_appender<std::deque<T, Allocator>>
{
    using push_back_appender<std::deque<T, Allocator>>::push_back_appender;
};

UAB_NS_END

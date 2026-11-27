#pragma once
/*!
 * @file
 * @brief std::forward_list: built with insert_after, keeping the order elements
 * are given in.
 *
 * Specialises default_appender. Its sequence traits are detected (it's
 * iterable, with a value_type).
 */
#include "type_blueprints/core/appenders.hpp"
#include "type_blueprints/core/appenders/insert_after.hpp"
#include "type_blueprints/core/internal/macros.hpp"

 // std::forward_list.
#include <forward_list>

TYPE_BLUEPRINTS_NS_BEGIN

template <typename T, typename Allocator>
struct default_appender<std::forward_list<T, Allocator>>
    : insert_after_appender<std::forward_list<T, Allocator>>
{
    using insert_after_appender<std::forward_list<T, Allocator>>::insert_after_appender;
};

TYPE_BLUEPRINTS_NS_END

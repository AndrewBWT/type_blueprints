#pragma once
/*!
 * @file
 * @brief std::set: built with insert.
 *
 * Specialises default_appender. Its sequence traits are detected (it's
 * iterable, with a value_type).
 */
#include "type_blueprints/core/appenders.hpp"
#include "type_blueprints/core/appenders/unique_insert.hpp"
#include "type_blueprints/core/internal/macros.hpp"

 // std::set.
#include <set>

TYPE_BLUEPRINTS_NS_BEGIN

template <typename Key, typename Compare, typename Allocator>
struct default_appender<std::set<Key, Compare, Allocator>>
    : unique_insert_appender<std::set<Key, Compare, Allocator>>
{
    using unique_insert_appender<std::set<Key, Compare, Allocator>>::unique_insert_appender;
};

TYPE_BLUEPRINTS_NS_END

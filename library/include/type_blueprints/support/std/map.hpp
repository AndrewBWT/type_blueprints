#pragma once
/*!
 * @file
 * @brief std::map: built with insert, from std::pair<Key, T>.
 *
 * Specialises default_appender. Its sequence traits are detected (it's
 * iterable, with a value_type).
 */
#include "type_blueprints/core/appenders.hpp"
#include "type_blueprints/core/appenders/unique_insert.hpp"
#include "type_blueprints/core/internal/macros.hpp"

 // std::map.
#include <map>

UAB_NS_BEGIN

template <typename Key, typename T, typename Compare, typename Allocator>
struct default_appender<std::map<Key, T, Compare, Allocator>>
    : unique_insert_appender<std::map<Key, T, Compare, Allocator>>
{
    using unique_insert_appender<std::map<Key, T, Compare, Allocator>>::unique_insert_appender;
};

UAB_NS_END

#pragma once
/*!
 * @file
 * @brief std::unordered_map: built with insert, from std::pair<Key, T>.
 *
 * Specialises default_appender. Its sequence traits are detected (it's
 * iterable, with a value_type).
 */
#include "type_blueprints/core/appenders.hpp"
#include "type_blueprints/core/appenders/unique_insert.hpp"
#include "type_blueprints/core/internal/macros.hpp"

 // std::unordered_map.
#include <unordered_map>

UAB_NS_BEGIN

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
struct default_appender<std::unordered_map<Key, T, Hash, KeyEqual, Allocator>>
    : unique_insert_appender<std::unordered_map<Key, T, Hash, KeyEqual, Allocator>>
{
    using unique_insert_appender<std::unordered_map<Key, T, Hash, KeyEqual, Allocator>>::unique_insert_appender;
};

UAB_NS_END

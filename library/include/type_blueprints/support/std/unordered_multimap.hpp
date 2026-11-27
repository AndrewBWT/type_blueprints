#pragma once
/*!
 * @file
 * @brief std::unordered_multimap: built with insert, from std::pair<Key, T>.
 *
 * Specialises default_appender. Its sequence traits are detected (it's
 * iterable, with a value_type).
 */
#include "type_blueprints/core/appenders.hpp"
#include "type_blueprints/core/appenders/multi_insert.hpp"
#include "type_blueprints/core/internal/macros.hpp"

// std::unordered_multimap.
#include <unordered_map>

TYPE_BLUEPRINTS_NS_BEGIN

template <
    typename Key,
    typename T,
    typename Hash,
    typename KeyEqual,
    typename Allocator>
struct default_appender<
    std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>>
    : multi_insert_appender<
          std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>>
{
    using multi_insert_appender<
        std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>>::
        multi_insert_appender;
};

TYPE_BLUEPRINTS_NS_END

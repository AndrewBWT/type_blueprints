#pragma once
/*!
 * @file
 * @brief std::unordered_set: built with insert.
 *
 * Specialises default_appender. Its sequence traits are detected (it's
 * iterable, with a value_type).
 */
#include "type_blueprints/core/appenders.hpp"
#include "type_blueprints/core/appenders/unique_insert.hpp"
#include "type_blueprints/core/internal/macros.hpp"

// std::unordered_set.
#include <unordered_set>

TYPE_BLUEPRINTS_NS_BEGIN

template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
struct default_appender<std::unordered_set<Key, Hash, KeyEqual, Allocator>>
    : unique_insert_appender<std::unordered_set<Key, Hash, KeyEqual, Allocator>>
{
    using unique_insert_appender<
        std::unordered_set<Key, Hash, KeyEqual, Allocator>>::
        unique_insert_appender;
};

TYPE_BLUEPRINTS_NS_END

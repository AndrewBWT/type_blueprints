#pragma once
/*!
 * @file
 * @brief std::multiset: built with insert.
 *
 * Specialises default_appender. Its sequence traits are detected (it's
 * iterable, with a value_type).
 */
#include "type_blueprints/core/appenders.hpp"
#include "type_blueprints/core/appenders/multi_insert.hpp"
#include "type_blueprints/core/internal/macros.hpp"

 // std::multiset.
#include <set>

UAB_NS_BEGIN

template <typename Key, typename Compare, typename Allocator>
struct default_appender<std::multiset<Key, Compare, Allocator>>
    : multi_insert_appender<std::multiset<Key, Compare, Allocator>>
{
    using multi_insert_appender<std::multiset<Key, Compare, Allocator>>::multi_insert_appender;
};

UAB_NS_END

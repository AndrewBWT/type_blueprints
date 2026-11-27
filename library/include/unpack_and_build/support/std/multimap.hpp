#pragma once
/*!
 * @file
 * @brief std::multimap: built with insert, from std::pair<Key, T>.
 *
 * Specialises default_appender. Its sequence traits are detected (it's
 * iterable, with a value_type).
 */
#include "unpack_and_build/core/appenders.hpp"
#include "unpack_and_build/core/appenders/multi_insert.hpp"
#include "unpack_and_build/core/internal/macros.hpp"

 // std::multimap.
#include <map>

UAB_NS_BEGIN

template <typename Key, typename T, typename Compare, typename Allocator>
struct default_appender<std::multimap<Key, T, Compare, Allocator>>
    : multi_insert_appender<std::multimap<Key, T, Compare, Allocator>>
{
    using multi_insert_appender<std::multimap<Key, T, Compare, Allocator>>::multi_insert_appender;
};

UAB_NS_END

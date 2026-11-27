#pragma once
/*!
 * @file
 * @brief std::unordered_multiset: built with insert.
 *
 * Specialises default_appender. Its sequence traits are detected (it's
 * iterable, with a value_type).
 */
#include "unpack_and_build/core/appenders.hpp"
#include "unpack_and_build/core/appenders/multi_insert.hpp"
#include "unpack_and_build/core/internal/macros.hpp"

 // std::unordered_multiset.
#include <unordered_set>

UAB_NS_BEGIN

template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
struct default_appender<std::unordered_multiset<Key, Hash, KeyEqual, Allocator>>
    : multi_insert_appender<std::unordered_multiset<Key, Hash, KeyEqual, Allocator>>
{
    using multi_insert_appender<std::unordered_multiset<Key, Hash, KeyEqual, Allocator>>::multi_insert_appender;
};

UAB_NS_END

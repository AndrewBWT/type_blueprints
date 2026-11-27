#pragma once
/*!
 * @file
 * @brief std::multiset: built with insert.
 *
 * Specialises default_appender. Its sequence traits are detected (it's
 * iterable, with a value_type).
 */
#include "unpack_and_build/core/appenders.hpp"
#include "unpack_and_build/core/appenders/multi_insert.hpp"
#include "unpack_and_build/core/internal/macros.hpp"

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

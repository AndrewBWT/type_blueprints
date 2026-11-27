#pragma once
/*!
 * @file
 * @brief std::set: built with insert.
 *
 * Specialises default_appender. Its sequence traits are detected (it's
 * iterable, with a value_type).
 */
#include "unpack_and_build/core/appenders.hpp"
#include "unpack_and_build/core/appenders/unique_insert.hpp"
#include "unpack_and_build/core/internal/macros.hpp"

 // std::set.
#include <set>

UAB_NS_BEGIN

template <typename Key, typename Compare, typename Allocator>
struct default_appender<std::set<Key, Compare, Allocator>>
    : unique_insert_appender<std::set<Key, Compare, Allocator>>
{
    using unique_insert_appender<std::set<Key, Compare, Allocator>>::unique_insert_appender;
};

UAB_NS_END

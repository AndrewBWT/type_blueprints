#pragma once
/*!
 * @file
 * @brief Tests for multi_insert_appender.
 *
 * The node-based containers aren't constexpr in C++23, so behaviour is
 * tested at runtime.
 */
#include "type_blueprints/core/appenders.hpp"
#include "type_blueprints/core/appenders/multi_insert.hpp"

#include <catch2/catch_test_macros.hpp>

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

UAB_NS_BEGIN
namespace multi_insert_tests
{
static_assert(appender_c<multi_insert_appender<std::multiset<int>>, std::multiset<int>>);
static_assert(appender_c<
              multi_insert_appender<std::multimap<int, char>>,
              std::multimap<int, char>>);

TEST_CASE("multi_insert_appender adds duplicates", "[appenders][multi_insert]")
{
    std::multiset<int>                        multiset;
    multi_insert_appender<std::multiset<int>> appender(multiset);
    appender(1);
    CHECK(appender(1) == append_result::added);
    appender.finish();
    CHECK(appender.complete());
    CHECK(multiset.size() == 2);

    std::unordered_multiset<int>                        unordered;
    multi_insert_appender<std::unordered_multiset<int>> unordered_appender(unordered);
    unordered_appender(2);
    CHECK(unordered_appender(2) == append_result::added);
    unordered_appender.finish();
    CHECK(unordered.size() == 2);
}

TEST_CASE("multi_insert_appender keeps every value for a key", "[appenders][multi_insert]")
{
    std::multimap<int, char>                        map;
    multi_insert_appender<std::multimap<int, char>> appender(map);
    appender({1, 'a'});
    CHECK(appender({1, 'b'}) == append_result::added);
    appender.finish();
    CHECK(map.count(1) == 2);

    std::unordered_multimap<int, char>                        unordered;
    multi_insert_appender<std::unordered_multimap<int, char>> unordered_appender(unordered);
    unordered_appender({2, 'c'});
    unordered_appender({2, 'd'});
    unordered_appender.finish();
    CHECK(unordered.count(2) == 2);
}
} // namespace multi_insert_tests
UAB_NS_END

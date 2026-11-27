#pragma once
/*!
 * @file
 * @brief Tests for unique_insert_appender.
 *
 * The node-based containers aren't constexpr in C++23, so behaviour is
 * tested at runtime.
 */
#include "type_blueprints/core/appenders.hpp"
#include "type_blueprints/core/appenders/unique_insert.hpp"

#include <catch2/catch_test_macros.hpp>

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

UAB_NS_BEGIN
namespace unique_insert_tests
{
static_assert(appender_c<unique_insert_appender<std::set<int>>, std::set<int>>);
static_assert(appender_c<
              unique_insert_appender<std::map<int, char>>,
              std::map<int, char>>);

TEST_CASE("unique_insert_appender reports duplicates", "[appenders][unique_insert]")
{
    std::set<int>                         set;
    unique_insert_appender<std::set<int>> appender(set);
    CHECK(appender(1) == append_result::added);
    CHECK(appender(1) == append_result::already_present);
    appender.finish();
    CHECK(appender.complete());
    CHECK(set.size() == 1);

    std::unordered_set<int>                         unordered;
    unique_insert_appender<std::unordered_set<int>> unordered_appender(unordered);
    unordered_appender(2);
    CHECK(unordered_appender(2) == append_result::already_present);
    unordered_appender.finish();
    CHECK(unordered.size() == 1);
}

TEST_CASE("unique_insert_appender builds maps from pair<K, V>, keeping the first value", "[appenders][unique_insert]")
{
    std::map<int, char>                         map;
    unique_insert_appender<std::map<int, char>> appender(map);
    appender({1, 'a'});
    CHECK(appender({1, 'b'}) == append_result::already_present);
    appender.finish();
    CHECK(map.at(1) == 'a');

    std::unordered_map<int, char>                         unordered;
    unique_insert_appender<std::unordered_map<int, char>> unordered_appender(unordered);
    unordered_appender({2, 'c'});
    unordered_appender.finish();
    CHECK(unordered.at(2) == 'c');
}
} // namespace unique_insert_tests
UAB_NS_END

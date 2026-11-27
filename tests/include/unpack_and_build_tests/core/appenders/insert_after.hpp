#pragma once
/*!
 * @file
 * @brief Tests for insert_after_appender.
 *
 * std::forward_list isn't constexpr in C++23, so behaviour is tested at
 * runtime.
 */
#include "unpack_and_build/core/appenders.hpp"
#include "unpack_and_build/core/appenders/insert_after.hpp"

#include <catch2/catch_test_macros.hpp>

#include <forward_list>

UAB_NS_BEGIN
namespace insert_after_tests
{
static_assert(appender_c<
              insert_after_appender<std::forward_list<int>>,
              std::forward_list<int>>);

TEST_CASE("insert_after_appender keeps the order elements are given in", "[appenders][insert_after]")
{
    SECTION("After elements already there")
    {
        std::forward_list<int>                        list{1, 2};
        insert_after_appender<std::forward_list<int>> appender(list);
        appender(3);
        CHECK(appender(4) == append_result::added);
        appender.finish();
        CHECK(appender.complete());
        CHECK(list == std::forward_list<int>{1, 2, 3, 4});
    }

    SECTION("From empty")
    {
        std::forward_list<int>                        list;
        insert_after_appender<std::forward_list<int>> appender(list);
        appender(5);
        appender(6);
        appender.finish();
        CHECK(list == std::forward_list<int>{5, 6});
    }
}
} // namespace insert_after_tests
UAB_NS_END

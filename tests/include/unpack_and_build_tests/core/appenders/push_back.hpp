#pragma once
/*!
 * @file
 * @brief Tests for push_back_appender.
 *
 * std::vector and std::basic_string are constexpr, so are tested with
 * static_assert; std::deque and std::list at runtime.
 */
#include "unpack_and_build/core/appenders.hpp"
#include "unpack_and_build/core/appenders/push_back.hpp"

#include <catch2/catch_test_macros.hpp>

#include <deque>
#include <list>
#include <string>
#include <vector>

UAB_NS_BEGIN
namespace push_back_tests
{
static_assert(appender_c<push_back_appender<std::vector<int>>, std::vector<int>>);

// Elements go after those already there, and every one is added.
constexpr bool
    appends_to_vector()
{
    std::vector<int>                     container{1};
    push_back_appender<std::vector<int>> appender(container);
    const bool added = appender(2) == append_result::added
                       && appender(3) == append_result::added;
    appender.finish();
    return added && appender.complete() && container == std::vector<int>{1, 2, 3};
}

static_assert(appends_to_vector());

constexpr bool
    appends_to_string()
{
    std::u8string                     container;
    push_back_appender<std::u8string> appender(container);
    appender(u8'h');
    appender(u8'i');
    appender.finish();
    return container == u8"hi";
}

static_assert(appends_to_string());

TEST_CASE("push_back_appender fills std::deque and std::list", "[appenders][push_back]")
{
    std::deque<int>                     deque;
    push_back_appender<std::deque<int>> deque_appender(deque);
    deque_appender(7);
    deque_appender.finish();
    CHECK(deque == std::deque<int>{7});

    std::list<int>                     list;
    push_back_appender<std::list<int>> list_appender(list);
    list_appender(8);
    list_appender.finish();
    CHECK(list == std::list<int>{8});
}
} // namespace push_back_tests
UAB_NS_END

#pragma once
/*!
 * @file
 * @brief Tests for support/std/queue.hpp: std::queue as an adaptor.
 *
 * std::queue isn't constexpr in C++23, so behaviour is tested at runtime.
 */
#include "type_blueprints/support/std/queue.hpp"

#include <catch2/catch_test_macros.hpp>

#include <deque>
#include <queue>
#include <type_traits>

TYPE_BLUEPRINTS_NS_BEGIN
namespace support_std_queue_tests
{
static_assert(adaptor_c<std::queue<int>>);
static_assert(std::is_same_v<adaptor_container_t<std::queue<int>>, std::deque<int>>);

TEST_CASE("std::queue's container holds its elements front to back", "[adaptors][queue]")
{
    using traits = adaptor_traits<std::queue<int>>;
    std::queue<int> queue;
    queue.push(7);
    queue.push(8);
    CHECK(traits::underlying(queue) == std::deque<int>{7, 8});

    const auto rebuilt = traits::from_container({1, 2});
    CHECK(rebuilt.front() == 1);
    CHECK(rebuilt.back() == 2);
}
} // namespace support_std_queue_tests
TYPE_BLUEPRINTS_NS_END

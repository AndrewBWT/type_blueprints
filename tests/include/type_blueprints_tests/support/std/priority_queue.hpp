#pragma once
/*!
 * @file
 * @brief Tests for support/std/priority_queue.hpp: std::priority_queue as an
 * adaptor, rebuilt as a heap.
 *
 * std::priority_queue isn't constexpr in C++23, so behaviour is tested at
 * runtime.
 */
#include "type_blueprints/support/std/priority_queue.hpp"

#include <catch2/catch_test_macros.hpp>

#include <functional>
#include <queue>
#include <type_traits>
#include <vector>

// ============================================================ test types

namespace priority_queue_test_types
{
// A comparator that can't be default-constructed.
struct needs_state
{
    explicit needs_state(int) {}

    bool
        operator()(
            const int left_arg,
            const int right_arg
        ) const
    {
        return left_arg < right_arg;
    }
};
} // namespace priority_queue_test_types

TYPE_BLUEPRINTS_NS_BEGIN
namespace support_std_priority_queue_tests
{
using namespace ::priority_queue_test_types;

using min_queue_t = std::priority_queue<int, std::vector<int>, std::greater<int>>;
using stateful_queue_t
    = std::priority_queue<int, std::vector<int>, needs_state>;

static_assert(adaptor_c<std::priority_queue<int>>);
static_assert(adaptor_c<min_queue_t>);
static_assert(std::is_same_v<
              adaptor_container_t<std::priority_queue<int>>,
              std::vector<int>>);
// No default comparator, so no one-argument from_container: not an
// adaptor_c, though the comparator can still be passed.
static_assert(not adaptor_c<stateful_queue_t>);

TEST_CASE("std::priority_queue is rebuilt as a heap", "[adaptors][priority_queue]")
{
    SECTION("With a default comparator")
    {
        const auto queue = adaptor_traits<min_queue_t>::from_container({3, 1, 2});
        CHECK(queue.top() == 1);
        CHECK(adaptor_traits<min_queue_t>::underlying(queue).size() == 3);
    }

    SECTION("With a comparator passed explicitly")
    {
        const auto queue = adaptor_traits<stateful_queue_t>::from_container(
            {3, 1, 2}, needs_state(0)
        );
        CHECK(queue.top() == 3);
    }
}
} // namespace support_std_priority_queue_tests
TYPE_BLUEPRINTS_NS_END

#pragma once
/*!
 * @file
 * @brief Tests for support/std/stack.hpp: std::stack as an adaptor.
 *
 * std::stack isn't constexpr in C++23, so behaviour is tested at runtime.
 */
#include "type_blueprints/support/std/stack.hpp"

#include <catch2/catch_test_macros.hpp>

#include <deque>
#include <stack>
#include <type_traits>
#include <vector>

UAB_NS_BEGIN
namespace support_std_stack_tests
{
static_assert(adaptor_c<std::stack<int>>);
static_assert(adaptor_c<std::stack<int, std::vector<int>>>);
static_assert(std::is_same_v<adaptor_container_t<std::stack<int>>, std::deque<int>>);
static_assert(std::is_same_v<
              adaptor_container_t<std::stack<int, std::vector<int>>>,
              std::vector<int>>);

TEST_CASE("std::stack's container holds its elements bottom to top", "[adaptors][stack]")
{
    using traits = adaptor_traits<std::stack<int>>;
    std::stack<int> stack;
    stack.push(1);
    stack.push(2);
    stack.push(3);
    CHECK(traits::underlying(stack) == std::deque<int>{1, 2, 3});

    const auto rebuilt = traits::from_container({4, 5, 6});
    CHECK(rebuilt.top() == 6);
    CHECK(rebuilt.size() == 3);
}
} // namespace support_std_stack_tests
UAB_NS_END

#pragma once
/*!
 * @file
 * @brief Tests for support/std/vector.hpp: std::vector is built with
 * push_back_appender.
 */
#include "type_blueprints/support/std/vector.hpp"

#include <type_traits>
#include <vector>

UAB_NS_BEGIN
namespace support_std_vector_tests
{
using container_t = std::vector<int>;

static_assert(std::is_base_of_v<
              push_back_appender<container_t>,
              default_appender<container_t>>);
static_assert(buildable_sequence_c<container_t>);

// Built at compile time.
constexpr bool
    builds()
{
    container_t                   container;
    default_appender<container_t> appender(container);
    appender(1);
    appender(2);
    appender.finish();
    return appender.complete() && container == std::vector<int>{1, 2};
}

static_assert(builds());
} // namespace support_std_vector_tests
UAB_NS_END

#pragma once
/*!
 * @file
 * @brief Tests for support/std/string.hpp: std::basic_string is built with
 * push_back_appender.
 */
#include "type_blueprints/support/std/string.hpp"

#include <string>
#include <type_traits>

UAB_NS_BEGIN
namespace support_std_string_tests
{
using container_t = std::u8string;

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
    appender(u8'h');
    appender(u8'i');
    appender.finish();
    return appender.complete() && container == u8"hi";
}

static_assert(builds());
} // namespace support_std_string_tests
UAB_NS_END

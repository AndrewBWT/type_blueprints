#pragma once
/*!
 * @file
 * @brief std::basic_string: built with push_back.
 *
 * Specialises default_appender. Its sequence traits are detected (it's
 * iterable, with a value_type).
 */
#include "type_blueprints/core/appenders.hpp"
#include "type_blueprints/core/appenders/push_back.hpp"
#include "type_blueprints/core/internal/macros.hpp"

 // std::basic_string.
#include <string>

TYPE_BLUEPRINTS_NS_BEGIN

template <typename CharT, typename Traits, typename Allocator>
struct default_appender<std::basic_string<CharT, Traits, Allocator>>
    : push_back_appender<std::basic_string<CharT, Traits, Allocator>>
{
    using push_back_appender<std::basic_string<CharT, Traits, Allocator>>::push_back_appender;
};

TYPE_BLUEPRINTS_NS_END

#pragma once
/*!
 * @file
 * @brief What happened when an appender was given an element.
 */
#include "type_blueprints/core/internal/macros.hpp"

UAB_NS_BEGIN

/*!
 * @brief What happened when an appender was given an element. Appenders only
 * report; each consumer decides what a result means to it (a parser may treat
 * already_present as an error where a generator ignores it).
 */
enum class append_result
{
    // The element was added.
    added,
    // An equivalent element was already present, so nothing was added (unique
    // associative containers).
    already_present,
    // The container can hold no more elements, so nothing was added.
    full
};

UAB_NS_END

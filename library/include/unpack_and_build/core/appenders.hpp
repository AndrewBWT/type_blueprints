#pragma once
/*!
 * @file
 * @brief Appenders: building a sequence one element at a time.
 *
 * An appender is constructed from the sequence it fills (T&), and has:
 * - operator()(sequence_build_value_t<T>&&), returning an append_result.
 * - complete() const, returning whether the elements given so far make a
 *   whole sequence (false only while a fixed-length sequence is short).
 * - finish(), which consumers must call after the last element. Most
 *   appenders write each element as it arrives and do nothing here; one that
 *   can only build its container at the end (std::valarray's) does it here.
 *   finish() may be called more than once; each call leaves the container
 *   holding every element appended so far.
 *
 * default_appender<T> is the appender consumers use for T. Every buildable
 * type specialises it; nothing is detected. The appenders themselves are in
 * appenders/.
 *
 * Anything that uses default_appender on a type must see that type's
 * specialisation first. Include type_extensions/support/std.hpp from your own
 * library's core.
 */
#include "unpack_and_build/core/appenders/append_result_enum.hpp"
#include "unpack_and_build/core/internal/macros.hpp"
#include "unpack_and_build/core/sequence_traits.hpp"

 // std::is_constructible_v, std::is_same_v.
#include <type_traits>
// std::move.
#include <utility>

UAB_NS_BEGIN

/*!
 * @brief The appender used to build T from its elements.
 *
 * Nothing is detected: the primary template is empty, so
 * appender_c<default_appender<T>, T> is false until T has a specialisation.
 * Each type says how it's built, usually by inheriting one of the appenders in
 * appenders/ (push_back_appender, insert_appender, insert_after_appender,
 * fixed_size_appender) and its constructor.
 *
 * support/ has a specialisation for built-in arrays and each buildable std
 * container, one header per type.
 */
    template <typename T>
struct default_appender
{
};

/*!
 * @brief Satisfied when A can build a T: A is constructible from a T&; its
 * operator() takes a sequence_build_value_t<T>&& and returns an
 * append_result; complete() on a const A returns bool; and finish() can be
 * called.
 *
 * @tparam A The appender.
 * @tparam T The sequence it builds.
 */
template <typename A, typename T>
concept appender_c
= std::is_constructible_v<A, T&>
&& requires (
    A& appender_arg,
    const A& const_appender_arg,
    sequence_build_value_t<T>&& value_arg
    ) {
    requires std::is_same_v<
        decltype(appender_arg(std::move(value_arg))),
            append_result>;
            requires std::is_same_v<
                decltype(const_appender_arg.complete()),
                    bool>;
                appender_arg.finish();
};

/*!
 * @brief Satisfied when T has a build type and default_appender<T> can build
 * it.
 *
 * @tparam T The type to check, unqualified.
 */
template <typename T>
concept buildable_sequence_c
= requires { typename sequence_build_value<T>::type; }
&& appender_c<default_appender<T>, T>;

UAB_NS_END

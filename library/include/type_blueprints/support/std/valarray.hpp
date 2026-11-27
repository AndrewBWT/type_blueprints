#pragma once
/*!
 * @file
 * @brief Building a std::valarray.
 *
 * A valarray is detected as a sequence (it's iterable through std::begin and
 * std::end, and has a value_type), but it can't grow, so it's built by
 * collecting the elements first; this header specialises default_appender. Include it wherever
 * default_appender is used on a std::valarray (see sequence_traits.hpp).
 */

#include "type_blueprints/core/appenders.hpp"
#include "type_blueprints/core/appenders/append_result_enum.hpp"
#include "type_blueprints/core/sequence_traits.hpp"
#include "type_blueprints/core/internal/macros.hpp"

 // std::move.
#include <utility>
// std::valarray.
#include <valarray>
// std::vector.
#include <vector>

UAB_NS_BEGIN

/*!
 * @brief Builds a std::valarray by collecting its elements, then constructing
 * it in finish(). Until finish() is called, the valarray is unchanged.
 *
 * Elements already in the valarray are kept, in front of the new ones.
 *
 * Not constexpr: std::valarray isn't.
 */
    template <typename T>
class valarray_appender
{
public:
    explicit valarray_appender(
        std::valarray<T>& valarray_arg
    )
        : _valarray(&valarray_arg)
        , _elements(std::begin(valarray_arg), std::end(valarray_arg))
    {
    }

    append_result
        operator()(
            T&& value_arg
            )
    {
        _elements.push_back(std::move(value_arg));
        return append_result::added;
    }

    // Always true: any number of elements is a whole valarray.
    [[nodiscard]]
    constexpr bool
        complete() const noexcept
    {
        return true;
    }

    // Makes the valarray hold every element collected so far.
    void
        finish()
    {
        *_valarray = std::valarray<T>(_elements.data(), _elements.size());
    }
private:
    std::valarray<T>* _valarray;
    std::vector<T>    _elements;
};

template <typename T>
struct default_appender<std::valarray<T>> : valarray_appender<T>
{
    using valarray_appender<T>::valarray_appender;
};

UAB_NS_END

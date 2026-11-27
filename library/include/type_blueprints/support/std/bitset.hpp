#pragma once
/*!
 * @file
 * @brief std::bitset as a sequence of bools, most significant bit first (the
 * order std::bitset::to_string() writes them).
 *
 * A bitset can't be iterated and has no value_type, so nothing is detected
 * for it; this header specialises sequence_value, sequence_elements,
 * sequence_fixed_count and default_appender. Include it wherever those
 * traits are used on a std::bitset (see sequence_traits.hpp).
 */

#include "type_blueprints/core/appenders.hpp"
#include "type_blueprints/core/appenders/append_result_enum.hpp"
#include "type_blueprints/core/sequence_traits.hpp"
#include "type_blueprints/core/internal/macros.hpp"

 // std::bitset.
#include <bitset>
// std::size_t.
#include <cstddef>

UAB_NS_BEGIN

/*!
 * @brief A std::bitset's bits, most significant first.
 */
    template <std::size_t N>
struct bitset_bits_view
{
    struct iterator
    {
        const std::bitset<N>* bits;
        // The bits still to come; the next is bit remaining - 1.
        std::size_t remaining;

        constexpr bool
            operator*() const
        {
            return bits->test(remaining - 1);
        }

        constexpr iterator&
            operator++() noexcept
        {
            --remaining;
            return *this;
        }

        constexpr bool
            operator==(
                const iterator& other_arg
                ) const noexcept
        {
            return remaining == other_arg.remaining;
        }
    };

    const std::bitset<N>* bits;

    constexpr iterator
        begin() const noexcept
    {
        return { bits, N };
    }

    constexpr iterator
        end() const noexcept
    {
        return { bits, 0 };
    }
};

/*!
 * @brief Builds a std::bitset bit by bit, most significant first: each bit is
 * shifted in at the low end, so N bits land in the order they were given.
 *
 * Fewer than N bits fill the low bits (complete() is false until N have been
 * given); more than N report full.
 */
template <std::size_t N>
class bitset_appender
{
public:
    constexpr explicit bitset_appender(
        std::bitset<N>& bitset_arg
    ) noexcept
        : _bitset(&bitset_arg)
    {
    }

    constexpr append_result
        operator()(
            bool&& bit_arg
            )
    {
        if (_count == N)
        {
            return append_result::full;
        }
        *_bitset <<= 1;
        _bitset->set(0, bit_arg);
        ++_count;
        return append_result::added;
    }

    // Whether all N bits have been given.
    [[nodiscard]]
    constexpr bool
        complete() const noexcept
    {
        return _count == N;
    }

    // Nothing to do: each bit is written as it's appended.
    constexpr void
        finish() noexcept
    {
    }
private:
    std::bitset<N>* _bitset;
    std::size_t     _count = 0;
};

template <std::size_t N>
struct sequence_value<std::bitset<N>>
{
    using type = bool;
};

template <std::size_t N>
struct sequence_elements<std::bitset<N>>
{
    static constexpr bitset_bits_view<N>
        view(
            const std::bitset<N>& bitset_arg
        ) noexcept
    {
        return { &bitset_arg };
    }
};

template <std::size_t N>
struct sequence_fixed_count<std::bitset<N>>
{
    static constexpr std::size_t value = N;
};

template <std::size_t N>
struct default_appender<std::bitset<N>> : bitset_appender<N>
{
    using bitset_appender<N>::bitset_appender;
};

UAB_NS_END

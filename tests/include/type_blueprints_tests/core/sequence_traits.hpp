/*!
 * @file
 * @brief Tests for the sequence traits. All static_asserts.
 */
 // Every file using the traits includes the std specialisations first.
#include "type_blueprints/support/std/array.hpp"
#include "type_blueprints/support/std/bitset.hpp"

#include "type_blueprints/core/sequence_traits.hpp"

#include <array>
#include <bitset>
#include <cstddef>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <vector>

// ============================================================ test types

// Iterable through begin and end found by argument-dependent lookup.
struct adl_iterable
{
    using value_type = int;
    int values[3];
};

constexpr const int*
begin(
    const adl_iterable& object_arg
) noexcept
{
    return object_arg.values;
}

constexpr const int*
end(
    const adl_iterable& object_arg
) noexcept
{
    return object_arg.values + 3;
}

// Has key_type and mapped_type, but its elements aren't
// std::pair<const key_type, mapped_type>, so it isn't map-like.
struct not_a_map
{
    using key_type = int;
    using mapped_type = char;
    using value_type = long;

    constexpr const long*
        begin() const noexcept
    {
        return nullptr;
    }

    constexpr const long*
        end() const noexcept
    {
        return nullptr;
    }
};

// Iterable, but no value_type.
struct no_value_type_2
{
    constexpr const int*
        begin() const noexcept
    {
        return nullptr;
    }

    constexpr const int*
        end() const noexcept
    {
        return nullptr;
    }
};

// Holds bits but isn't iterable: the user specialises sequence_value and
// sequence_elements.
struct bits
{
    unsigned value;
};

struct bits_view
{
    struct iterator
    {
        const bits* source;
        unsigned    index;

        constexpr bool
            operator*() const noexcept
        {
            return ((source->value >> index) & 1U) != 0;
        }

        constexpr iterator&
            operator++() noexcept
        {
            ++index;
            return *this;
        }

        constexpr bool
            operator==(const iterator&) const
            = default;
    };

    const bits* source;

    constexpr iterator
        begin() const noexcept
    {
        return { source, 0 };
    }

    constexpr iterator
        end() const noexcept
    {
        return { source, 4 };
    }
};

// A family described by one constrained partial specialisation.
struct tag
{
};

template <typename T>
struct tagged_vector : std::vector<T>
{
    using is_tagged = void;
};

template <typename T>
concept tagged_c = requires { typename T::is_tagged; };

// Detected as a sequence of int; the user builds it from long.
struct built_from_long
{
    using value_type = int;

    constexpr const int*
        begin() const noexcept
    {
        return nullptr;
    }

    constexpr const int*
        end() const noexcept
    {
        return nullptr;
    }
};

// A user type whose length is part of its type: the user specialises
// sequence_fixed_count, partially.
template <std::size_t N>
struct fixed_buffer
{
    using value_type = int;
    int values[N];

    constexpr const int*
        begin() const noexcept
    {
        return values;
    }

    constexpr const int*
        end() const noexcept
    {
        return values + N;
    }
};

// A sequence_fixed_count whose value isn't a std::size_t.
struct wrong_count_type
{
};

UAB_NS_BEGIN

template <std::size_t N>
struct sequence_fixed_count<fixed_buffer<N>>
{
    static constexpr std::size_t value = N;
};

template <>
struct sequence_fixed_count<wrong_count_type>
{
    static constexpr int value = 3;
};

template <>
struct sequence_value<bits>
{
    using type = bool;
};

template <>
struct sequence_elements<bits>
{
    static constexpr bits_view
        view(
            const bits& object_arg
        ) noexcept
    {
        return { &object_arg };
    }
};

// Unambiguous: the library never partially specialises sequence_value, so
// this wins over detection though tagged_vector has a value_type.
template <typename T>
    requires tagged_c<T>
struct sequence_value<T>
{
    using type = tag;
};

template <>
struct sequence_build_value<built_from_long>
{
    using type = long;
};

namespace sequence_traits_tests
{
    // ============================================================ element types

    static_assert(std::is_same_v<sequence_value_t<std::vector<int>>, int>);
    static_assert(std::is_same_v<sequence_value_t<std::deque<int>>, int>);
    static_assert(std::is_same_v<sequence_value_t<std::list<int>>, int>);
    static_assert(std::is_same_v<sequence_value_t<std::forward_list<int>>, int>);
    static_assert(std::is_same_v<sequence_value_t<std::set<int>>, int>);
    static_assert(std::is_same_v<sequence_value_t<std::u8string>, char8_t>);
    static_assert(std::is_same_v<sequence_value_t<std::array<int, 2>>, int>);
    static_assert(std::is_same_v<sequence_value_t<std::valarray<int>>, int>);
    static_assert(std::is_same_v<sequence_value_t<int[3]>, int>);
    static_assert(std::is_same_v<sequence_value_t<int[2][3]>, int[3]>);
    static_assert(std::is_same_v<
        sequence_value_t<std::map<int, char>>,
        std::pair<const int, char>>);
    static_assert(std::is_same_v<sequence_value_t<adl_iterable>, int>);
    // Specialised: by support/std/bitset.hpp, and by the user.
    static_assert(std::is_same_v<sequence_value_t<std::bitset<4>>, bool>);
    static_assert(std::is_same_v<sequence_value_t<bits>, bool>);
    static_assert(std::is_same_v<sequence_value_t<tagged_vector<int>>, tag>);

    template <typename T>
    concept has_value_c = requires { typename sequence_value_t<T>; };
    // std::optional has a value_type, but isn't iterable.
    static_assert(not has_value_c<std::optional<int>>);
    static_assert(not has_value_c<no_value_type_2>);
    static_assert(not has_value_c<std::pair<int, int>>);
    static_assert(not has_value_c<int>);
    static_assert(not has_value_c<int[]>);
    // The adaptors hide their elements; nothing is detected for them.
    static_assert(not has_value_c<std::stack<int>>);
    static_assert(not has_value_c<std::queue<int>>);
    static_assert(not has_value_c<std::priority_queue<int>>);

    // ============================================================ build types

    static_assert(std::is_same_v<sequence_build_value_t<std::vector<int>>, int>);
    static_assert(std::is_same_v<sequence_build_value_t<std::set<int>>, int>);
    static_assert(std::is_same_v<sequence_build_value_t<std::unordered_set<int>>, int>);
    static_assert(std::is_same_v<
        sequence_build_value_t<std::map<int, char>>,
        std::pair<int, char>>);
    static_assert(std::is_same_v<
        sequence_build_value_t<std::multimap<int, char>>,
        std::pair<int, char>>);
    static_assert(std::is_same_v<
        sequence_build_value_t<std::unordered_map<int, char>>,
        std::pair<int, char>>);
    static_assert(std::is_same_v<sequence_build_value_t<int[2][3]>, int[3]>);
    static_assert(std::is_same_v<sequence_build_value_t<not_a_map>, long>);
    // Follows a specialised element type...
    static_assert(std::is_same_v<sequence_build_value_t<bits>, bool>);
    static_assert(std::is_same_v<sequence_build_value_t<std::bitset<4>>, bool>);
    // ...unless specialised itself.
    static_assert(std::is_same_v<sequence_value_t<built_from_long>, int>);
    static_assert(std::is_same_v<sequence_build_value_t<built_from_long>, long>);

    template <typename T>
    concept has_build_value_c = requires { typename sequence_build_value_t<T>; };
    // No element type, so no build type.
    static_assert(not has_build_value_c<no_value_type_2>);
    static_assert(not has_build_value_c<std::optional<int>>);
    static_assert(not has_build_value_c<int>);

    // ============================================================ fixed counts

    static_assert(fixed_count_sequence_c<int[3]>);
    static_assert(sequence_fixed_count_v<int[3]> == 3);
    static_assert(sequence_fixed_count_v<int[2][3]> == 2);
    // Specialised in support/std/.
    static_assert(sequence_fixed_count_v<std::array<int, 4>> == 4);
    static_assert(sequence_fixed_count_v<std::array<int, 0>> == 0);
    static_assert(sequence_fixed_count_v<std::bitset<8>> == 8);
    // Specialised by the user.
    static_assert(sequence_fixed_count_v<fixed_buffer<5>> == 5);

    static_assert(not fixed_count_sequence_c<std::vector<int>>);
    static_assert(not fixed_count_sequence_c<std::u8string>);
    static_assert(not fixed_count_sequence_c<std::valarray<int>>);
    static_assert(not fixed_count_sequence_c<std::tuple<int, int>>);
    static_assert(not fixed_count_sequence_c<int>);
    static_assert(not fixed_count_sequence_c<int[]>);
    // The value must be a std::size_t.
    static_assert(not fixed_count_sequence_c<wrong_count_type>);

    // ============================================================ sequence_c

    static_assert(sequence_c<std::vector<int>>);
    static_assert(sequence_c<std::map<int, char>>);
    static_assert(sequence_c<std::u8string>);
    static_assert(sequence_c<std::valarray<int>>);
    static_assert(sequence_c<int[3]>);
    static_assert(sequence_c<std::array<int, 0>>);
    static_assert(sequence_c<adl_iterable>);
    static_assert(sequence_c<not_a_map>);
    // Not iterable itself, but its specialised view is.
    static_assert(sequence_c<std::bitset<4>>);
    static_assert(sequence_c<bits>);

    static_assert(not sequence_c<std::optional<int>>);
    static_assert(not sequence_c<no_value_type_2>);
    static_assert(not sequence_c<std::tuple<int, int>>);
    static_assert(not sequence_c<int>);
    static_assert(not sequence_c<int[]>);
    static_assert(not sequence_c<std::stack<int>>);
    static_assert(not sequence_c<std::queue<int>>);
    static_assert(not sequence_c<std::priority_queue<int>>);

    // ============================================================ elements

    // The default view is the object itself.
    constexpr bool
        default_view_is_identity()
    {
        const std::vector<int> object{ 1, 2 };
        return &sequence_elements<std::vector<int>>::view(object) == &object;
    }

    static_assert(default_view_is_identity());

    // A specialised view gives the elements of a non-iterable sequence.
    constexpr bool
        reads_bits()
    {
        const bits object{ 0b1010 };
        unsigned   rebuilt = 0;
        unsigned   index = 0;
        for (const bool bit : sequence_elements<bits>::view(object))
        {
            rebuilt |= (bit ? 1U : 0U) << index;
            ++index;
        }
        return index == 4 && rebuilt == object.value;
    }

    static_assert(reads_bits());

    // support/std/bitset.hpp gives a std::bitset's bits most significant first,
    // the order std::bitset::to_string() writes them.
    constexpr bool
        reads_bitset_most_significant_first()
    {
        const std::bitset<4> object{ 0b0011 };
        bool                 expected[4] = { false, false, true, true };
        std::size_t          index = 0;
        for (const bool bit : sequence_elements<std::bitset<4>>::view(object))
        {
            if (index == 4 || bit != expected[index])
            {
                return false;
            }
            ++index;
        }
        return index == 4;
    }

    static_assert(reads_bitset_most_significant_first());
} // namespace sequence_traits_tests
UAB_NS_END

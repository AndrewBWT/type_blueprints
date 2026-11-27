/*!
 * @file
 * @brief Tests for contiguous_container_c and contiguous_container_of_c.
 *
 * Everything here is a static_assert: the file only compiles if the concepts
 * behave as described. It has no runtime tests.
 */
#include "unpack_and_build/core/extended_concepts.hpp"

#include <array>
#include <cstddef>
#include <deque>
#include <forward_list>
#include <initializer_list>
#include <list>
#include <map>
#include <optional>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <valarray>
#include <vector>

// ============================================================ test types

// The minimum a contiguous container needs.
struct minimal_container
{
    using value_type = int;

    const int*
        data() const noexcept;
    std::size_t
        size() const noexcept;
};

// Missing value_type.
struct no_value_type
{
    const int*
        data() const noexcept;
    std::size_t
        size() const noexcept;
};

// Missing data().
struct no_data
{
    using value_type = int;

    std::size_t
        size() const noexcept;
};

// Missing size().
struct no_size
{
    using value_type = int;

    const int*
        data() const noexcept;
};

// data() returns something that isn't a pointer.
struct data_not_pointer
{
    using value_type = int;

    int
        data() const noexcept;
    std::size_t
        size() const noexcept;
};

// data() points at something other than value_type.
struct data_points_elsewhere
{
    using value_type = int;

    const long*
        data() const noexcept;
    std::size_t
        size() const noexcept;
};

// size() returns something that doesn't convert to std::size_t.
struct size_not_convertible
{
    struct not_a_size
    {};

    using value_type = int;

    const int*
        data() const noexcept;
    not_a_size
        size() const noexcept;
};

// data() is only callable on a non-const object.
struct non_const_data_only
{
    using value_type = int;

    int*
        data() noexcept;
    std::size_t
        size() const noexcept;
};

// size() returns an int: converts to std::size_t, so allowed.
struct int_size
{
    using value_type = int;

    const int*
        data() const noexcept;
    int
        size() const noexcept;
};

// data() returns a pointer to non-const (shallow constness, like std::span).
struct mutable_data
{
    using value_type = int;

    int*
        data() const noexcept;
    std::size_t
        size() const noexcept;
};

// data() returns a pointer to volatile value_type: cv is removed.
struct volatile_data
{
    using value_type = int;

    const volatile int*
        data() const noexcept;
    std::size_t
        size() const noexcept;
};

// value_type is itself const: the pointee, with cv removed, doesn't match.
struct const_value_type
{
    using value_type = const int;

    const int*
        data() const noexcept;
    std::size_t
        size() const noexcept;
};

// For contiguous_container_of_c: a derived element type.
struct base
{
    int x;
};

struct derived : base
{
    int y;
};

// ============================================================ test types
// for specialization_of_c

// A user's variadic template.
template <typename... Ts>
struct pack
{};

// A user's template with one type parameter.
template <typename T>
struct box
{};

// Derives from a specialization, but isn't one itself.
struct derived_from_tuple : std::tuple<int>
{};

// ============================================================ test types
// for iterable_c

namespace iterable_test_types
{
// begin() and end() members.
struct member_begin_end
{
    const int*
        begin() const noexcept;
    const int*
        end() const noexcept;
};

// begin and end found by argument-dependent lookup.
struct adl_begin_end
{};

const int*
    begin(const adl_begin_end&) noexcept;
const int*
    end(const adl_begin_end&) noexcept;

// begin() but no end().
struct only_begin
{
    const int*
        begin() const noexcept;
};

// end() but no begin().
struct only_end
{
    const int*
        end() const noexcept;
};

// begin() and end() only on a non-const object.
struct non_const_only
{
    int*
        begin() noexcept;
    int*
        end() noexcept;
};

// A free begin and end, but in another namespace, so ADL doesn't find them.
struct unreachable_begin_end
{};
} // namespace iterable_test_types

namespace elsewhere
{
const int*
    begin(const iterable_test_types::unreachable_begin_end&) noexcept;
const int*
    end(const iterable_test_types::unreachable_begin_end&) noexcept;
} // namespace elsewhere

UAB_NS_BEGIN

namespace extended_concepts_tests
{
// ============================================================
// contiguous_container_c: standard types

// Contiguous, with data(), size() and value_type.
static_assert(contiguous_container_c<std::vector<int>>);
static_assert(contiguous_container_c<std::array<int, 3>>);
static_assert(contiguous_container_c<std::array<int, 0>>);
static_assert(contiguous_container_c<std::string>);
static_assert(contiguous_container_c<std::u8string>);
static_assert(contiguous_container_c<std::string_view>);
static_assert(contiguous_container_c<std::span<int>>);
static_assert(contiguous_container_c<std::span<const int>>);
static_assert(contiguous_container_c<std::span<int, 3>>);

// Not contiguous.
static_assert(not contiguous_container_c<std::list<int>>);
static_assert(not contiguous_container_c<std::forward_list<int>>);
static_assert(not contiguous_container_c<std::deque<int>>);
// Contiguous storage, but no data() member.
static_assert(not contiguous_container_c<std::valarray<int>>);
// C++26 gives initializer_list a data() member; with it, it's a contiguous
// container, and without it, it isn't.
static_assert(
    contiguous_container_c<std::initializer_list<int>>
    == requires (const std::initializer_list<int>& list_arg
    ) { list_arg.data(); }
);
// Packed bits: no data().
static_assert(not contiguous_container_c<std::vector<bool>>);
// Built-in arrays have no members.
static_assert(not contiguous_container_c<int[3]>);
// Not containers.
static_assert(not contiguous_container_c<int>);
static_assert(not contiguous_container_c<int*>);

// A const container still works (its data() is const anyway); a reference
// doesn't, as a reference type has no member types. Pass the type itself.
static_assert(contiguous_container_c<const std::vector<int>>);
static_assert(not contiguous_container_c<std::vector<int>&>);

// ============================================================
// contiguous_container_c: each requirement

static_assert(contiguous_container_c<minimal_container>);
static_assert(not contiguous_container_c<no_value_type>);
static_assert(not contiguous_container_c<no_data>);
static_assert(not contiguous_container_c<no_size>);
static_assert(not contiguous_container_c<data_not_pointer>);
static_assert(not contiguous_container_c<data_points_elsewhere>);
static_assert(not contiguous_container_c<size_not_convertible>);
// data() must be callable on a const container.
static_assert(not contiguous_container_c<non_const_data_only>);

// Allowed variations.
static_assert(contiguous_container_c<int_size>);
static_assert(contiguous_container_c<mutable_data>);
static_assert(contiguous_container_c<volatile_data>);
// value_type is compared as declared: a const value_type doesn't match the
// pointee with const removed.
static_assert(not contiguous_container_c<const_value_type>);

// ============================================================
// contiguous_container_of_c

static_assert(contiguous_container_of_c<std::vector<int>, int>);
static_assert(contiguous_container_of_c<std::array<double, 2>, double>);
static_assert(contiguous_container_of_c<std::u8string, char8_t>);
static_assert(contiguous_container_of_c<std::span<const int>, int>);
static_assert(contiguous_container_of_c<std::span<int>, int>);
static_assert(contiguous_container_of_c<minimal_container, int>);

// The element type must match exactly.
static_assert(not contiguous_container_of_c<std::vector<int>, long>);
static_assert(not contiguous_container_of_c<std::vector<int>, unsigned int>);
static_assert(not contiguous_container_of_c<std::string, char8_t>);
// Not even a derived type: its elements are larger, so walking data() as a
// base* would read them at the wrong stride.
static_assert(not contiguous_container_of_c<std::vector<derived>, base>);
static_assert(contiguous_container_of_c<std::vector<derived>, derived>);
// T is unqualified: a cv-qualified T never matches.
static_assert(not contiguous_container_of_c<std::vector<int>, const int>);
static_assert(not contiguous_container_of_c<std::span<const int>, const int>);

// Not a contiguous container at all.
static_assert(not contiguous_container_of_c<std::list<int>, int>);
static_assert(not contiguous_container_of_c<int[3], int>);
static_assert(not contiguous_container_of_c<no_size, int>);

// ============================================================
// specialization_of_c

// Standard templates.
static_assert(specialization_of_c<std::tuple<>, std::tuple>);
static_assert(specialization_of_c<std::tuple<int, char>, std::tuple>);
static_assert(specialization_of_c<std::pair<int, char>, std::pair>);
static_assert(specialization_of_c<std::optional<int>, std::optional>);
// Default template arguments (the allocator, the comparator) still match.
static_assert(specialization_of_c<std::vector<int>, std::vector>);
static_assert(specialization_of_c<std::map<int, char>, std::map>);

// User templates, including an empty pack.
static_assert(specialization_of_c<pack<>, pack>);
static_assert(specialization_of_c<pack<int, char, long>, pack>);
static_assert(specialization_of_c<box<int>, box>);

// A different template.
static_assert(not specialization_of_c<std::pair<int, int>, std::tuple>);
static_assert(not specialization_of_c<std::tuple<int, int>, std::pair>);
static_assert(not specialization_of_c<pack<int>, box>);
static_assert(not specialization_of_c<box<int>, pack>);

// Not a specialization of anything.
static_assert(not specialization_of_c<int, std::tuple>);
static_assert(not specialization_of_c<void, std::tuple>);

// Only the outermost template counts.
static_assert(specialization_of_c<std::tuple<std::vector<int>>, std::tuple>);
static_assert(not specialization_of_c<
              std::tuple<std::vector<int>>,
              std::vector>);

// Deriving from a specialization doesn't make a type one.
static_assert(not specialization_of_c<derived_from_tuple, std::tuple>);

// cv-qualifiers, references and pointers aren't removed.
static_assert(not specialization_of_c<const std::tuple<int>, std::tuple>);
static_assert(not specialization_of_c<volatile std::tuple<int>, std::tuple>);
static_assert(not specialization_of_c<std::tuple<int>&, std::tuple>);
static_assert(not specialization_of_c<std::tuple<int>&&, std::tuple>);
static_assert(not specialization_of_c<std::tuple<int>*, std::tuple>);

// Not testable here: templates with a value parameter, such as std::array.
// specialization_of_c<std::array<int, 3>, std::array> doesn't evaluate to
// false; it fails to compile, as std::array can't be passed where a
// template <typename...> typename is expected.

// ============================================================ iterable_c

// Standard containers and views.
static_assert(iterable_c<std::vector<int>>);
static_assert(iterable_c<std::array<int, 3>>);
static_assert(iterable_c<std::array<int, 0>>);
static_assert(iterable_c<std::list<int>>);
static_assert(iterable_c<std::forward_list<int>>);
static_assert(iterable_c<std::set<int>>);
static_assert(iterable_c<std::map<int, char>>);
static_assert(iterable_c<std::string>);
static_assert(iterable_c<std::u8string_view>);
static_assert(iterable_c<std::span<int>>);
static_assert(iterable_c<std::initializer_list<int>>);
// std::valarray has no members; its begin and end are found by ADL.
static_assert(iterable_c<std::valarray<int>>);

// Built-in arrays of known bound, including nested ones.
static_assert(iterable_c<int[3]>);
static_assert(iterable_c<int[2][3]>);
// An array of unknown bound can't be iterated.
static_assert(not iterable_c<int[]>);

// Each way of providing begin and end.
static_assert(iterable_c<iterable_test_types::member_begin_end>);
static_assert(iterable_c<iterable_test_types::adl_begin_end>);

// Both begin and end are needed.
static_assert(not iterable_c<iterable_test_types::only_begin>);
static_assert(not iterable_c<iterable_test_types::only_end>);
// They must work on a const object.
static_assert(not iterable_c<iterable_test_types::non_const_only>);
// A free begin and end must be found by ADL, not merely exist.
static_assert(not iterable_c<iterable_test_types::unreachable_begin_end>);

// Not iterable.
static_assert(not iterable_c<int>);
static_assert(not iterable_c<int*>);
static_assert(not iterable_c<std::optional<int>>);
static_assert(not iterable_c<std::tuple<int, int>>);
static_assert(not iterable_c<std::pair<int, int>>);
} // namespace extended_concepts_tests

UAB_NS_END
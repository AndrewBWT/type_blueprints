#pragma once
/*!
 * @file
 * @brief Tests for adaptor_traits, adaptor_c and adaptor_container_t, with
 * user types. The std adaptors are tested with their support headers.
 *
 * All static_asserts, plus one Catch2 test.
 */
#include "type_blueprints/core/adaptors.hpp"

#include <catch2/catch_test_macros.hpp>

#include <type_traits>
#include <utility>
#include <vector>

// ============================================================ test types

namespace adaptor_test_types
{
// A user's adaptor, described by specialising adaptor_traits.
class wrapper
{
public:
    constexpr explicit wrapper(
        std::vector<int>&& items_arg
    )
        : _items(std::move(items_arg))
    {}

    constexpr const std::vector<int>&
        items() const noexcept
    {
        return _items;
    }
private:
    std::vector<int> _items;
};

// Laid out like a standard adaptor, but nothing is detected: not an adaptor
// until adaptor_traits is specialised for it.
class unspecialised_adaptor
{
public:
    using container_type = std::vector<int>;
protected:
    container_type c;
};
} // namespace adaptor_test_types

TYPE_BLUEPRINTS_NS_BEGIN

template <>
struct adaptor_traits<adaptor_test_types::wrapper>
{
    using container_type = std::vector<int>;

    static constexpr const container_type&
        underlying(
            const adaptor_test_types::wrapper& wrapper_arg
        ) noexcept
    {
        return wrapper_arg.items();
    }

    static constexpr adaptor_test_types::wrapper
        from_container(
            container_type&& container_arg
        )
    {
        return adaptor_test_types::wrapper(std::move(container_arg));
    }
};

namespace adaptors_tests
{
using namespace ::adaptor_test_types;

// ============================================================ adaptor_c

static_assert(adaptor_c<wrapper>);
static_assert(std::is_same_v<adaptor_container_t<wrapper>, std::vector<int>>);

static_assert(not adaptor_c<unspecialised_adaptor>);
static_assert(not adaptor_c<std::vector<int>>);
static_assert(not adaptor_c<int>);

// ============================================================ round trip

// The user's adaptor is usable at compile time, as std::vector is.
constexpr bool
    wrapper_round_trip()
{
    const wrapper object = adaptor_traits<wrapper>::from_container({1, 2});
    return adaptor_traits<wrapper>::underlying(object) == std::vector<int>{1, 2};
}

static_assert(wrapper_round_trip());

TEST_CASE("A user's adaptor works through its specialisation", "[adaptors]")
{
    CHECK(wrapper_round_trip());
}
} // namespace adaptors_tests
TYPE_BLUEPRINTS_NS_END

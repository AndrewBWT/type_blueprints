#pragma once
#include "type_blueprints/core/extended_concepts.hpp"
#include "type_blueprints/core/internal/macros.hpp"

// std::size_t.
#include <cstddef>
// std::u8string_view.
#include <string_view>
// std::is_enum_v, std::is_same_v.
#include <type_traits>
// For std::invalid_argument.
#include <stdexcept>

UAB_NS_BEGIN

/*!
 * @brief An enum_element; the enum value with its string name.
 * @tparam E The enum parameter. Must adhere to std::is_enum_v.
 */
    template <typename E>
    requires std::is_enum_v<E>
struct enum_element
{
    //! The internal type of the enum_element.
    using enum_type = E;
    //! The enum itself.
    E value;
    //! The string representing the enum.
    std::u8string_view name;
};

/*!
 * @brief Lists an enumeration's values and their names at compile time.
 *
 * Each specialisation provides values, a static constexpr built-in array or
 * std::array of enum_element<E>. The primary template is empty, so nothing is
 * listed until enum_list is specialised.
 *
 * The base list, enum_list<E> (Version is void), defines the values and
 * their order. Its names must be the enumerators' spellings in the source,
 * which are checked to be ASCII identifiers.
 *
 * A version, enum_list<E, Version> with a user-defined tag type, gives the
 * same values other names (symbols, translations). It must list every value
 * in the base list, in the same order, so an index means the same enumerator
 * in every version. Its names can be any UTF-8; they aren't validated.
 *
 * Every list must have at least one element, non-empty names, unique values
 * and unique names. A broken list is a compile error when used.
 *
 * Alias enumerators (two enumerators with the same value) can't both be
 * listed, as value to name must be a function. List one of them.
 *
 * @code
 * UAB_NS_BEGIN
 * template <>
 * struct enum_list<result>
 * {
 *     static constexpr enum_element<result> values[] = {
 *         {result::pass, u8"pass"},
 *         {result::fail, u8"fail"},
 *     };
 * };
 *
 * struct symbols {};
 * template <>
 * struct enum_list<result, symbols>
 * {
 *     static constexpr enum_element<result> values[] = {
 *         {result::pass, u8"✓"},
 *         {result::fail, u8"✗"},
 *     };
 * };
 * UAB_NS_END
 * @endcode
 *
 * @tparam E The enumeration.
 * @tparam Version void for the base list, or a tag type for a version.
 */
template <typename E, typename Version = void>
    requires std::is_enum_v<E>
struct enum_list
{
};

/*!
 * @brief A read-only view of an enumerator list, held elsewhere. Use cases are
 * the compile-time enum_list<E>::values, or a list built at runtime.
 *
 * It can be thought of as a span or string_view, in that it acts as an
 * interface to the underlying elements.
 *
 * The view doesn't own the list or the names inside it. Both must
 * outlive the view. A list whose names are built at runtime needs
 * somewhere to keep them (a std::vector<std::u8string>, for example).
 *
 * @code
 * std::vector<enum_element<colour>> runtime_list{{colour::red, u8"rouge"}};
 * const enum_list_view list(runtime_list);
 * list.find(u8"rouge");
 * @endcode
 *
 * @tparam E The enum class.
 */
template <typename E>
    requires std::is_enum_v<E>
class enum_list_view
{
public:
    //! Member type for the enum_element contained in the view.
    using element_type = enum_element<E>;
    static constexpr std::size_t npos = static_cast<std::size_t>(-1);
    /*!
     * @brief Constructor using pointer and size.
     * @param elements_arg Pointer to memory containing elements.
     *
     * Throws an exception if any of the following occur. Exceptions thrown
     * during constant evaluation will result in compilation failures.
     * -   Must be at least one element_type in elements_arg.
     * -   None of the element_type's names are empty.
     * -   Each of the element type's name and value are unique.
     *
     * @param size_arg Number of elements to take.
     * @throws invalid_argument If any of the above criteria are false.
     */
    constexpr enum_list_view(
        const element_type* const elements_arg,
        const std::size_t         size_arg
    );
    /*!
     * @brief Constructor for C array of elements.
     *
     * See the (element_type* const, size_t) constructor for conditions that
     * throw an exception.
     *
     * @tparam N The size of the array argument.
     * @param elements_arg The array argument.
     * @throws invalid_argument If any of the checks on the input fail.
     */
    template <std::size_t N>
    constexpr enum_list_view(const element_type(&elements_arg)[N]);
    /*!
     * @brief Constructor taking an object that adheres to the
     * contiguous_container_of_c concept.
     *
     * See the (element_type* const, size_t) constructor for conditions that
     * throw an exception.
     * @tparam Container The type container_arg.
     * @param container_arg The container holding the elements.
     * @throws invalid_argument If any of the checks on the input fail.
     */
    template <typename Container>
        requires contiguous_container_of_c<Container, enum_element<E>>
    constexpr enum_list_view(const Container& container_arg);
    // Deleted as enum_list_view does not own its elements. This constructor
    // would make the object invalid.
    template <typename Container>
        requires (not std::is_same_v<Container, enum_list_view>)
    enum_list_view(const Container&&) = delete;
    /*!
     * @brief Gets the size of the enum_list_view.
     * @return The number of elements in the enum_list_view.
     */
    [[nodiscard]]
    constexpr std::size_t
        size() const noexcept;
    /*!
     * @brief Gets a pointer to the beginning of the contiguous memory
     * containing the set of element_types.
     * @return A pointer to the start of the memory.
     */
    [[nodiscard]]
    constexpr const element_type*
        begin() const noexcept;
    /*!
     * @brief Gets a pointer to one past the end of the contiguous memory
     * containing the set of element_types.
     * @return A pointer to one past the end of the memory.
     */
    [[nodiscard]]
    constexpr const element_type*
        end() const noexcept;
    /*!
     * @brief Access operator for indexing into the contiguous memory containing
     * the element_types.
     * @param index_arg The index to get the element_type for.
     * @return The element_type at index_arg.
     */
    [[nodiscard]]
    constexpr const element_type&
        operator[](const std::size_t index_arg) const noexcept;
    /*!
     * @brief Finds a pointer to the element_type of the given value_arg.
     *
     * Takes value_arg as a value, as E is always enum, so it should be cheap
     * to copy.
     *
     * @param value_arg The enum to check for.
     * @return The element_type representing value_arg, or nullptr if there is
     * none.
     */
    [[nodiscard]]
    constexpr const element_type*
        find(const E value_arg) const noexcept;
    /*!
     * @brief Finds a pointer to the element_type of the given name name_arg.
     *
     * @param name_arg The name to check for
     * @return The element_type containing name_arg, or nullptr if there is
     * none.
     */
    [[nodiscard]]
    constexpr const element_type*
        find(const std::u8string_view name_arg) const noexcept;
    /*!
     * @brief Finds the index of the element with a value.
     * @param value_arg The value to look for.
     * @return The index of the element with value_arg, or npos if there is
     * none.
     */
    [[nodiscard]]
    constexpr std::size_t
        index_of(const E value_arg) const noexcept;
    /*!
     * @brief Finds the index of the element with a name.
     * @param name_arg The name to look for.
     * @return The index of the element with name_arg, or npos if there is
     * none.
     */
    [[nodiscard]]
    constexpr std::size_t
        index_of(const std::u8string_view name_arg) const noexcept;
    /*!
     * @brief Finds the name of a value.
     *
     * Names are never empty, so an empty result can only mean the value isn't
     * listed.
     *
     * @param value_arg The value to look for.
     * @return The name of value_arg, or an empty string_view if there is none.
     */
    [[nodiscard]]
    constexpr std::u8string_view
        name_of(const E value_arg) const noexcept;
private:
    //! The beginning of the memory containing the element_types.
    const element_type* _elements;
    //! The size of the memory containing the element_types.
    std::size_t _size;
};

/*!
 * @brief Deduction rule for containers, allows the constructor to deduce the
 * created type.
 */
template <typename Container>
enum_list_view(const Container&)
-> enum_list_view<typename Container::value_type::enum_type>;
UAB_INTERNAL_NS_BEGIN
/*!
 * @brief Checks that enum_list<E, Version> has a values member that an
 * enum_list_view can be built from.
 */
    template <typename E, typename Version>
concept has_values_c = std::is_enum_v<E> && requires {
    enum_list_view<E>(enum_list<E, Version>::values);
};
UAB_INTERNAL_NS_END
/*!
 * @brief Checks that an enum_list is defined for an enumeration, and that an
 * enum_list_view can be built from its values.
 *
 * For a version, the base list must be defined too.
 *
 * Only the list's shape is checked here, not its contents; enumerators_of
 * checks those.
 *
 * @tparam E The enumeration.
 * @tparam Version void for the base list, or a version's tag type.
 */
    template <typename E, typename Version = void>
concept has_enum_list_c
= internal::has_values_c<E, Version>
&& (std::is_void_v<Version> || UAB_INTERNAL_NS::has_values_c<E, void>);

/*!
 * @brief A view of an enum_list, usable at compile time and at runtime.
 *
 * The list is checked the first time it is used in each translation unit,
 * not on every call. A broken list is a compile error.
 *
 * @tparam E The enumeration.
 * @tparam Version void for the base list, or a version's tag type.
 * @return The view of enum_list<E, Version>::values.
 */
template <typename E, typename Version = void>
    requires has_enum_list_c<E, Version>
consteval enum_list_view<E>
enumerators_of();
UAB_INTERNAL_NS_BEGIN

/*!
 * @brief Builds the view of enum_list<E, Version>::values, and checks it:
 * the general rules, then the ASCII identifier rule for a base list, or the
 * same-values-same-order rule for a version.
 *
 * constexpr rather than consteval, so the tests can call it at runtime on
 * broken lists and check what it throws. Its only real use is initialising
 * checked_view_v, which is constant-evaluated, so a broken list is still a
 * compile error.
 *
 * @return The checked view.
 * @throws std::invalid_argument If the list breaks a rule.
 */
    template <typename E, typename Version>
constexpr enum_list_view<E>
make_checked_view();

/*!
 * @brief The checked view of each list, so each list is checked once per
 * translation unit, however often enumerators_of is called.
 */
template <typename E, typename Version>
// Initialised with =, not braces: when the list is broken, braces make GCC
// add a misleading "no matching function" error after the real one.
inline constexpr enum_list_view<E> checked_view_v
= make_checked_view<E, Version>();
/*
 * The checks run on enumerator lists. Each throws std::invalid_argument on
 * the first problem it finds. During constant evaluation a throw is a compile
 * error, so a bad compile-time list doesn't build.
 */
 /*!
  * @brief Checks the rules every list follows, compile-time or runtime.
  *
  * -   There is at least one element.
  * -   No name is empty.
  * -   No two elements share a value.
  * -   No two elements share a name.
  *
  * The value and name checks compare every pair, so are O(n^2).
  *
  * @param elements_arg The first element.
  * @param size_arg The number of elements.
  * @throws std::invalid_argument If any rule is broken.
  */
template <typename E>
constexpr void
check_list(
    const enum_element<E>* const elements_arg,
    const std::size_t            size_arg
);

/*!
 * @brief Checks that a character can start an ASCII identifier: a letter or
 * an underscore.
 * @param char_arg The character.
 * @return True if it can.
 */
constexpr bool
is_ascii_identifier_start(const char8_t char_arg) noexcept;

/*!
 * @brief Checks that a character can continue an ASCII identifier: a letter,
 * a digit or an underscore.
 * @param char_arg The character.
 * @return True if it can.
 */
constexpr bool
is_ascii_identifier_continue(const char8_t char_arg) noexcept;

/*!
 * @brief Checks that a name is an ASCII identifier:
 * [A-Za-z_][A-Za-z0-9_]*.
 *
 * Doesn't reject keywords; a keyword can't be an enumerator, so can't be
 * listed by accident.
 *
 * @param name_arg The name.
 * @return True if it is.
 */
constexpr bool
is_ascii_identifier(const std::u8string_view name_arg) noexcept;

/*!
 * @brief Checks the extra rule for a base list (enum_list<E>): every name is
 * an ASCII identifier, so it can be the enumerator's spelling in the source.
 * @param elements_arg The first element.
 * @param size_arg The number of elements.
 * @throws std::invalid_argument If a name isn't an ASCII identifier.
 */
template <typename E>
constexpr void
check_identifier_names(
    const enum_element<E>* const elements_arg,
    const std::size_t            size_arg
);

/*!
 * @brief Checks the extra rule for a version list (enum_list<E, Version>):
 * it lists the base list's values, all of them, in the same order.
 *
 * As the base list's values are unique, this also makes the version's values
 * unique. Comparing in order is O(n).
 *
 * @param version_arg The version's first element.
 * @param version_size_arg The version's number of elements.
 * @param base_arg The base list's first element.
 * @param base_size_arg The base list's number of elements.
 * @throws std::invalid_argument If the values differ in number or order.
 */
template <typename E>
constexpr void
check_matches_base(
    const enum_element<E>* const version_arg,
    const std::size_t            version_size_arg,
    const enum_element<E>* const base_arg,
    const std::size_t            base_size_arg
);
template <typename E>
constexpr void
check_list(
    const enum_element<E>* const elements_arg,
    const std::size_t            size_arg
)
{
    if (size_arg == 0)
    {
        throw std::invalid_argument("enum_list must contain at least one entry"
        );
    }
    for (std::size_t idx{ 0 }; idx < size_arg; ++idx)
    {
        const enum_element<E>& element_idx{ elements_arg[idx] };
        if (element_idx.name.empty())
        {
            throw std::invalid_argument("enum_list names must be non-empty");
        }
        for (std::size_t jdx{ idx + 1 }; jdx < size_arg; ++jdx)
        {
            const enum_element<E>& element_jdx{ elements_arg[jdx] };
            if (element_idx.value == element_jdx.value)
            {
                throw std::invalid_argument("enum_list values must be unique");
            }
            if (element_idx.name == element_jdx.name)
            {
                throw std::invalid_argument("enum_list names must be unique");
            }
        }
    }
}

constexpr bool
is_ascii_identifier_start(
    const char8_t char_arg
) noexcept
{
    return (char_arg >= u8'a' && char_arg <= u8'z')
        || (char_arg >= u8'A' && char_arg <= u8'Z') || char_arg == u8'_';
}

constexpr bool
is_ascii_identifier_continue(
    const char8_t char_arg
) noexcept
{
    return is_ascii_identifier_start(char_arg)
        || (char_arg >= u8'0' && char_arg <= u8'9');
}

constexpr bool
is_ascii_identifier(
    const std::u8string_view name_arg
) noexcept
{
    if (name_arg.empty() || not is_ascii_identifier_start(name_arg.front()))
    {
        return false;
    }
    for (const char8_t char_arg : name_arg.substr(1))
    {
        if (not is_ascii_identifier_continue(char_arg))
        {
            return false;
        }
    }
    return true;
}

template <typename E>
constexpr void
check_identifier_names(
    const enum_element<E>* const elements_arg,
    const std::size_t            size_arg
)
{
    for (std::size_t idx{ 0 }; idx < size_arg; ++idx)
    {
        if (not is_ascii_identifier(elements_arg[idx].name))
        {
            throw std::invalid_argument("enum_list base names must be ASCII identifiers"
            );
        }
    }
}

template <typename E>
constexpr void
check_matches_base(
    const enum_element<E>* const version_arg,
    const std::size_t            version_size_arg,
    const enum_element<E>* const base_arg,
    const std::size_t            base_size_arg
)
{
    if (version_size_arg != base_size_arg)
    {
        throw std::invalid_argument("enum_list version must list every value in the base list"
        );
    }
    for (std::size_t idx{ 0 }; idx < version_size_arg; ++idx)
    {
        if (version_arg[idx].value != base_arg[idx].value)
        {
            throw std::invalid_argument("enum_list version must list values in the base list's order"
            );
        }
    }
}
UAB_INTERNAL_NS_END
UAB_NS_END

UAB_NS_BEGIN
template <typename E>
    requires std::is_enum_v<E>
constexpr enum_list_view<E>::enum_list_view(
    const element_type* const elements_arg,
    const std::size_t         size_arg
)
    : _elements(elements_arg), _size(size_arg)
{
    internal::check_list(_elements, _size);
}

template <typename E>
    requires std::is_enum_v<E>
template <std::size_t N>
constexpr enum_list_view<E>::enum_list_view(
    const element_type(&elements_arg)[N]
)
    : enum_list_view(elements_arg, N)
{
}

template <typename E>
    requires std::is_enum_v<E>
template <typename Container>
    requires contiguous_container_of_c<Container, enum_element<E>>
constexpr enum_list_view<E>::enum_list_view(
    const Container& container_arg
)
    : enum_list_view(container_arg.data(), container_arg.size())
{
}

template <typename E>
    requires std::is_enum_v<E>
constexpr std::size_t
enum_list_view<E>::size() const noexcept
{
    return _size;
}

template <typename E>
    requires std::is_enum_v<E>
constexpr const enum_list_view<E>::element_type*
enum_list_view<E>::begin() const noexcept
{
    return _elements;
}

template <typename E>
    requires std::is_enum_v<E>
constexpr const enum_list_view<E>::element_type*
enum_list_view<E>::end() const noexcept
{
    return _elements + _size;
}

template <typename E>
    requires std::is_enum_v<E>
constexpr const enum_list_view<E>::element_type&
enum_list_view<E>::operator[](
    const std::size_t index_arg
    ) const noexcept
{
    return _elements[index_arg];
}

template <typename E>
    requires std::is_enum_v<E>
constexpr const enum_list_view<E>::element_type*
enum_list_view<E>::find(
    const E value_arg
) const noexcept
{
    for (const element_type& element : *this)
    {
        if (element.value == value_arg)
        {
            return &element;
        }
    }
    return nullptr;
}

template <typename E>
    requires std::is_enum_v<E>
constexpr const enum_list_view<E>::element_type*
enum_list_view<E>::find(
    const std::u8string_view name_arg
) const noexcept
{
    for (const element_type& element : *this)
    {
        if (element.name == name_arg)
        {
            return &element;
        }
    }
    return nullptr;
}

template <typename E>
    requires std::is_enum_v<E>
constexpr std::size_t
enum_list_view<E>::index_of(
    const E value_arg
) const noexcept
{
    for (std::size_t idx{ 0 }; idx < _size; ++idx)
    {
        if (_elements[idx].value == value_arg)
        {
            return idx;
        }
    }
    return npos;
}

template <typename E>
    requires std::is_enum_v<E>
constexpr std::size_t
enum_list_view<E>::index_of(
    const std::u8string_view name_arg
) const noexcept
{
    for (std::size_t idx{ 0 }; idx < _size; ++idx)
    {
        if (_elements[idx].name == name_arg)
        {
            return idx;
        }
    }
    return npos;
}

template <typename E>
    requires std::is_enum_v<E>
constexpr std::u8string_view
enum_list_view<E>::name_of(
    const E value_arg
) const noexcept
{
    const element_type* const element{ find(value_arg) };
    if (element == nullptr)
    {
        return {};
    }
    return element->name;
}

template <typename E, typename Version>
    requires has_enum_list_c<E, Version>
consteval enum_list_view<E>
enumerators_of()
{
    return internal::checked_view_v<E, Version>;
}

UAB_INTERNAL_NS_BEGIN
template <typename E, typename Version>
constexpr enum_list_view<E>
make_checked_view()
{
    const enum_list_view<E> view(enum_list<E, Version>::values);
    if constexpr (std::is_void_v<Version>)
    {
        check_identifier_names(view.begin(), view.size());
    }
    else
    {
        const enum_list_view<E> base{ checked_view_v<E, void> };
        check_matches_base(view.begin(), view.size(), base.begin(), base.size());
    }
    return view;
}
UAB_INTERNAL_NS_END

UAB_NS_END
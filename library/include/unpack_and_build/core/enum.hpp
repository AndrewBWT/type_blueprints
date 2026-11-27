#pragma once
#include "unpack_and_build/core/extended_concepts.hpp"
#include "unpack_and_build/core/internal/macros.hpp"

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
 * Each specialisation provides values, a static constexpr built-in
 * array or std::array of enum_element<E>.
 *
 * The primary template is empty, so no enumeration is listed until
 * enum_list is specialised for it.
 *
 * The static member variable values must be filled out to work with the enum
 * infrastructure.
 *
 * @code
 * UAB_NS_BEGIN
 * template <>
 * struct enum_list<colour>
 * {
 *     static constexpr enum_element<colour> values[] = {
 *         {colour::red, u8"red"},
 *         {colour::green, u8"green"},
 *     };
 * };
 * UAB_NS_END
 * @endcode
 */
template <typename E>
struct enum_list
{};

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
    constexpr enum_list_view(const element_type (&elements_arg)[N]);
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

/*!
 * @brief Checks that there is an enum_list defined for a specific enum.
 *
 * Also ensures that enum_list_view can be constructed from the
 * enum_list<E>::values static data member.
 */
template <typename E>
concept has_enum_list_c = std::is_enum_v<E> && requires {
    enum_list_view<E>(enum_list<E>::values);
};

/*!
 * @brief An enum_list_view of an enum_list, viewable at compile time.
 * @tparam E The enum in question.
 * @return The enum_list_view created.
 */
template <typename E>
requires has_enum_list_c<E>
consteval enum_list_view<E>
    enumerators_of();

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
    using namespace std;
    if (_size == 0)
    {
        throw std::invalid_argument("enum_list must contain at least one entry"
        );
    }
    for (auto element_ptr_idx = begin(); element_ptr_idx < end();
         ++element_ptr_idx)
    {
        auto& element_idx = *element_ptr_idx;
        if (element_idx.name.empty())
        {
            throw std::invalid_argument("enum_list names must be non-empty");
        }
        for (auto element_ptr_jdx = element_ptr_idx + 1;
             element_ptr_jdx < end();
             ++element_ptr_jdx)
        {
            auto& element_jdx = *element_ptr_jdx;
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

template <typename E>
requires std::is_enum_v<E>
template <std::size_t N>
constexpr enum_list_view<E>::enum_list_view(
    const element_type (&elements_arg)[N]
)
    : enum_list_view(elements_arg, N)
{}

template <typename E>
requires std::is_enum_v<E>
template <typename Container>
requires contiguous_container_of_c<Container, enum_element<E>>
constexpr enum_list_view<E>::enum_list_view(
    const Container& container_arg
)
    : enum_list_view(container_arg.data(), container_arg.size())
{}

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
requires has_enum_list_c<E>
consteval enum_list_view<E>
    enumerators_of()
{
    return enum_list_view<E>(enum_list<E>::values);
}

UAB_NS_END
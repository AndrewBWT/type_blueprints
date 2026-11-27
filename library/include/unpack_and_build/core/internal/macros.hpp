#pragma once
/*!
 * @file
 * @brief Generic namespace-related macros for unpack_and_build.
 */
// The namespace unpack_and_build lives in. Define before including any library
// headers to change it.
#ifndef UAB_NAMESPACE
    #define UAB_NAMESPACE unpack_and_build
#endif
// The internal namespace, inside UAB_NAMESPACE, for implementation details.
#ifndef UAB_INTERNAL_NAMESPACE
    #define UAB_INTERNAL_NAMESPACE internal
#endif
// Used when wanting to refer to the unpack_and_build namespace, such as using a
// qualified name from it.
#define UAB_NS          ::UAB_NAMESPACE
// Used when wanting to refer to the unpack_and_build internal namespace.
#define UAB_INTERNAL_NS ::UAB_NAMESPACE::UAB_INTERNAL_NAMESPACE

// Begins the unpack_and_build namespace.
#define UAB_NS_BEGIN        \
    namespace UAB_NAMESPACE \
    {
// Ends the unpack_and_build namespace.
#define UAB_NS_END }
// Begins the internal unpack_and_build namespace.
#define UAB_INTERNAL_NS_BEGIN        \
    namespace UAB_INTERNAL_NAMESPACE \
    {
// Ends the internal unpack_and_build namespace.
#define UAB_INTERNAL_NS_END }

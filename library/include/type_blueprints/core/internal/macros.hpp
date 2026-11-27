#pragma once
/*!
 * @file
 * @brief Generic namespace-related macros for type_blueprints.
 */
// The namespace type_blueprints lives in. Define before including any library
// headers to change it.
#ifndef TYPE_BLUEPRINTS_NAMESPACE
    #define TYPE_BLUEPRINTS_NAMESPACE type_blueprints
#endif
// The internal namespace, inside TYPE_BLUEPRINTS_NAMESPACE, for implementation details.
#ifndef TYPE_BLUEPRINTS_INTERNAL_NAMESPACE
    #define TYPE_BLUEPRINTS_INTERNAL_NAMESPACE internal
#endif
// Used when wanting to refer to the type_blueprints namespace, such as using a
// qualified name from it.
#define TYPE_BLUEPRINTS_NS          ::TYPE_BLUEPRINTS_NAMESPACE
// Used when wanting to refer to the type_blueprints internal namespace.
#define TYPE_BLUEPRINTS_INTERNAL_NS ::TYPE_BLUEPRINTS_NAMESPACE::TYPE_BLUEPRINTS_INTERNAL_NAMESPACE

// Begins the type_blueprints namespace.
#define TYPE_BLUEPRINTS_NS_BEGIN        \
    namespace TYPE_BLUEPRINTS_NAMESPACE \
    {
// Ends the type_blueprints namespace.
#define TYPE_BLUEPRINTS_NS_END }
// Begins the internal type_blueprints namespace.
#define TYPE_BLUEPRINTS_INTERNAL_NS_BEGIN        \
    namespace TYPE_BLUEPRINTS_INTERNAL_NAMESPACE \
    {
// Ends the internal type_blueprints namespace.
#define TYPE_BLUEPRINTS_INTERNAL_NS_END }

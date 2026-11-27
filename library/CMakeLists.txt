# Guard against rebuilding by accident.
if(NOT DEFINED TYPE_BLUEPRINTS_LIBRARY_INCLUDED)
  set(TYPE_BLUEPRINTS_LIBRARY_INCLUDED TRUE)
  # Set library name so that, if we change it, we only have to change this line
  set(TYPE_BLUEPRINTS_LIBRARY_NAME type_blueprints)
  set(TYPE_BLUEPRINTS_NESTED_LIBRARY_NAME type_blueprints::type_blueprints)

  # List of all the header files.
  set(TYPE_BLUEPRINTS_INTERNAL_HEADERS
  # Contains the macros for the namespaces. 
    include/type_blueprints/core/internal/macros.hpp
  # Exports everything except the appender specialisations.
    include/type_blueprints/core.hpp
  # # Contains generic concepts that help when working with the standard library.
    include/type_blueprints/core/extended_concepts.hpp
  # Allows a generic way of obtaining the underlying structure in an adaptor.
    include/type_blueprints/core/adaptors.hpp
  # Contains the enum_list definitions, and the associated functions.
    include/type_blueprints/core/enum.hpp
  # The object_traits class, and functions to work with it.
    include/type_blueprints/core/object.hpp
  # Adds functionality when working with tuples.
    include/type_blueprints/core/tuples.hpp
  # Concepts and traits for the libraries version of a sequence. 
    include/type_blueprints/core/sequence_traits.hpp
  # Function that acts like ranges::to, but works with the appenders machineary.
    include/type_blueprints/core/build_from.hpp
  # Concepts for default appending functionality.
    include/type_blueprints/core/appenders.hpp
  # Contains the result type given when appending a value to a container.
    include/type_blueprints/core/appenders/append_result_enum.hpp
  # The fixed sized appender - think std::arrays, built-in arrays etc.
    include/type_blueprints/core/appenders/fixed_size.hpp
  # For objects that have an insert operation that may fail - e.g. std::set.
    include/type_blueprints/core/appenders/unique_insert.hpp
  # For sequences that have an insert that always succeeds - e.g. std::multiset.
    include/type_blueprints/core/appenders/multi_insert.hpp
  # For sequences that efficiently insert using an iterator from the last insertion - e.g. std::forward_list. 
    include/type_blueprints/core/appenders/insert_after.hpp
  # Sequences that have a push_back operation
    include/type_blueprints/core/appenders/push_back.hpp
  # A placeholder is used for types that aren't default constructable.
    include/type_blueprints/core/placeholders.hpp
  # The specializations for each class in the std.
    include/type_blueprints/support/built_in_array.hpp
    include/type_blueprints/support/std/array.hpp
    include/type_blueprints/support/std/bitset.hpp
    include/type_blueprints/support/std/compare.hpp
    include/type_blueprints/support/std/deque.hpp
    include/type_blueprints/support/std/expected.hpp
    include/type_blueprints/support/std/forward_list.hpp
    include/type_blueprints/support/std/list.hpp
    include/type_blueprints/support/std/map.hpp
    include/type_blueprints/support/std/multimap.hpp
    include/type_blueprints/support/std/multiset.hpp
    include/type_blueprints/support/std/pair.hpp
    include/type_blueprints/support/std/priority_queue.hpp
    include/type_blueprints/support/std/queue.hpp
    include/type_blueprints/support/std/set.hpp
    include/type_blueprints/support/std/stack.hpp
    include/type_blueprints/support/std/string.hpp
    include/type_blueprints/support/std/tuple.hpp
    include/type_blueprints/support/std/unordered_map.hpp
    include/type_blueprints/support/std/unordered_multimap.hpp
    include/type_blueprints/support/std/unordered_multiset.hpp
    include/type_blueprints/support/std/unordered_set.hpp
    include/type_blueprints/support/std/unexpected.hpp
    include/type_blueprints/support/std/valarray.hpp
    include/type_blueprints/support/std/variant.hpp
    include/type_blueprints/support/std/vector.hpp
  )

  # Sets how the IDE organises the files.
  source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/.. FILES
    ${TYPE_BLUEPRINTS_INTERNAL_HEADERS}
  )

  # Header-only: an INTERFACE library has nothing to compile or link, so CMake
  # never needs to know a linker language. Every property must be INTERFACE.
  add_library(${TYPE_BLUEPRINTS_LIBRARY_NAME} INTERFACE)
  add_library(${TYPE_BLUEPRINTS_NESTED_LIBRARY_NAME} ALIAS ${TYPE_BLUEPRINTS_LIBRARY_NAME})

  # The file set puts include/ on consumers' include path (build tree), lists
  # the headers in the IDE, and lets install() keep the folder structure.
  target_sources(${TYPE_BLUEPRINTS_LIBRARY_NAME}
    PUBLIC
      FILE_SET public_headers TYPE HEADERS
      BASE_DIRS include
      FILES ${TYPE_BLUEPRINTS_INTERNAL_HEADERS}
  )

  # Anyone including the headers needs C++23.
  target_compile_features(${TYPE_BLUEPRINTS_LIBRARY_NAME} INTERFACE cxx_std_23)
endif()
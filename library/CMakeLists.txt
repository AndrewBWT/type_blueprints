# Guard against rebuilding by accident.
if(NOT DEFINED UNPACK_AND_BUILD_LIBRARY_INCLUDED)
  set(UNPACK_AND_BUILD_LIBRARY_INCLUDED TRUE)
  # Set library name so that, if we change it, we only have to change this line
  set(UNPACK_AND_BUILD_LIBRARY_NAME unpack_and_build)
  set(UNPACK_AND_BUILD_NESTED_LIBRARY_NAME unpack_and_build::unpack_and_build)

  # List of all the header files.
  set(UNPACK_AND_BUILD_INTERNAL_HEADERS
  # Contains the macros for the namespaces. 
    include/unpack_and_build/core/internal/macros.hpp
  # Exports everything except the appender specialisations.
    include/unpack_and_build/core.hpp
  # # Contains generic concepts that help when working with the standard library.
    include/unpack_and_build/core/extended_concepts.hpp
  # Allows a generic way of obtaining the underlying structure in an adaptor.
    include/unpack_and_build/core/adaptors.hpp
  # Contains the enum_list definitions, and the associated functions.
    include/unpack_and_build/core/enum.hpp
  # The object_traits class, and functions to work with it.
    include/unpack_and_build/core/object.hpp
  # Adds functionality when working with tuples.
    include/unpack_and_build/core/tuples.hpp
  # Concepts and traits for the libraries version of a sequence. 
    include/unpack_and_build/core/sequence_traits.hpp
  # Function that acts like ranges::to, but works with the appenders machineary.
    include/unpack_and_build/core/build_from.hpp
  # Concepts for default appending functionality.
    include/unpack_and_build/core/appenders.hpp
  # Contains the result type given when appending a value to a container.
    include/unpack_and_build/core/appenders/append_result_enum.hpp
  # The fixed sized appender - think std::arrays, built-in arrays etc.
    include/unpack_and_build/core/appenders/fixed_size.hpp
  # For objects that have an insert operation that may fail - e.g. std::set.
    include/unpack_and_build/core/appenders/unique_insert.hpp
  # For sequences that have an insert that always succeeds - e.g. std::multiset.
    include/unpack_and_build/core/appenders/multi_insert.hpp
  # For sequences that efficiently insert using an iterator from the last insertion - e.g. std::forward_list. 
    include/unpack_and_build/core/appenders/insert_after.hpp
  # Sequences that have a push_back operation
    include/unpack_and_build/core/appenders/push_back.hpp
  # A placeholder is used for types that aren't default constructable.
    include/unpack_and_build/core/placeholders.hpp
  # The specializations for each class in the std.
    include/unpack_and_build/support/built_in_array.hpp
    include/unpack_and_build/support/std/array.hpp
    include/unpack_and_build/support/std/bitset.hpp
    include/unpack_and_build/support/std/compare.hpp
    include/unpack_and_build/support/std/deque.hpp
    include/unpack_and_build/support/std/expected.hpp
    include/unpack_and_build/support/std/forward_list.hpp
    include/unpack_and_build/support/std/list.hpp
    include/unpack_and_build/support/std/map.hpp
    include/unpack_and_build/support/std/multimap.hpp
    include/unpack_and_build/support/std/multiset.hpp
    include/unpack_and_build/support/std/pair.hpp
    include/unpack_and_build/support/std/priority_queue.hpp
    include/unpack_and_build/support/std/queue.hpp
    include/unpack_and_build/support/std/set.hpp
    include/unpack_and_build/support/std/stack.hpp
    include/unpack_and_build/support/std/string.hpp
    include/unpack_and_build/support/std/tuple.hpp
    include/unpack_and_build/support/std/unordered_map.hpp
    include/unpack_and_build/support/std/unordered_multimap.hpp
    include/unpack_and_build/support/std/unordered_multiset.hpp
    include/unpack_and_build/support/std/unordered_set.hpp
    include/unpack_and_build/support/std/unexpected.hpp
    include/unpack_and_build/support/std/valarray.hpp
    include/unpack_and_build/support/std/variant.hpp
    include/unpack_and_build/support/std/vector.hpp
  )

  # Sets how the IDE organises the files.
  source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/.. FILES
    ${UNPACK_AND_BUILD_INTERNAL_HEADERS}
  )

  # Header-only: an INTERFACE library has nothing to compile or link, so CMake
  # never needs to know a linker language. Every property must be INTERFACE.
  add_library(${UNPACK_AND_BUILD_LIBRARY_NAME} INTERFACE)
  add_library(${UNPACK_AND_BUILD_NESTED_LIBRARY_NAME} ALIAS ${UNPACK_AND_BUILD_LIBRARY_NAME})

  # The file set puts include/ on consumers' include path (build tree), lists
  # the headers in the IDE, and lets install() keep the folder structure.
  target_sources(${UNPACK_AND_BUILD_LIBRARY_NAME}
    PUBLIC
      FILE_SET public_headers TYPE HEADERS
      BASE_DIRS include
      FILES ${UNPACK_AND_BUILD_INTERNAL_HEADERS}
  )

  # Anyone including the headers needs C++23.
  target_compile_features(${UNPACK_AND_BUILD_LIBRARY_NAME} INTERFACE cxx_std_23)
endif()
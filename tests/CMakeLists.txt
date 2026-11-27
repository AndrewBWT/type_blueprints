# Guard against rebuilding by accident.
if(NOT DEFINED TYPE_BLUEPRINTS_TESTS_INCLUDED)
  set(TYPE_BLUEPRINTS_TESTS_INCLUDED TRUE)
  # Set library name so that, if we change it, we only have to change this line
  # Set the name of the example executable once
  set(TYPE_BLUEPRINTS_TESTS_EXECUTABLE_NAME type_blueprints_tests)

  # The set of all test files. The idea behind these is they mirror the files in the library.
  # However we may change this as we move forward.

  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1  # pin to a specific release
  )
  FetchContent_MakeAvailable(Catch2)
  # These are the libs required for the testing suite.
  set(TYPE_BLUEPRINTS_TESTS_REQUIRED_LIBS
    Catch2::Catch2
    type_blueprints::type_blueprints
  )

  #There are some additional entries at the end which are not mirrored by files in the library.
  set(TYPE_BLUEPRINTS_TESTS_FILES
  # Mirrors library/include/type_blueprints/core.hpp and core/.
    include/type_blueprints_tests/core.hpp
    include/type_blueprints_tests/core/adaptors.hpp
    include/type_blueprints_tests/core/appenders.hpp
    include/type_blueprints_tests/core/appenders/append_result_enum.hpp
    include/type_blueprints_tests/core/appenders/fixed_size.hpp
    include/type_blueprints_tests/core/appenders/insert_after.hpp
    include/type_blueprints_tests/core/appenders/multi_insert.hpp
    include/type_blueprints_tests/core/appenders/push_back.hpp
    include/type_blueprints_tests/core/appenders/unique_insert.hpp
    include/type_blueprints_tests/core/build_from.hpp
    include/type_blueprints_tests/core/enum.hpp
    include/type_blueprints_tests/core/extended_concepts.hpp
    include/type_blueprints_tests/core/object.hpp
    include/type_blueprints_tests/core/placeholders.hpp
    include/type_blueprints_tests/core/sequence_traits.hpp
    include/type_blueprints_tests/core/tuples.hpp
  # Mirrors library/include/type_blueprints/support/.
    include/type_blueprints_tests/support.hpp
    include/type_blueprints_tests/support/built_in_array.hpp
    include/type_blueprints_tests/support/std/array.hpp
    include/type_blueprints_tests/support/std/bitset.hpp
    include/type_blueprints_tests/support/std/compare.hpp
    include/type_blueprints_tests/support/std/deque.hpp
    include/type_blueprints_tests/support/std/expected.hpp
    include/type_blueprints_tests/support/std/forward_list.hpp
    include/type_blueprints_tests/support/std/list.hpp
    include/type_blueprints_tests/support/std/map.hpp
    include/type_blueprints_tests/support/std/multimap.hpp
    include/type_blueprints_tests/support/std/multiset.hpp
    include/type_blueprints_tests/support/std/pair.hpp
    include/type_blueprints_tests/support/std/priority_queue.hpp
    include/type_blueprints_tests/support/std/queue.hpp
    include/type_blueprints_tests/support/std/set.hpp
    include/type_blueprints_tests/support/std/stack.hpp
    include/type_blueprints_tests/support/std/string.hpp
    include/type_blueprints_tests/support/std/tuple.hpp
    include/type_blueprints_tests/support/std/unexpected.hpp
    include/type_blueprints_tests/support/std/unordered_map.hpp
    include/type_blueprints_tests/support/std/unordered_multimap.hpp
    include/type_blueprints_tests/support/std/unordered_multiset.hpp
    include/type_blueprints_tests/support/std/unordered_set.hpp
    include/type_blueprints_tests/support/std/valarray.hpp
    include/type_blueprints_tests/support/std/variant.hpp
    include/type_blueprints_tests/support/std/vector.hpp
    src/main.cpp
  )
  
  # Sets how the IDE organises the files.
  source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/.. FILES
    ${TYPE_BLUEPRINTS_TESTS_FILES}
  )

  # Add the set of files to executable.
  add_executable(${TYPE_BLUEPRINTS_TESTS_EXECUTABLE_NAME} ${TYPE_BLUEPRINTS_TESTS_FILES})
  
  set_common_compile_options(${TYPE_BLUEPRINTS_TESTS_EXECUTABLE_NAME})
  
  # Add the include directory to the target.
  target_include_directories(${TYPE_BLUEPRINTS_TESTS_EXECUTABLE_NAME} PRIVATE include)
  # Link the REQUIRED_LIBS to the LIBRARY.
  target_link_libraries(${TYPE_BLUEPRINTS_TESTS_EXECUTABLE_NAME} PUBLIC ${TYPE_BLUEPRINTS_TESTS_REQUIRED_LIBS})

  # The library's headers, as listed in its file set. Uses the real target
  # name: get_target_property doesn't accept aliases.
  get_target_property(TYPE_BLUEPRINTS_HEADERS type_blueprints HEADER_SET_public_headers)

  # Header checks: one generated translation unit per header, containing only
  # that header's #include. Checks every header compiles on its own (no
  # missing includes hidden by main.cpp).
  foreach(header IN LISTS TYPE_BLUEPRINTS_HEADERS)
    string(MAKE_C_IDENTIFIER "${header}" header_id)
    set(check_file "${CMAKE_CURRENT_BINARY_DIR}/header_checks/${header_id}.cpp")
    file(CONFIGURE OUTPUT "${check_file}" CONTENT "#include \"${header}\"\n")
    list(APPEND TYPE_BLUEPRINTS_HEADER_CHECKS "${check_file}")
  endforeach()
  add_library(type_blueprints_header_checks OBJECT ${TYPE_BLUEPRINTS_HEADER_CHECKS})
  target_link_libraries(type_blueprints_header_checks PRIVATE type_blueprints::type_blueprints)
  set_common_compile_options(type_blueprints_header_checks)
  # The generated files live in the build folder, so they get a flat group.
  source_group("Header checks" FILES ${TYPE_BLUEPRINTS_HEADER_CHECKS})

  # The library is header-only (an INTERFACE target), so it has no compile
  # settings of its own, and IntelliSense falls back to defaults for its
  # headers. Listing them in this compiled target gives them its settings,
  # and keeps them out of the tests project.
  target_sources(type_blueprints_header_checks PRIVATE ${TYPE_BLUEPRINTS_HEADERS})
  source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../library/include FILES ${TYPE_BLUEPRINTS_HEADERS})
  
  # Adds the test as a command.
  add_test(NAME "Run type_blueprints test suite executable"
           COMMAND ${TYPE_BLUEPRINTS_TESTS_EXECUTABLE_NAME} "--config" ${TYPE_BLUEPRINTS_TEST_RUN_ARGS})
endif()
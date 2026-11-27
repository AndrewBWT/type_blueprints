# Guard against rebuilding by accident.
if(NOT DEFINED UNPACK_AND_BUILD_TESTS_INCLUDED)
  set(UNPACK_AND_BUILD_TESTS_INCLUDED TRUE)
  # Set library name so that, if we change it, we only have to change this line
  # Set the name of the example executable once
  set(UNPACK_AND_BUILD_TESTS_EXECUTABLE_NAME unpack_and_build_tests)

  # The set of all test files. The idea behind these is they mirror the files in the library.
  # However we may change this as we move forward.

  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1  # pin to a specific release
  )
  FetchContent_MakeAvailable(Catch2)
  # These are the libs required for the testing suite.
  set(UNPACK_AND_BUILD_TESTS_REQUIRED_LIBS
    Catch2::Catch2
    unpack_and_build::unpack_and_build
  )

  #There are some additional entries at the end which are not mirrored by files in the library.
  set(UNPACK_AND_BUILD_TESTS_FILES
  # Mirrors library/include/unpack_and_build/core.hpp and core/.
    include/unpack_and_build_tests/core.hpp
    include/unpack_and_build_tests/core/adaptors.hpp
    include/unpack_and_build_tests/core/appenders.hpp
    include/unpack_and_build_tests/core/appenders/append_result_enum.hpp
    include/unpack_and_build_tests/core/appenders/fixed_size.hpp
    include/unpack_and_build_tests/core/appenders/insert_after.hpp
    include/unpack_and_build_tests/core/appenders/multi_insert.hpp
    include/unpack_and_build_tests/core/appenders/push_back.hpp
    include/unpack_and_build_tests/core/appenders/unique_insert.hpp
    include/unpack_and_build_tests/core/build_from.hpp
    include/unpack_and_build_tests/core/enum.hpp
    include/unpack_and_build_tests/core/extended_concepts.hpp
    include/unpack_and_build_tests/core/object.hpp
    include/unpack_and_build_tests/core/placeholders.hpp
    include/unpack_and_build_tests/core/sequence_traits.hpp
    include/unpack_and_build_tests/core/tuples.hpp
  # Mirrors library/include/unpack_and_build/support/.
    include/unpack_and_build_tests/support.hpp
    include/unpack_and_build_tests/support/built_in_array.hpp
    include/unpack_and_build_tests/support/std/array.hpp
    include/unpack_and_build_tests/support/std/bitset.hpp
    include/unpack_and_build_tests/support/std/compare.hpp
    include/unpack_and_build_tests/support/std/deque.hpp
    include/unpack_and_build_tests/support/std/expected.hpp
    include/unpack_and_build_tests/support/std/forward_list.hpp
    include/unpack_and_build_tests/support/std/list.hpp
    include/unpack_and_build_tests/support/std/map.hpp
    include/unpack_and_build_tests/support/std/multimap.hpp
    include/unpack_and_build_tests/support/std/multiset.hpp
    include/unpack_and_build_tests/support/std/pair.hpp
    include/unpack_and_build_tests/support/std/priority_queue.hpp
    include/unpack_and_build_tests/support/std/queue.hpp
    include/unpack_and_build_tests/support/std/set.hpp
    include/unpack_and_build_tests/support/std/stack.hpp
    include/unpack_and_build_tests/support/std/string.hpp
    include/unpack_and_build_tests/support/std/tuple.hpp
    include/unpack_and_build_tests/support/std/unexpected.hpp
    include/unpack_and_build_tests/support/std/unordered_map.hpp
    include/unpack_and_build_tests/support/std/unordered_multimap.hpp
    include/unpack_and_build_tests/support/std/unordered_multiset.hpp
    include/unpack_and_build_tests/support/std/unordered_set.hpp
    include/unpack_and_build_tests/support/std/valarray.hpp
    include/unpack_and_build_tests/support/std/variant.hpp
    include/unpack_and_build_tests/support/std/vector.hpp
    src/main.cpp
  )
  
  # Sets how the IDE organises the files.
  source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/.. FILES
    ${UNPACK_AND_BUILD_TESTS_FILES}
  )

  # Add the set of files to executable.
  add_executable(${UNPACK_AND_BUILD_TESTS_EXECUTABLE_NAME} ${UNPACK_AND_BUILD_TESTS_FILES})
  
  set_common_compile_options(${UNPACK_AND_BUILD_TESTS_EXECUTABLE_NAME})
  
  # Add the include directory to the target.
  target_include_directories(${UNPACK_AND_BUILD_TESTS_EXECUTABLE_NAME} PRIVATE include)
  # Link the REQUIRED_LIBS to the LIBRARY.
  target_link_libraries(${UNPACK_AND_BUILD_TESTS_EXECUTABLE_NAME} PUBLIC ${UNPACK_AND_BUILD_TESTS_REQUIRED_LIBS})

  # The library's headers, as listed in its file set. Uses the real target
  # name: get_target_property doesn't accept aliases.
  get_target_property(UNPACK_AND_BUILD_HEADERS unpack_and_build HEADER_SET_public_headers)

  # Header checks: one generated translation unit per header, containing only
  # that header's #include. Checks every header compiles on its own (no
  # missing includes hidden by main.cpp).
  foreach(header IN LISTS UNPACK_AND_BUILD_HEADERS)
    string(MAKE_C_IDENTIFIER "${header}" header_id)
    set(check_file "${CMAKE_CURRENT_BINARY_DIR}/header_checks/${header_id}.cpp")
    file(CONFIGURE OUTPUT "${check_file}" CONTENT "#include \"${header}\"\n")
    list(APPEND UNPACK_AND_BUILD_HEADER_CHECKS "${check_file}")
  endforeach()
  add_library(unpack_and_build_header_checks OBJECT ${UNPACK_AND_BUILD_HEADER_CHECKS})
  target_link_libraries(unpack_and_build_header_checks PRIVATE unpack_and_build::unpack_and_build)
  set_common_compile_options(unpack_and_build_header_checks)
  # The generated files live in the build folder, so they get a flat group.
  source_group("Header checks" FILES ${UNPACK_AND_BUILD_HEADER_CHECKS})

  # The library is header-only (an INTERFACE target), so it has no compile
  # settings of its own, and IntelliSense falls back to defaults for its
  # headers. Listing them in this compiled target gives them its settings,
  # and keeps them out of the tests project.
  target_sources(unpack_and_build_header_checks PRIVATE ${UNPACK_AND_BUILD_HEADERS})
  source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../library/include FILES ${UNPACK_AND_BUILD_HEADERS})
  
  # Adds the test as a command.
  add_test(NAME "Run unpack_and_build test suite executable"
           COMMAND ${UNPACK_AND_BUILD_TESTS_EXECUTABLE_NAME} "--config" ${UNPACK_AND_BUILD_TEST_RUN_ARGS})
endif()
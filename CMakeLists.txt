function(set_common_compile_options TARGET)
  target_compile_features(${TARGET} PUBLIC cxx_std_23)
  
  target_compile_options(${TARGET} PRIVATE
    "$<$<CXX_COMPILER_ID:MSVC>:/bigobj>"
    "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
  )
  if(MSVC)
#    target_compile_options(${TARGET} PRIVATE /W4 /permissive- /bigobj /utf-8)
  else()
#    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -pedantic)
  endif()
  set_target_properties(${TARGET} PROPERTIES CXX_EXTENSIONS OFF)
endfunction()

# Current CMake version. We have not tested with other versions.
cmake_minimum_required(VERSION 3.28)

# Use FetchContent to get libraries etc.
include(FetchContent)

# Set up the project information
project(
  unpack_and_build
  VERSION 			0.0.1
  DESCRIPTION 		"A C++ library for unpacking, building and simplifying generic data code creation around enums, objects, and collections."
  LANGUAGES 		CXX
)

# Set up the variables TOP_LEVEL_PROJECT and RUNNING_TESTS

# UNPACK_AND_BUILD_TOP_LEVEL_PROJECT informs the user if this is the top level project.
# If it is the top level project, we assume that it is being built on its own.
set(UNPACK_AND_BUILD_TOP_LEVEL_PROJECT FALSE)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(UNPACK_AND_BUILD_TOP_LEVEL_PROJECT TRUE)
endif()

# We include this here so that the variable BUILD_TESTING is now defined as TRUE (due to its inclusion by CTest).
# Remember, the user can set BUILD_TESTING themselves if they so wish, include(CTest) won't override that value.
if(UNPACK_AND_BUILD_TOP_LEVEL_PROJECT)
  include(CTest)
endif()

# Ensures that BUILD_TESTING is defined as FALSE. If UNPACK_AND_BUILD_TOP_LEVEL_PROJECT == FALSE, it would be an undefined value.
if(NOT DEFINED BUILD_TESTING)
  set(BUILD_TESTING OFF)
endif()

# Sets up the UNPACK_AND_BUILD_RUNNING_TESTS variable.
set(UNPACK_AND_BUILD_RUNNING_TESTS FALSE)
if(UNPACK_AND_BUILD_TOP_LEVEL_PROJECT AND BUILD_TESTING)
  set(UNPACK_AND_BUILD_RUNNING_TESTS TRUE)
endif()

# Set up the options for the user.
option(UNPACK_AND_BUILD_ENABLE_DOCS "Build unpack_and_build documentation" ${UNPACK_AND_BUILD_TOP_LEVEL_PROJECT})
option(UNPACK_AND_BUILD_ENABLE_TESTS "Build unpack_and_build tests" ${UNPACK_AND_BUILD_RUNNING_TESTS})
option(UNPACK_AND_BUILD_ENABLE_EXAMPLES "Build unpack_and_build examples" ${UNPACK_AND_BUILD_TOP_LEVEL_PROJECT})
option(UNPACK_AND_BUILD_ENABLE_ALGORITHM_BENCHMARKS "Build the unpack_and_build algorithm benchmarks" ${UNPACK_AND_BUILD_TOP_LEVEL_PROJECT})

# Show the user the status of the core variables.
message(STATUS "unpack_and_build - Top level project: ${UNPACK_AND_BUILD_TOP_LEVEL_PROJECT}")
message(STATUS "unpack_and_build - Building tests: ${UNPACK_AND_BUILD_ENABLE_TESTS}")
message(STATUS "unpack_and_build - Building docs: ${UNPACK_AND_BUILD_ENABLE_DOCS}")
message(STATUS "unpack_and_build - Building examples: ${UNPACK_AND_BUILD_ENABLE_EXAMPLES}")
  
# Add the library folder.
add_subdirectory(library)

# Enable tests if required
if(UNPACK_AND_BUILD_ENABLE_TESTS)
  add_subdirectory(tests)
endif()

# Enable examples if required
#if(UNPACK_AND_BUILD_ENABLE_EXAMPLES)
#  add_subdirectory(examples)
#endif()

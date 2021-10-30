cmake_minimum_required(VERSION 3.19)

# Detect if being bundled via sub-directory.
if(NOT DEFINED PROJECT_NAME)
  set(ESL_NOT_SUBPROJECT ON)
endif()

project(esl CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(ESL_NOT_SUBPROJECT)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)

  set(ROOT_DIR ${CMAKE_SOURCE_DIR})
endif()

set(ESL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ESL_CMAKE_DIR ${ESL_DIR}/cmake)

include(CTest)
include(${ESL_CMAKE_DIR}/CPM.cmake)

message(STATUS "GENERATOR = " ${CMAKE_GENERATOR})

# Output configurations.
get_property(MULTICONF_GENERATOR GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(MULTICONF_GENERATOR)
  foreach(OUTPUTCONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG_TYPE} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG_TYPE}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG_TYPE}/lib)
  endforeach()
else()
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
  endif()
  message(STATUS "BUILD_TYPE = " ${CMAKE_BUILD_TYPE})
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

# Compiler configurations.
if(MSVC)
  include(${ESL_CMAKE_DIR}/compiler_msvc.cmake)
else()
  include(${ESL_CMAKE_DIR}/compiler_posix.cmake)
endif()

add_subdirectory(esl)

if(ESL_NOT_SUBPROJECT AND BUILD_TESTING)
  add_subdirectory(tests)
endif()

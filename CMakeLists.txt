cmake_minimum_required(VERSION 3.1)
project(LASWaveViewer)

# the version number
set(LASWaveViewer_VERSION_MAJOR 1)
set(LASWaveViewer_VERSION_MINOR 0)

# cmake configuration type
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "possible configurations" FORCE)
# In case the user does not setup CMAKE_BUILD_TYPE, assume it's Release
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "build type default to Release, set to Release to improve performance" FORCE)
endif("${CMAKE_BUILD_TYPE}" STREQUAL "")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/ ${CMAKE_MODULE_PATH})

# ---[ Release/Debug specific flags
if(WIN32 AND NOT MINGW)
  if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX "_debug")
  endif()
  if(NOT DEFINED CMAKE_RELEASE_POSTFIX)
    set(CMAKE_RELEASE_POSTFIX "_release")
  endif()
  if(NOT DEFINED CMAKE_RELWITHDEBINFO_POSTFIX)
    set(CMAKE_RELWITHDEBINFO_POSTFIX "_release")
  endif()
  if(NOT DEFINED CMAKE_MINSIZEREL_POSTFIX)
    set(CMAKE_MINSIZEREL_POSTFIX "_release")
  endif()
endif()

# set lib/dll/exe output dir
set(PROJECT_OUTPUT_LIB_DIR "${PROJECT_SOURCE_DIR}/lib")
set(PROJECT_OUTPUT_BIN_DIR "${PROJECT_SOURCE_DIR}/bin")

if(WIN32)
	foreach(config ${CMAKE_CONFIGURATION_TYPES})
	string(TOUPPER ${config} CONFIG)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} "${PROJECT_OUTPUT_LIB_DIR}/${config}")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${PROJECT_OUTPUT_BIN_DIR}/${config}")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${PROJECT_OUTPUT_BIN_DIR}/${config}")
	endforeach(config)
else(WIN32)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_OUTPUT_LIB_DIR}")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_OUTPUT_BIN_DIR}")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_OUTPUT_BIN_DIR}")
endif(WIN32)

if(WIN32)
	set(_USE_MATH_DEFINES ON)
endif(WIN32)

# find Eigen
find_package(Eigen REQUIRED)

# find Boost
include("${PROJECT_SOURCE_DIR}/cmake/Modules/find_boost.cmake")

find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Gui)

#configure a header file to pass some of cmake setting to the source code
configure_file ("${PROJECT_SOURCE_DIR}/src/lasviewer_config.h.in" "${PROJECT_BINARY_DIR}/lasviewer_config.h")

# set lib link directories
link_directories(${PROJECT_SOURCE_DIR}/lib)

# set include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_BINARY_DIR})

# add subdirectory
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
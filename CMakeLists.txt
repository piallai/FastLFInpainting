# This is the root Cmake file of FastInpainting project
# author : Pierre Allain
# see the accompanying license for info

PROJECT(FastInpaintingProject)

cmake_minimum_required(VERSION 3.0)
find_package( OpenCV REQUIRED )

IF(WIN32)
set(BOOST_LIBRARYDIR $ENV{BOOST_ROOT}/stage_x64/lib)
set(Boost_USE_STATIC_LIBS ON)
ENDIF(WIN32)
find_package(Boost COMPONENTS system filesystem thread REQUIRED)

if(COMMAND cmake_policy)
   cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

SET(CMAKE_BUILD_TYPE Release)

SET(CMAKE_VERBOSE_MAKEFILE True)

if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
endif()
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W3")
endif()

SET(LIBRARY_OUTPUT_PATH ${${PROJECT_NAME}_SOURCE_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${${PROJECT_NAME}_SOURCE_DIR}/bin)

add_subdirectory(src/src_utils)
add_subdirectory(src/src_ocv)
add_subdirectory(src/src_img_tools)
add_subdirectory(src/src_superpixel)
add_subdirectory(src/src_light_field)
add_subdirectory(src/src_core)
add_subdirectory(src/src_main)

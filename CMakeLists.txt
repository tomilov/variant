project( versatile )
cmake_minimum_required( VERSION 2.8 )

if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
        FORCE )
endif( NOT CMAKE_BUILD_TYPE )

set( CMAKE_VERBOSE_MAKEFILE ON )
set( CMAKE_COLOR_MAKEFILE ON )

set( CMAKE_CXX_COMPILER "clang++" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++1z -stdlib=libc++" )
set( CMAKE_CXX_FLAGS_RELEASE         "-Ofast -DNDEBUG=1 -march=native" )
set( CMAKE_CXX_FLAGS_MINSIZEREL      "-Os -DNDEBUG=1 -march=native" )
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO  "-O2 -gline-tables-only -DNDEBUG=1 -march=native" )
set( CMAKE_CXX_FLAGS_DEBUG           "-g -DDEBUG=1 -D_DEBUG=1 -D_GLIBCXX_DEBUG=1 -mtune=generic -march=x86-64" )

set( CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "" ) # suppress -rdynamic flag

aux_source_directory( ./ SRC_LIST )
include_directories( include/ )
#FILE( GLOB_RECURSE HEADERS "*.hpp" )
set( HEADERS
     "include/versatile.hpp"
     "include/versatile/type_traits.hpp"
     "include/versatile/recursive_wrapper.hpp"
     "include/versatile/versatile.hpp"
     "include/versatile/variant.hpp"
     "include/versatile/visit.hpp"
     "include/versatile/compare.hpp"
     "include/versatile/io.hpp"
)

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    add_definitions( -DROWS=5 -DCOLS=5 )
elseif( CMAKE_BUILD_TYPE STREQUAL "Release" )
    add_definitions( -DROWS=2 -DCOLS=2 )
elseif( CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
    add_definitions( -DROWS=2 -DCOLS=2 )
elseif( CMAKE_BUILD_TYPE STREQUAL "MinSizeRel" )
    add_definitions( -DROWS=2 -DCOLS=2 )
endif()

add_executable( ${PROJECT_NAME} ${SRC_LIST} ${HEADERS} )

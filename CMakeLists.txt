project( versatile )
cmake_minimum_required( VERSION 2.8 )

if(     CMAKE_BUILD_TYPE STREQUAL "Debug" )
    add_definitions( -DROWS=2 -DCOLS=2 )
elseif( CMAKE_BUILD_TYPE STREQUAL "Release" )
    add_definitions( -DROWS=5 -DCOLS=5 )
elseif( CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
    add_definitions( -DROWS=5 -DCOLS=5 )
elseif( CMAKE_BUILD_TYPE STREQUAL "MinSizeRel" )
    add_definitions( -DROWS=5 -DCOLS=5 )
else()
    message( STATUS "No build type selected, default to Debug" )
    set( CMAKE_BUILD_TYPE "Debug" )
    add_definitions( -DROWS=2 -DCOLS=2 )
endif()

set( CMAKE_VERBOSE_MAKEFILE ON )
set( CMAKE_COLOR_MAKEFILE   ON )

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer" )
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address" )
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address" )

set( CMAKE_CXX_COMPILER             "clang++")
set( CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} -W -Weverything -pedantic -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -ftemplate-backtrace-limit=0 -fdiagnostics-color=always" )
set( CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} -std=gnu++1z -stdlib=libc++")
set( CMAKE_CXX_FLAGS_RELEASE        "-Ofast -DNDEBUG=1 -march=native")
set( CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG=1 -march=native")
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -gline-tables-only -DNDEBUG=1 -march=native")
set( CMAKE_CXX_FLAGS_DEBUG          "-g -fno-inline -DDEBUG=1 -D_DEBUG=1 -D_GLIBCXX_DEBUG=1 -march=x86-64 -mtune=generic" )

set( CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "" ) # suppress -rdynamic flag

include_directories( include/ )
set( HEADERS
    "include/versatile.hpp"
    "include/versatile/type_traits.hpp"
    "include/versatile/wrappers.hpp"
    "include/versatile/versatile.hpp"
    "include/versatile/variant.hpp"
    "include/versatile/visit.hpp"
    "include/versatile/compare.hpp"
    "include/versatile/io.hpp"

    "boost_variant.hpp"
)

add_executable( ${PROJECT_NAME} "main.cpp" ${HEADERS} )

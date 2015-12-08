project(versatile)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftime-report -std=gnu++1z -stdlib=libc++ -W -Weverything -Wmismatched-tags -pedantic -Wfatal-errors -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -fconstexpr-steps=50000000 -ftemplate-backtrace-limit=0" )
set(CMAKE_CXX_FLAGS_RELEASE        "-Ofast -DNDEBUG=1 -march=native")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG=1 -march=native")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -gline-tables-only -DNDEBUG=1 -march=native")
set(CMAKE_CXX_FLAGS_DEBUG          "-g -fno-inline -DDEBUG=1 -D_DEBUG=1 -D_GLIBCXX_DEBUG=1 -march=x86-64 -mtune=generic")

set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "") # suppress -rdynamic flag

include_directories("include/")
set(HEADERS
    "include/versatile.hpp"
    "include/versatile/type_traits.hpp"
    "include/versatile/wrappers.hpp"
    "include/versatile/versatile.hpp"
    "include/versatile/variant.hpp"
    "include/versatile/visit.hpp"
    "include/versatile/compare.hpp"
    "include/versatile/io.hpp"

    "test/prologue.hpp"
    "test/traits.hpp"
    "test/variant.hpp"
    "test/visit.hpp"
    "test/boost_variant.hpp"
)

add_executable("versatile"     "test/versatile.cpp"     ${HEADERS})
add_executable("variant"       "test/variant.cpp"       ${HEADERS})
#add_executable("boost_variant" "test/boost_variant.cpp" ${HEADERS})
#add_executable("eggs_variant"  "test/eggs_variant.cpp"  ${HEADERS})

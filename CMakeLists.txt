project(versatile)
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE ON)

if(NOT DEFINED CMAKE_CXX_COMPILER)
    message(STATUS "No compiler specified. Default to clang++.")
    set(CMAKE_CXX_COMPILER "clang++")
endif()

include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-std=gnu++1z" COMPILER_SUPPORTS_CXX1Z)
if(NOT COMPILER_SUPPORTS_CXX1Z)
    message(FATAL_ERROR "Compiler does not support C++1z standard")
endif()

CHECK_CXX_COMPILER_FLAG("-stdlib=libc++" COMPILER_SUPPORTS_LIBCXX)
if(NOT COMPILER_SUPPORTS_LIBCXX)
    message(FATAL_ERROR "libc++ does not installed or compiler does not support libc++")
endif()

set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "") # suppress -rdynamic flag

#set(CXX_EXTENSIONS ON)
#set_property(TARGET ${TARGETS} PROPERTY CXX_STANDARD 17)
#set_property(TARGET ${TARGETS} PROPERTY CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++1z")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconstexpr-steps=50000000 -ftemplate-depth=150")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftime-report")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Weverything -Wmismatched-tags -Wuninitialized -pedantic -Wfatal-errors -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-backtrace-limit=0  -fconstexpr-backtrace-limit=0")

#set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-inline -DDEBUG=1 -D_DEBUG=1 -D_GLIBCXX_DEBUG=1 -march=x86-64 -mtune=generic")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG=1 -march=native")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -gline-tables-only -DNDEBUG=1 -march=native")
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG=1 -march=native")
else()
    message(STATUS "Wrong build type selected, default to Debug.")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

include_directories("include/")

set(HEADERS
    "include/versatile/type_traits.hpp"
    "include/versatile/in_place.hpp"
    "include/versatile/recursive_wrapper.hpp"
    "include/versatile/aggregate_wrapper.hpp"
    "include/versatile/versatile.hpp"
    "include/versatile/variant.hpp"
    "include/versatile/visit.hpp"
    "include/versatile/utility.hpp"
    "include/versatile/io.hpp"
    "include/versatile.hpp"

    "test/prologue.hpp"
)

#add_executable("test_traits"        "test/traits.cpp"        ${HEADERS} "test/traits.hpp")
#add_executable("test_versatile"     "test/versatile.cpp"     ${HEADERS} "test/utility.hpp" "test/visit.hpp" "test/wrappers.hpp" "test/versatile.hpp")
add_executable("test_variant"       "test/variant.cpp"       ${HEADERS} "test/utility.hpp" "test/visit.hpp" "test/wrappers.hpp" "test/variant.hpp")
#add_executable("test_boost_variant" "test/boost_variant.cpp" ${HEADERS} "test/utility.hpp" "test/visit.hpp" "test/boost_variant.hpp")
#add_executable("test_eggs_variant"  "test/eggs_variant.cpp"  ${HEADERS} "test/utility.hpp" "test/visit.hpp" "test/eggs_variant.hpp")

#[[
add_executable("test_ct_versatile"     "test/test_ct_versatile.cpp"     ${HEADERS} "test/deep_and_hard.hpp")
add_executable("test_ct_variant"       "test/test_ct_variant.cpp"       ${HEADERS} "test/deep_and_hard.hpp")
add_executable("test_ct_eggs_variant"  "test/test_ct_eggs_variant.cpp"  ${HEADERS} "test/deep_and_hard.hpp")
add_executable("test_ct_boost_variant" "test/test_ct_boost_variant.cpp" ${HEADERS} "test/deep_and_hard.hpp")
set(TEST_CT_TARGETS
    "test_ct_versatile"
    "test_ct_variant"
    "test_ct_eggs_variant"
    "test_ct_boost_variant"
)
foreach(t ${TEST_CT_TARGETS})
    target_compile_definitions(${t} PUBLIC "-DCOLS=5" "-DROWS=5")
endforeach()
]]

# for f in `ls build/test_*` ; do du -sh ./$f ; ./$f && echo -e "\e[1;32mSuccess! \e[0m$f" || echo -e "\e[1;31mFailure! \e[0m$f" ; done ;

#[[
message("Your C++ compiler supports these C++ features:")
foreach(i ${CMAKE_CXX_COMPILE_FEATURES})
  message("${i}")
endforeach()
target_compile_features("test_versatile" PUBLIC ${CMAKE_CXX_COMPILE_FEATURES})
]]


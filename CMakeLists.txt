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

set( CMAKE_CXX_COMPILER             "clang++"                                                                 )
set( CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} -W -Weverything -pedantic -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -ftemplate-backtrace-limit=0 -fdiagnostics-color=always" )
set( CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} -std=gnu++1z -stdlib=libc++"                          )
set( CMAKE_CXX_FLAGS_RELEASE        "-Ofast -DNDEBUG=1 -march=native"                                         )
set( CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG=1 -march=native"                                            )
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -gline-tables-only -DNDEBUG=1 -march=native"                         )
set( CMAKE_CXX_FLAGS_DEBUG          "-g -DDEBUG=1 -D_DEBUG=1 -D_GLIBCXX_DEBUG=1 -march=x86-64 -mtune=generic" )

set( CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "" ) # suppress -rdynamic flag

include_directories( include/ )
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

add_executable( ${PROJECT_NAME} "main.cpp" ${HEADERS} )

#target_compile_features( versatile PRIVATE cxx_alias_templates )
#target_compile_features( versatile PRIVATE cxx_auto_type )
#target_compile_features( versatile PRIVATE cxx_constexpr )
#target_compile_features( versatile PRIVATE cxx_decltype )
#target_compile_features( versatile PRIVATE cxx_decltype_auto )
#target_compile_features( versatile PRIVATE cxx_default_function_template_args )
#target_compile_features( versatile PRIVATE cxx_defaulted_functions )
#target_compile_features( versatile PRIVATE cxx_defaulted_move_initializers )
#target_compile_features( versatile PRIVATE cxx_delegating_constructors )
#target_compile_features( versatile PRIVATE cxx_deleted_functions )
#target_compile_features( versatile PRIVATE cxx_explicit_conversions )
#target_compile_features( versatile PRIVATE cxx_generalized_initializers )
#target_compile_features( versatile PRIVATE cxx_generic_lambdas )
#target_compile_features( versatile PRIVATE cxx_inheriting_constructors )
#target_compile_features( versatile PRIVATE cxx_lambdas )
#target_compile_features( versatile PRIVATE cxx_lambda_init_captures )
#target_compile_features( versatile PRIVATE cxx_noexcept )
#target_compile_features( versatile PRIVATE cxx_nullptr )
#target_compile_features( versatile PRIVATE cxx_range_for )
#target_compile_features( versatile PRIVATE cxx_reference_qualified_functions )
#target_compile_features( versatile PRIVATE cxx_return_type_deduction )
#target_compile_features( versatile PRIVATE cxx_rvalue_references )
#target_compile_features( versatile PRIVATE cxx_static_assert )
#target_compile_features( versatile PRIVATE cxx_strong_enums )
#target_compile_features( versatile PRIVATE cxx_uniform_initialization )
#target_compile_features( versatile PRIVATE cxx_unrestricted_unions )
#target_compile_features( versatile PRIVATE cxx_variable_templates )
#target_compile_features( versatile PRIVATE cxx_variadic_templates )
#target_compile_features( versatile PRIVATE cxx_template_template_parameters )

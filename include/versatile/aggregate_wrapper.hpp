#pragma once

#include <versatile/type_traits.hpp>

#include <type_traits>
#include <utility>
#include <memory>

namespace versatile
{

template< typename type >
struct aggregate_wrapper
    : type
{

    constexpr
    aggregate_wrapper() = default;

    template< typename argument,
              bool is_noexcept = noexcept(::new (std::declval< void * >()) type(std::declval< argument >())) >
    constexpr
    aggregate_wrapper(argument && _argument) noexcept(is_noexcept)
        : type(std::forward< argument >(_argument))
    { ; }

    template< typename ...arguments,
              bool is_noexcept = noexcept(::new (std::declval< void * >()) type{std::declval< arguments >()...}) >
    constexpr
    aggregate_wrapper(arguments &&... _arguments) noexcept(is_noexcept)
        : type{std::forward< arguments >(_arguments)...}
    { ; }

};

template< typename type >
struct unwrap_type< aggregate_wrapper< type > >
        : unwrap_type< type >
{

};

}

#pragma once

#include "type_traits.hpp"

#include <type_traits>
#include <utility>
#include <memory>

namespace versatile
{

template< typename type >
struct unwrap_type
        : identity< type >
{

};

template< typename type >
using unwrap_type_t = typename unwrap_type< std::decay_t< type > >::type;

template< typename type >
struct embrace
{

    type value_;

    constexpr
    operator type const & () const noexcept
    {
        return value_;
    }

    constexpr
    operator type & () noexcept
    {
        return value_;
    }

};

template< typename type >//std::conditional_t< std::is_class_v< type >, type, embrace< type > > >
struct aggregate_wrapper
    : type
{

    constexpr
    aggregate_wrapper() = default;

    template< typename argument,
              bool is_noexcept = noexcept(::new (nullptr) type(std::declval< argument >())) >
    constexpr
    aggregate_wrapper(argument && _argument) noexcept(is_noexcept)
        : type(std::forward< argument >(_argument))
    { ; }

    template< typename ...arguments,
              bool is_noexcept = noexcept(::new (nullptr) type{std::declval< arguments >()...}) >
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

template< typename type >
struct recursive_wrapper
{

    template< typename ...arguments,
              typename = decltype(type(std::declval< arguments >()...)) >
    recursive_wrapper(arguments &&... _arguments)
        : storage_(std::make_unique< type >(std::forward< arguments >(_arguments)...))
    { ; }

    operator type & () noexcept
    {
        return static_cast< type & >(*storage_);
    }

    operator type const & () const noexcept
    {
        return static_cast< type const & >(*storage_);
    }

private :

    std::unique_ptr< type > storage_;

};

template< typename type >
struct unwrap_type< recursive_wrapper< type > >
        : unwrap_type< type >
{

};

}

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

template< typename this_type >
class recursive_wrapper
{

    std::unique_ptr< this_type > storage_;

public :

    template< typename ...arguments,
              typename = decltype(::new (std::declval< void * >()) this_type(std::declval< arguments >()...)) >
    recursive_wrapper(arguments &&... _arguments)
        : storage_(std::make_unique< this_type >(std::forward< arguments >(_arguments)...))
    { ; }

    template< typename type >
    operator type & () noexcept
    {
        return static_cast< type & >(*storage_);
    }

    template< typename type >
    operator type const & () const noexcept
    {
        return static_cast< type const & >(*storage_);
    }

    void
    swap(recursive_wrapper & _other) noexcept
    {
        storage_.swap(_other.storage_);
    }

};

template< typename type >
struct unwrap_type< recursive_wrapper< type > >
        : unwrap_type< type >
{

};

}

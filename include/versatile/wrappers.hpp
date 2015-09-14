#pragma once

#include "type_traits.hpp"

#include <type_traits>
#include <utility>
#include <memory>

namespace versatile
{

template< typename type, typename decay_type = std::decay_t< type > >
struct unwrap_type
        : identity< decay_type >
{

};

template< typename type >
using unwrap_type_t = typename unwrap_type< type >::type;

template< typename aggregate >
struct aggregate_wrapper
    : aggregate
{

    constexpr
    aggregate_wrapper() = default;

    template< typename argument, bool is_noexcept = noexcept(aggregate(std::declval< argument >())) >
    constexpr
    aggregate_wrapper(argument && _aggregate) noexcept(is_noexcept)
        : aggregate(std::forward< argument >(_aggregate))
    { ; }

    template< typename ...arguments, bool is_noexcept = noexcept(aggregate{std::declval< arguments >()...}) >
    constexpr
    aggregate_wrapper(arguments &&... _arguments) noexcept(is_noexcept)
        : aggregate{std::forward< arguments >(_arguments)...}
    { ; }

};

template< typename aggregate_wrapper_type, typename type >
struct unwrap_type< aggregate_wrapper_type, aggregate_wrapper< type > >
        : unwrap_type< type >
{

};

template< typename type >
struct recursive_wrapper
        : identity< type >
{

    template< typename ...arguments, typename = decltype(type(std::declval< arguments >()...)) >
    recursive_wrapper(arguments &&... _arguments)
        : storage_(std::make_unique< type >(std::forward< arguments >(_arguments)...))
    { ; }

    using this_type = unwrap_type_t< type >;

    operator this_type & () noexcept
    {
        return static_cast< this_type & >(*storage_);
    }

    operator this_type const & () const noexcept
    {
        return static_cast< this_type const & >(*storage_);
    }

private :

    std::unique_ptr< type > storage_;

};

template< typename recursive_wrapper_type, typename type >
struct unwrap_type< recursive_wrapper_type, recursive_wrapper< type > >
        : unwrap_type< type >
{

};

}

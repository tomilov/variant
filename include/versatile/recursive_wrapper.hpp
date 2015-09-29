#pragma once

#include "type_traits.hpp"

#include <type_traits>
#include <utility>
#include <memory>

namespace versatile
{

template< typename type >
struct recursive_wrapper
        : identity< type >
{

    template< typename ...arguments >
    recursive_wrapper(arguments &&... _arguments)
        : storage_(std::make_unique< type >(std::forward< arguments >(_arguments)...))
    { ; }

    explicit
    operator type & () & noexcept
    {
        return *storage_;
    }

    explicit
    operator type const & () const & noexcept
    {
        return *storage_;
    }

    explicit
    operator type && () && noexcept
    {
        return std::move(*storage_);
    }

    explicit
    operator type const && () const && noexcept
    {
        return std::move(*storage_);
    }

private :

    std::unique_ptr< type > storage_;

};

template< typename type, typename = std::decay_t< type > >
struct unwrap_type
        : identity< type >
{

};

template< typename recursive_wrapper_type, typename type >
struct unwrap_type< recursive_wrapper_type, recursive_wrapper< type > >
        : identity< type >
{

};

template< typename type >
using unwrap_type_t = typename unwrap_type< type >::type;

}

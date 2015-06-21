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

    template< typename ...types >
    recursive_wrapper(types &&... _values)
        : storage_(std::make_unique< type >(std::forward< types >(_values)...))
    { ; }

    recursive_wrapper(recursive_wrapper & _rhs)
        : recursive_wrapper(static_cast< type & >(_rhs))
    { ; }

    recursive_wrapper(recursive_wrapper const & _rhs)
        : recursive_wrapper(static_cast< type const & >(_rhs))
    { ; }

    recursive_wrapper(recursive_wrapper && _rhs)
        : recursive_wrapper(static_cast< type && >(std::move(_rhs)))
    { ; }

    recursive_wrapper(recursive_wrapper const && _rhs)
        : recursive_wrapper(static_cast< type const && >(std::move(_rhs)))
    { ; }

    void
    swap(recursive_wrapper & _rhs) noexcept
    {
        storage_.swap(_rhs.storage_);
    }

    void
    operator = (recursive_wrapper & _rhs) & noexcept(std::is_nothrow_assignable< type &, type & >{})
    {
        operator type & () = _rhs;
    }

    void
    operator = (recursive_wrapper const & _rhs) & noexcept(std::is_nothrow_copy_assignable< type >{})
    {
        operator type & () = _rhs;
    }

    void
    operator = (recursive_wrapper && _rhs) & noexcept(std::is_nothrow_move_assignable< type >{})
    {
        operator type & () = std::move(_rhs);
    }

    void
    operator = (recursive_wrapper const && _rhs) & noexcept(std::is_nothrow_assignable< type &, type const && >{})
    {
        operator type & () = std::move(_rhs);
    }

    void
    operator = (type & _rhs) & noexcept(std::is_nothrow_assignable< type &, type & >{})
    {
        operator type & () = _rhs;
    }

    void
    operator = (type const & _rhs) & noexcept(std::is_nothrow_copy_assignable< type >{})
    {
        operator type & () = _rhs;
    }

    void
    operator = (type && _rhs) & noexcept(std::is_nothrow_move_assignable< type >{})
    {
        operator type & () = std::move(_rhs);
    }

    void
    operator = (type const && _rhs) & noexcept(std::is_nothrow_assignable< type &, type const && >{})
    {
        operator type & () = std::move(_rhs);
    }

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

template< typename wrapped_type >
void
swap(recursive_wrapper< wrapped_type > & _lhs, recursive_wrapper< wrapped_type > & _rhs) noexcept
{
    _lhs.swap(_rhs);
}

template< typename type >
struct is_recursive_wrapper
        : std::false_type
{

};

template< typename wrapped_type >
struct is_recursive_wrapper< recursive_wrapper< wrapped_type > >
        : std::true_type
{

};

template< typename type >
struct is_recursive_wrapper< type const >
        : is_recursive_wrapper< type >
{

};

template< typename type >
struct is_recursive_wrapper< volatile type >
        : is_recursive_wrapper< type >
{

};

template< typename type >
struct is_recursive_wrapper< volatile type const >
        : is_recursive_wrapper< type >
{

};

template< typename type, typename = std::decay_t< type > >
struct unwrap_type
        : identity< type >
{

};

template< typename recursive_wrapper_type, typename type >
struct unwrap_type< recursive_wrapper_type, recursive_wrapper< type > >
        : identity< copy_cv_reference_t< recursive_wrapper_type, typename recursive_wrapper< type >::type > >
{

};

template< typename type >
using unwrap_type_t = typename unwrap_type< type >::type;

template< typename type >
constexpr
decltype(auto)
unwrap(type && _value) noexcept
{
    return static_cast< unwrap_type_t< type && > >(std::forward< type >(_value));
}

}

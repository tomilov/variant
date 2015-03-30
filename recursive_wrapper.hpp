#pragma once

#include "type_traits.hpp"

#include <type_traits>
#include <utility>
#include <memory>

namespace variant
{

template< typename incomplete_type >
struct recursive_wrapper
{

    using type = incomplete_type;

    ~recursive_wrapper() noexcept = default;

    template< typename first, typename ...rest >
    recursive_wrapper(first && _first, rest &&... _rest)
        : storage_(std::make_unique< type >(std::forward< first >(_first), std::forward< rest >(_rest)...))
    { ; }

    recursive_wrapper()
        : storage_(std::make_unique< type >())
    { ; }

    recursive_wrapper(recursive_wrapper const & _rhs)
        : recursive_wrapper(static_cast< type const & >(_rhs))
    { ; }

    recursive_wrapper(recursive_wrapper & _rhs)
        : recursive_wrapper(static_cast< type & >(_rhs))
    { ; }

    recursive_wrapper(recursive_wrapper const && _rhs)
        : recursive_wrapper(static_cast< type const && >(_rhs))
    { ; }

    recursive_wrapper(recursive_wrapper && _rhs)
        : recursive_wrapper(static_cast< type && >(_rhs))
    { ; }

    recursive_wrapper &
    operator = (recursive_wrapper const & _rhs) &
    {
        operator type & () = static_cast< type const & >(_rhs);
        return *this;
    }

    recursive_wrapper &
    operator = (recursive_wrapper & _rhs) &
    {
        operator type & () = static_cast< type & >(_rhs);
        return *this;
    }

    recursive_wrapper &
    operator = (recursive_wrapper const && _rhs) &
    {
        operator type & () = static_cast< type const && >(_rhs);
        return *this;
    }

    recursive_wrapper &
    operator = (recursive_wrapper && _rhs) &
    {
        operator type & () = static_cast< type && >(_rhs);
        return *this;
    }

    recursive_wrapper &
    operator = (type const & _rhs) &
    {
        operator type & () = _rhs;
        return *this;
    }

    recursive_wrapper &
    operator = (type & _rhs) &
    {
        operator type & () = _rhs;
        return *this;
    }

    recursive_wrapper &
    operator = (type const && _rhs) &
    {
        operator type & () = std::move(_rhs);
        return *this;
    }

    recursive_wrapper &
    operator = (type && _rhs) &
    {
        operator type & () = std::move(_rhs);
        return *this;
    }

    void
    swap(recursive_wrapper & _rhs) noexcept
    {
        using std::swap;
        swap(storage_, _rhs.storage_);
    }

    explicit
    operator type const & () const & noexcept
    {
        return *storage_;
    }

    explicit
    operator type & () & noexcept
    {
        return *storage_;
    }

    explicit
    operator type const && () const && noexcept
    {
        return std::move(*storage_);
    }

    explicit
    operator type && () && noexcept
    {
        return std::move(*storage_);
    }

private :

    std::unique_ptr< type > storage_;

};

template< typename incomplete_type >
void
swap(recursive_wrapper< incomplete_type > & _lhs, recursive_wrapper< incomplete_type > & _rhs) noexcept
{
    _lhs.swap(_rhs);
}

template< typename type >
struct is_recursive_wrapper
        : std::false_type
{

};

template< typename incomplete_type >
struct is_recursive_wrapper< recursive_wrapper< incomplete_type > >
        : std::true_type
{

};

template< typename type >
using is_recursive_wrapper_t = typename is_recursive_wrapper< type >::type;

namespace recursive_wrapping
{

template< typename type, bool = is_recursive_wrapper_t< std::remove_cv_t< std::remove_reference_t< type > > >{} >
struct unwrap_type;

template< typename general_type >
struct unwrap_type< general_type, false >
{

    using type = general_type;

};

template< typename recursive_wrapper_type >
struct unwrap_type< recursive_wrapper_type, true >
{

    using type = copy_cv_reference_t< recursive_wrapper_type, typename std::remove_reference_t< recursive_wrapper_type >::type >;

};

}

template< typename type >
using unwrap_type_t = typename recursive_wrapping::unwrap_type< type >::type;

}

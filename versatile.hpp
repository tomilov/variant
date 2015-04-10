#pragma once

#include "type_traits.hpp"
#include "recursive_wrapper.hpp"

#include <type_traits>
#include <utility>

#include <cstddef>
#include <cassert>

namespace variant
{

template< typename ...types >
union versatile;

template<>
union versatile<>
{

    template< std::size_t _which >
    using at = void;

    template< typename type = void >
    static
    constexpr
    std::size_t
    index() noexcept
    {
        return 0;
    }

    template< typename ...arguments >
    using constructible_type = void;

    template< typename ...arguments >
    static
    constexpr
    std::size_t
    index_of_constructible() noexcept
    {
        return 0;
    }

};

template< typename first, typename ...rest >
union versatile< first, rest... >
{

    static constexpr std::size_t size = sizeof...(rest);

private :

    using head = first;
    using tail = versatile< rest... >;

    head head_;
    tail tail_;

    template< typename ...arguments >
    explicit
    constexpr
    versatile(std::true_type, arguments &&... _arguments) noexcept(std::is_nothrow_constructible< head, arguments... >{})
        : head_(std::forward< arguments >(_arguments)...)
    { ; }

    template< typename ...arguments >
    explicit
    constexpr
    versatile(std::false_type, arguments &&... _arguments) noexcept(std::is_nothrow_constructible< tail, arguments... >{})
        : tail_(std::forward< arguments >(_arguments)...)
    { ; }

public :

    using this_type = unwrap_type_t< head >;

    template< std::size_t _which = size >
    using at = std::conditional_t< (_which == size), head, typename tail::template at< _which > >;

    template< typename type = this_type >
    static
    constexpr
    std::size_t
    index() noexcept
    {
        if (std::is_same< type, this_type >{}) {
            return size;
        } else {
            return tail::template index< type >();
        }
    }

    template< typename ...arguments >
    using constructible_type = std::conditional_t< (std::is_constructible< this_type, arguments... >{}), head, typename tail::template constructible_type< arguments... > >;

    template< typename ...arguments >
    static
    constexpr
    std::size_t
    index_of_constructible() noexcept
    {
        if (std::is_constructible< this_type, arguments... >{}) {
            return size;
        } else {
            return tail::template index_of_constructible< arguments... >();
        }
    }

    ~versatile() noexcept
    {
        tail_.~tail();
    }

    explicit
    constexpr
    versatile(head const & _rhs) noexcept(std::is_nothrow_copy_constructible< head >{})
        : head_(_rhs)
    { ; }

    explicit
    constexpr
    versatile(head & _rhs) noexcept(std::is_nothrow_constructible< head, head & >{})
        : head_(_rhs)
    { ; }

    explicit
    constexpr
    versatile(head const && _rhs) noexcept(std::is_nothrow_constructible< head, head const && >{})
        : head_(std::move(_rhs))
    { ; }

    explicit
    constexpr
    versatile(head && _rhs) noexcept(std::is_nothrow_move_constructible< head >{})
        : head_(std::move(_rhs))
    { ; }

    versatile(versatile const & _rhs) = delete;
    versatile(versatile & _rhs) = delete;
    versatile(versatile const && _rhs) = delete;
    versatile(versatile && _rhs) = delete;

    template< typename argument >
    constexpr
    versatile(argument && _argument) noexcept(std::is_nothrow_constructible< versatile, typename std::is_same< std::decay_t< argument >, this_type >::type, argument >{})
        : versatile(typename std::is_same< std::decay_t< argument >, this_type >::type{}, std::forward< argument >(_argument))
    { ; }

    template< typename ...arguments >
    constexpr
    versatile(arguments &&... _arguments) noexcept(std::is_nothrow_constructible< versatile, typename std::is_constructible< this_type, arguments... >::type, arguments... >{})
        : versatile(typename std::is_constructible< this_type, arguments... >::type{}, std::forward< arguments >(_arguments)...)
    { ; }

    constexpr
    void
    operator = (head const & _rhs) & noexcept(std::is_nothrow_copy_assignable< head >{})
    {
        operator head & () = _rhs;
    }

    constexpr
    void
    operator = (head & _rhs) & noexcept(std::is_nothrow_assignable< head &, head & >{})
    {
        operator head & () = _rhs;
    }

    constexpr
    void
    operator = (head const && _rhs) & noexcept(std::is_nothrow_assignable< head &, head const && >{})
    {
        operator head & () = std::move(_rhs);
    }

    constexpr
    void
    operator = (head && _rhs) & noexcept(std::is_nothrow_move_assignable< head >{})
    {
        operator head & () = std::move(_rhs);
    }

    void operator = (versatile const & _rhs) = delete;
    void operator = (versatile & _rhs) = delete;
    void operator = (versatile const && _rhs) = delete;
    void operator = (versatile && _rhs) = delete;

    template< typename rhs >
    constexpr
    std::enable_if_t< std::is_same< std::decay_t< rhs >, this_type >{} >
    operator = (rhs && _rhs) & noexcept(std::is_nothrow_assignable< tail &, rhs >{})
    {
        operator head & () = std::forward< rhs >(_rhs);
    }

    template< typename rhs >
    constexpr
    std::enable_if_t< !std::is_same< std::decay_t< rhs >, this_type >{} >
    operator = (rhs && _rhs) & noexcept(std::is_nothrow_assignable< tail &, rhs >{})
    {
        tail_ = std::forward< rhs >(_rhs);
    }

    explicit
    constexpr
    operator head const & () const & noexcept
    {
        return head_;
    }

    template< typename type >
    explicit
    constexpr
    operator type const & () const & noexcept
    {
        return static_cast< type const & >(tail_);
    }

    explicit
    constexpr
    operator head & () & noexcept
    {
        return head_;
    }

    template< typename type >
    explicit
    constexpr
    operator type & () & noexcept
    {
        return static_cast< type & >(tail_);
    }

    explicit
    constexpr
    operator head const && () const && noexcept
    {
        return std::move(head_);
    }

    template< typename type >
    explicit
    constexpr
    operator type const && () const && noexcept
    {
        return static_cast< type const && >(std::move(tail_));
    }

    explicit
    constexpr
    operator head && () && noexcept
    {
        return std::move(head_);
    }

    template< typename type >
    explicit
    constexpr
    operator type && () && noexcept
    {
        return static_cast< type && >(std::move(tail_));
    }

};

}

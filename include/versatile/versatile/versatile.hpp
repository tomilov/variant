#pragma once

#include "type_traits.hpp"
#include "recursive_wrapper.hpp"

#include <type_traits>
#include <utility>

#include <cstddef>
#include <cassert>

namespace versatile
{

template< typename ...types >
union versatile;

template<>
union versatile<>
{

    using size_type = std::size_t;

    static constexpr size_type size = 0;

private :

    struct head
    {

        size_type which_;

    };

    head head_{size};

public :

    using this_type = void;

    template< size_type _which = 0 >
    using at = void;

    template< typename type = void >
    static
    constexpr
    size_type
    index() noexcept
    {
        return 0;
    }

    template< typename ...arguments >
    using constructible_type = void;

    template< typename ...arguments >
    static
    constexpr
    size_type
    index_of_constructible() noexcept
    {
        return 0;
    }

    static
    constexpr
    size_type
    which() noexcept
    {
        return 0;
    }

    static
    constexpr
    bool
    empty() noexcept
    {
        return true;
    }

    template< typename type = void >
    static
    constexpr
    bool
    active() noexcept
    {
        return false;
    }

};

template< typename first, typename ...rest >
union versatile< first, rest... >
{

    using size_type = std::size_t;

    static constexpr size_type size = 1 + sizeof...(rest);

private :

    struct head
    {

        size_type which_;
        first value_;

        template< typename ...arguments >
        constexpr
        head(arguments &&... _arguments) noexcept(std::is_nothrow_constructible< first, arguments... >{})
            : which_(size)
            , value_(std::forward< arguments >(_arguments)...)
        { ; }

    };

    using tail = versatile< rest... >;

    head head_;
    tail tail_;

public :

    using this_type = unwrap_type_t< first >;

    template< size_type _which = size >
    using at = std::conditional_t< (_which == size), this_type, typename tail::template at< _which > >;

    template< typename type = this_type >
    static
    constexpr
    size_type
    index() noexcept
    {
        if (std::is_same< type, this_type >{}) {
            return size;
        } else {
            return tail::template index< type >();
        }
    }

    template< typename ...arguments >
    using constructible_type = std::conditional_t< (std::is_constructible< this_type, arguments... >{}), this_type, typename tail::template constructible_type< arguments... > >;

    template< typename ...arguments >
    static
    constexpr
    size_type
    index_of_constructible() noexcept
    {
        if (std::is_constructible< this_type, arguments... >{}) {
            return size;
        } else {
            return tail::template index_of_constructible< arguments... >();
        }
    }

    constexpr
    size_type
    which() const noexcept
    {
        return head_.which_;
    }

    constexpr
    bool
    empty() const noexcept
    { // initialization by versatile<>
        return (which() == 0);
    }

    template< typename type = this_type >
    constexpr
    bool
    active() const noexcept
    {
        if (std::is_same< type, this_type >{}) {
            return (which() == size);
        } else {
            return tail_.template active< type >();
        }
    }

    ~versatile() noexcept(std::is_nothrow_destructible< head >{} && std::is_nothrow_destructible< tail >{})
    {
        if (active()) {
            head_.~head();
        } else {
            tail_.~tail();
        }
    }

private :

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

    explicit
    constexpr
    versatile(this_type const & _rhs) noexcept(std::is_nothrow_constructible< head, this_type const & >{})
        : head_(_rhs)
    { ; }

    explicit
    constexpr
    versatile(this_type & _rhs) noexcept(std::is_nothrow_constructible< head, this_type & >{})
        : head_(_rhs)
    { ; }

    explicit
    constexpr
    versatile(this_type const && _rhs) noexcept(std::is_nothrow_constructible< head, this_type const && >{})
        : head_(std::move(_rhs))
    { ; }

    explicit
    constexpr
    versatile(this_type && _rhs) noexcept(std::is_nothrow_constructible< head, this_type && >{})
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
    operator = (this_type const & _rhs) & noexcept(std::is_nothrow_copy_assignable< this_type >{})
    {
        operator this_type & () = _rhs;
    }

    constexpr
    void
    operator = (this_type & _rhs) & noexcept(std::is_nothrow_assignable< this_type &, this_type & >{})
    {
        operator this_type & () = _rhs;
    }

    constexpr
    void
    operator = (this_type const && _rhs) & noexcept(std::is_nothrow_assignable< this_type &, this_type const && >{})
    {
        operator this_type & () = std::move(_rhs);
    }

    constexpr
    void
    operator = (this_type && _rhs) & noexcept(std::is_nothrow_move_assignable< this_type >{})
    {
        operator this_type & () = std::move(_rhs);
    }

    void operator = (versatile const & _rhs) = delete;
    void operator = (versatile & _rhs) = delete;
    void operator = (versatile const && _rhs) = delete;
    void operator = (versatile && _rhs) = delete;

    template< typename rhs >
    constexpr
    void
    operator = (rhs && _rhs) & noexcept(std::is_nothrow_assignable< tail &, rhs >{})
    {
        tail_ = std::forward< rhs >(_rhs);
    }

    explicit
    constexpr
    operator this_type const & () const & noexcept
    {
        assert(active());
        return static_cast< this_type const & >(head_.value_);
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
    operator this_type & () & noexcept
    {
        assert(active());
        return static_cast< this_type & >(head_.value_);
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
    operator this_type const && () const && noexcept
    {
        assert(active());
        return static_cast< this_type const && >(std::move(head_.value_));
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
    operator this_type && () && noexcept
    {
        assert(active());
        return static_cast< this_type && >(std::move(head_.value_));
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

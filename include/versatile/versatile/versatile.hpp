#pragma once

#include "type_traits.hpp"
#include "recursive_wrapper.hpp"

#include <type_traits>
#include <utility>
#include <experimental/optional>

#include <cstddef>
#include <cassert>

namespace versatile
{

template< typename ...types >
struct versatile;

template<>
struct versatile<>
{

    versatile() = delete;

    versatile(versatile &) = delete;
    versatile(versatile const &) = delete;
    versatile(versatile &&) = delete;
    versatile(versatile const &&) = delete;

    void operator = (versatile &) = delete;
    void operator = (versatile const &) = delete;
    void operator = (versatile &&) = delete;
    void operator = (versatile const &&) = delete;

};

template< typename first, typename ...rest >
struct versatile< first, rest... >
{

    using size_type = std::size_t;

    static constexpr size_type index = sizeof...(rest); // zero-based right-to-left index of leftmost alternative type

private :

    struct head
    {

        size_type which_;
        first value_;

        template< typename ...types >
        explicit
        constexpr
        head(std::experimental::in_place_t, types &&... _values) noexcept(std::is_nothrow_constructible< first, types... >{})
            : which_{index}
            , value_(std::forward< types >(_values)...)
        { ; }

        template< typename type >
        explicit
        constexpr
        head(type && _value) noexcept(std::is_nothrow_constructible< first, type >{})
            : head(std::experimental::in_place, std::forward< type >(_value))
        { ; }

    };

    using tail = versatile< rest... >;

    union
    {

        head head_;
        tail tail_;

    };

    template< typename ...types >
    explicit
    constexpr
    versatile(std::true_type, types &&... _values) noexcept(std::is_nothrow_constructible< head, types... >{})
        : head_(std::forward< types >(_values)...)
    { ; }

    template< typename ...types >
    explicit
    constexpr
    versatile(std::false_type, types &&... _values) noexcept(std::is_nothrow_constructible< tail, types... >{})
        : tail_(std::forward< types >(_values)...)
    { ; }

public :

    using this_type = unwrap_type_t< first >;

    constexpr
    size_type
    which() const noexcept
    {
        return head_.which_;
    }

    constexpr
    versatile() noexcept(std::is_nothrow_constructible< versatile, typename std::is_default_constructible< this_type >::type, std::experimental::in_place_t >{})
        : versatile(typename std::is_default_constructible< this_type >::type{}, std::experimental::in_place)
    { ; }

    versatile(versatile &) = delete;
    versatile(versatile const &) = delete;
    versatile(versatile &&) = delete;
    versatile(versatile const &&) = delete;

    template< typename type >
    explicit
    constexpr
    versatile(type && _value) noexcept(std::is_nothrow_constructible< versatile, typename std::is_same< std::decay_t< type >, this_type >::type, type >{})
        : versatile(typename std::is_same< std::decay_t< type >, this_type >::type{}, std::forward< type >(_value))
    { ; }

    template< typename ...types >
    explicit
    constexpr
    versatile(std::experimental::in_place_t, types &&... _values) noexcept(std::is_nothrow_constructible< versatile, typename std::is_constructible< this_type, types... >::type, std::experimental::in_place_t, types... >{})
        : versatile(typename std::is_constructible< this_type, types... >::type{}, std::experimental::in_place, std::forward< types >(_values)...)
    { ; }

    void operator = (versatile &) = delete;
    void operator = (versatile const &) = delete;
    void operator = (versatile &&) = delete;
    void operator = (versatile const &&) = delete;

    template< typename type >
    constexpr
    void
    operator = (type && _value) & noexcept(std::is_nothrow_assignable< std::decay_t< type > &, type >{})
    {
        operator std::decay_t< type > & () = std::forward< type >(_value);
    }

private :

    constexpr
    bool
    active() const noexcept
    {
        return (index == which());
    }

public :

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
    operator this_type && () && noexcept
    {
        assert(active());
        //return static_cast< this_type && >(std::move(head_.value_)); // There is known clang++ bug #19917 for static_cast to rvalue reference.
        return static_cast< this_type && >(static_cast< this_type & >(head_.value_)); // workaround
    }

    template< typename type >
    explicit
    constexpr
    operator type && () && noexcept
    {
        //return static_cast< type && >(std::move(tail_)); // There is known clang++ bug #19917 for static_cast to rvalue reference.
        return static_cast< type && >(static_cast< type & >(tail_)); // workaround
    }

    explicit
    constexpr
    operator this_type const && () const && noexcept
    {
        assert(active());
        //return static_cast< this_type const && >(std::move(head_.value_)); // There is known clang++ bug #19917 for static_cast to rvalue reference.
        return static_cast< this_type const && >(static_cast< this_type const & >(head_.value_)); // workaround
    }

    template< typename type >
    explicit
    constexpr
    operator type const && () const && noexcept
    {
        //return static_cast< type const && >(std::move(tail_)); // There is known clang++ bug #19917 for static_cast to rvalue reference.
        return static_cast< type const && >(static_cast< type const & >(tail_)); // workaround
    }

    ~versatile() noexcept(std::is_nothrow_destructible< head >{} && std::is_nothrow_destructible< tail >{})
    {
        if (active()) {
            head_.~head();
        } else {
            tail_.~tail();
        }
    }

};

}

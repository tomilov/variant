#pragma once

#include "visit.hpp"

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

};

template< typename first, typename ...rest >
struct versatile< first, rest... >
{

    using size_type = std::size_t;

private :

    struct dispatcher
    {

        struct head
        {

            size_type which_;
            first value_;

            template< typename ...types >
            explicit
            constexpr
            head(std::experimental::in_place_t, types &&... _values) noexcept(std::is_nothrow_constructible< first, types... >{})
                : which_{1 + sizeof...(rest)}
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
        dispatcher(std::true_type, types &&... _values) noexcept(std::is_nothrow_constructible< head, types... >{})
            : head_(std::forward< types >(_values)...)
        { ; }

        template< typename ...types >
        explicit
        constexpr
        dispatcher(std::false_type, types &&... _values) noexcept(std::is_nothrow_constructible< tail, types... >{})
            : tail_(std::forward< types >(_values)...)
        { ; }

        ~dispatcher() noexcept(std::is_nothrow_destructible< head >{} && std::is_nothrow_destructible< tail >{})
        {
            if (1 + sizeof...(rest) == head_.which_) {
                head_.~head();
            } else {
                tail_.~tail();
            }
        }

    };

    dispatcher dispatcher_;

public :

    using this_type = unwrap_type_t< first >;

    constexpr
    size_type
    which() const noexcept
    {
        return dispatcher_.head_.which_;
    }

    template< typename type >
    static
    constexpr
    size_type
    index() noexcept
    {
        return index_by_type< unwrap_type_t< type >, this_type, unwrap_type_t< rest >..., versatile<> >();
    }

    template< typename type = this_type >
    constexpr
    bool
    active() const noexcept
    {
        return (index< unwrap_type_t< type > >() == which());
    }

    explicit
    constexpr
    versatile(versatile<> = {}) noexcept(std::is_nothrow_constructible< dispatcher, typename std::is_default_constructible< first >::type, std::experimental::in_place_t >{})
        : dispatcher_(typename std::is_default_constructible< first >::type{}, std::experimental::in_place)
    { ; }

    template< typename type, typename = std::enable_if_t< !(std::is_same< unwrap_type_t< type >, versatile >{}) > >
    explicit
    constexpr
    versatile(type && _value) noexcept(std::is_nothrow_constructible< dispatcher, typename std::is_same< unwrap_type_t< type >, this_type >::type, type >{})
        : dispatcher_(typename std::is_same< unwrap_type_t< type >, this_type >::type{}, std::forward< type >(_value))
    { ; }

    template< typename ...types >
    explicit
    constexpr
    versatile(std::experimental::in_place_t, types &&... _values) noexcept(std::is_nothrow_constructible< dispatcher, typename std::is_constructible< first, types... >::type, std::experimental::in_place_t, types... >{})
        : dispatcher_(typename std::is_constructible< first, types... >::type{}, std::experimental::in_place, std::forward< types >(_values)...)
    { ; }

    explicit
    constexpr
    operator this_type & () noexcept
    {
        assert(active());
        return static_cast< this_type & >(dispatcher_.head_.value_);
    }

    template< typename type >
    explicit
    constexpr
    operator type & () noexcept
    {
        return static_cast< type & >(dispatcher_.tail_);
    }

    explicit
    constexpr
    operator this_type const & () const noexcept
    {
        assert(active());
        return static_cast< this_type const & >(dispatcher_.head_.value_);
    }

    template< typename type >
    explicit
    constexpr
    operator type const & () const noexcept
    {
        return static_cast< type const & >(dispatcher_.tail_);
    }

};

template< typename first, typename ...rest >
struct is_visitable< versatile< first, rest... > >
        : std::true_type
{

};

}

#pragma once

#include "visit.hpp"

#include <type_traits>
#include <utility>
#include <experimental/optional>

#include <cstddef>
#include <cassert>

namespace versatile
{

using std::experimental::in_place_t;
using std::experimental::in_place;

template< typename ...types >
struct versatile;

template<>
struct versatile<>
{

};

template< typename first, typename ...rest >
struct versatile< first, rest... >
{

    static_assert(std::is_standard_layout< first >{});

    using size_type = std::size_t;

private :

    struct dispatcher
    {

        struct head
        {

            size_type which_;
            first value_;

            template< typename ...types,
                      bool is_noexcept = noexcept(first(std::declval< types >()...)) >
            explicit
            constexpr
            head(in_place_t, types &&... _values) noexcept(is_noexcept)
                : which_{1 + sizeof...(rest)}
                , value_(std::forward< types >(_values)...)
            { ; }

            template< typename type,
                      bool is_noexcept = noexcept(first(std::declval< type >())) >
            explicit
            constexpr
            head(type && _value) noexcept(is_noexcept)
                : head(in_place, std::forward< type >(_value))
            { ; }

        };

        using tail = versatile< rest... >;

        union
        {

            head head_;
            tail tail_;

        };

        dispatcher(dispatcher const &) = default;
        dispatcher(dispatcher &) = default;
        dispatcher(dispatcher &&) = default;

        template< typename ...types,
                  bool is_noexcept = noexcept(head(std::declval< types >()...)) >
        explicit
        constexpr
        dispatcher(std::true_type, types &&... _values) noexcept(is_noexcept)
            : head_(std::forward< types >(_values)...)
        { ; }

        template< typename ...types >
        explicit
        constexpr
        dispatcher(std::false_type, types &&... _values) noexcept(noexcept(tail(std::declval< types >()...)))
            : tail_(std::forward< types >(_values)...)
        { ; }

        ~dispatcher() noexcept(noexcept(std::declval< head >().~head()) && noexcept(std::declval< tail >().~tail()))
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
    static constexpr size_type index = index_by_type< unwrap_type_t< type >, this_type, unwrap_type_t< rest >..., versatile<> >();

    template< typename type = this_type >
    constexpr
    bool
    active() const noexcept
    {
        return (index< unwrap_type_t< type > > == which());
    }

    template< typename is_default_constructible = typename std::is_default_constructible< first >::type >
    explicit
    constexpr
    versatile(versatile<> = {}) noexcept(noexcept(dispatcher(is_default_constructible{}, in_place)))
        : dispatcher_(is_default_constructible{}, in_place)
    { ; }

    versatile(versatile const &) = default;
    versatile(versatile &) = default;
    versatile(versatile &&) = default;

    template< typename type,
              typename is_same = typename std::is_same< unwrap_type_t< type >, this_type >::type >
    explicit
    constexpr
    versatile(type && _value) noexcept(noexcept(dispatcher(is_same{}, std::declval< type >())))
        : dispatcher_(is_same{}, std::forward< type >(_value))
    { ; }

    template< typename ...types,
              typename is_constructible = typename std::is_constructible< first, types... >::type >
    explicit
    constexpr
    versatile(in_place_t, types &&... _values) noexcept(noexcept(dispatcher(is_constructible{}, in_place, std::declval< types >()...)))
        : dispatcher_(is_constructible{}, in_place, std::forward< types >(_values)...)
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

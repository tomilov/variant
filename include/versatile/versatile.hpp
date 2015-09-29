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

};

template< typename first, typename ...rest >
struct versatile< first, rest... >
{

    using size_type = std::size_t;

    static constexpr size_type index = sizeof...(rest); // zero-based right-to-left index of leftmost alternative type

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
            if (index == head_.which_) {
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

    explicit
    constexpr
    versatile(versatile<> = {}) noexcept(std::is_nothrow_constructible< dispatcher, typename std::is_default_constructible< this_type >::type, std::experimental::in_place_t >{})
        : dispatcher_(typename std::is_default_constructible< this_type >::type{}, std::experimental::in_place)
    { ; }

    template< typename type, typename = std::enable_if_t< !(std::is_same< std::decay_t< type >, versatile >{}) > >
    explicit
    constexpr
    versatile(type && _value) noexcept(std::is_nothrow_constructible< dispatcher, typename std::is_same< std::decay_t< type >, this_type >::type, type >{})
        : dispatcher_(typename std::is_same< std::decay_t< type >, this_type >::type{}, std::forward< type >(_value))
    { ; }

    template< typename ...types >
    explicit
    constexpr
    versatile(std::experimental::in_place_t, types &&... _values) noexcept(std::is_nothrow_constructible< dispatcher, typename std::is_constructible< this_type, types... >::type, std::experimental::in_place_t, types... >{})
        : dispatcher_(typename std::is_constructible< this_type, types... >::type{}, std::experimental::in_place, std::forward< types >(_values)...)
    { ; }

    template< typename type, typename = std::enable_if_t< !(std::is_same< std::decay_t< type >, versatile >{}) > >
    constexpr
    void
    operator = (type && _value) & noexcept(std::is_nothrow_assignable< std::decay_t< type > &, type >{})
    {
        operator std::decay_t< type > & () = std::forward< type >(_value);
    }

    explicit
    constexpr
    operator this_type & () & noexcept
    {
        assert(index == which());
        return static_cast< this_type & >(dispatcher_.head_.value_);
    }

    template< typename type >
    explicit
    constexpr
    operator type & () & noexcept
    {
        return static_cast< type & >(dispatcher_.tail_);
    }

    explicit
    constexpr
    operator this_type const & () const & noexcept
    {
        assert(index == which());
        return static_cast< this_type const & >(dispatcher_.head_.value_);
    }

    template< typename type >
    explicit
    constexpr
    operator type const & () const & noexcept
    {
        return static_cast< type const & >(dispatcher_.tail_);
    }

    explicit
    constexpr
    operator this_type && () && noexcept
    {
        assert(index == which());
        //return static_cast< this_type && >(std::move(storage_.head_.value_)); // There is known clang++ bug #19917 for static_cast to rvalue reference.
        return static_cast< this_type && >(static_cast< this_type & >(dispatcher_.head_.value_)); // workaround
    }

    template< typename type >
    explicit
    constexpr
    operator type && () && noexcept
    {
        //return static_cast< type && >(std::move(storage_.tail_)); // There is known clang++ bug #19917 for static_cast to rvalue reference.
        return static_cast< type && >(static_cast< type & >(dispatcher_.tail_)); // workaround
    }

    explicit
    constexpr
    operator this_type const && () const && noexcept
    {
        assert(index == which());
        //return static_cast< this_type const && >(std::move(head_.value_)); // There is known clang++ bug #19917 for static_cast to rvalue reference.
        return static_cast< this_type const && >(static_cast< this_type const & >(dispatcher_.head_.value_)); // workaround
    }

    template< typename type >
    explicit
    constexpr
    operator type const && () const && noexcept
    {
        //return static_cast< type const && >(std::move(storage_.tail_)); // There is known clang++ bug #19917 for static_cast to rvalue reference.
        return static_cast< type const && >(static_cast< type const & >(dispatcher_.tail_)); // workaround
    }

};

namespace details
{

template< typename visitor, typename visitable, typename visitable_type, typename ...arguments >
struct dispatcher;

template< typename visitor,
          typename visitable,
          template< typename ...types > class visitable_type,
          typename ...types,
          typename ...arguments >
struct dispatcher< visitor, visitable, visitable_type< types... >, arguments... >
{

    using first_type = unwrap_type_t< typename identity< types... >::type >;

    using result_type = result_of_t< visitor, copy_cv_reference_t< visitable, first_type >, arguments... >;

private :

    using caller_type = result_type (*)(visitor &, visitable &, arguments &...);

    template< typename type >
    static
    result_type
    caller(visitor & _visitor, visitable & _visitable, arguments &... _arguments)
    {
        //return std::forward< visitor >(_visitor)(static_cast< type >(_visitable), std::forward< arguments >(_arguments)...); // There is known clang++ bug #19917 for static_cast to rvalue reference.
        return std::forward< visitor >(_visitor)(static_cast< type >(static_cast< type & >(_visitable)), std::forward< arguments >(_arguments)...); // workaround
    }

public :

    result_type
    operator () (visitor & _visitor, visitable & _visitable, arguments &... _arguments) const
    {
        constexpr auto type_qualifier_ = type_qualifier_of< visitable && >;
        static constexpr caller_type callers_[sizeof...(types)] = {dispatcher::caller< add_qualifier_t< type_qualifier_, unwrap_type_t< types > > >...};
        assert(_visitable.which() < sizeof...(types));
        return callers_[(sizeof...(types) - 1) - _visitable.which()](_visitor, _visitable, _arguments...);
    }

};

}

template< typename visitor, typename visitable, typename ...arguments >
decltype(auto)
visit(visitor && _visitor, visitable && _visitable, arguments &&... _arguments)
{
    return details::dispatcher< visitor, visitable, std::decay_t< visitable >, arguments... >{}(_visitor, _visitable, _arguments...);
}

}

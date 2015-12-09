#pragma once

#include "visit.hpp"
#include "in_place.hpp"

#include <type_traits>
#include <utility>
#include <typeinfo>

#include <cstddef>

namespace versatile
{

template< bool is_trivially_destructible, typename ...types >
class destructor_dispatcher;

template< bool is_trivially_destructible >
struct destructor_dispatcher< is_trivially_destructible >
{

    void
    destructor(std::size_t const) const noexcept
    { ; }

};

template< typename first, typename ...rest >
class destructor_dispatcher< true, first, rest... >
{

    using head = first;
    using tail = destructor_dispatcher< true, rest... >;

    union
    {

        head head_;
        tail tail_;

    };

public :

    destructor_dispatcher() = default;

    template< typename ...arguments >
    constexpr
    destructor_dispatcher(index_t< (1 + sizeof...(rest)) >, arguments &&... _arguments)
        : head_(std::forward< arguments >(_arguments)...)
    { ; }

    template< typename ...arguments >
    constexpr
    destructor_dispatcher(arguments &&... _arguments)
        : tail_(std::forward< arguments >(_arguments)...)
    { ; }

    using this_type = unwrap_type_t< first >;

    constexpr
    operator this_type const & () const noexcept
    {
        return static_cast< this_type const & >(head_);
    }

    constexpr
    operator this_type & () noexcept
    {
        return static_cast< this_type & >(head_);
    }

    template< typename type >
    constexpr
    operator type const & () const noexcept
    {
        return tail_;
    }

    template< typename type >
    constexpr
    operator type & () noexcept
    {
        return tail_;
    }

};

template< typename first, typename ...rest >
class destructor_dispatcher< false, first, rest... >
{

    using head = first;
    //using tail = destructor_dispatcher< (std::is_trivially_destructible_v< rest > && ...), rest... >; // redundant overengeneering
    using tail = destructor_dispatcher< false, rest... >;

    union
    {

        head head_;
        tail tail_;

    };

public :

    destructor_dispatcher(destructor_dispatcher const &) = default;
    destructor_dispatcher(destructor_dispatcher &) = default;
    destructor_dispatcher(destructor_dispatcher &&) = default;

    destructor_dispatcher & operator = (destructor_dispatcher const &) = default;
    destructor_dispatcher & operator = (destructor_dispatcher &) = default;
    destructor_dispatcher & operator = (destructor_dispatcher &&) = default;

    ~destructor_dispatcher() noexcept
    {
        //tail_.~tail();
    }

    void
    destructor(std::size_t const _which) const noexcept(noexcept(head_.~head()) && noexcept(tail_.destructor(_which)))
    {
        if (_which == (1 + sizeof...(rest))) {
            head_.~head();
        } else {
            tail_.destructor(_which);
        }
    }

    destructor_dispatcher() = default;

    template< typename ...arguments >
    constexpr
    destructor_dispatcher(index_t< (1 + sizeof...(rest)) >, arguments &&... _arguments)
        : head_(std::forward< arguments >(_arguments)...)
    { ; }

    template< typename ...arguments >
    constexpr
    destructor_dispatcher(arguments &&... _arguments)
        : tail_(std::forward< arguments >(_arguments)...)
    { ; }

    using this_type = unwrap_type_t< first >;

    constexpr
    operator this_type const & () const noexcept
    {
        return static_cast< this_type const & >(head_);
    }

    constexpr
    operator this_type & () noexcept
    {
        return static_cast< this_type & >(head_);
    }

    template< typename type >
    constexpr
    operator type const & () const noexcept
    {
        return tail_;
    }

    template< typename type >
    constexpr
    operator type & () noexcept
    {
        return tail_;
    }

};

template< bool is_trivially_destructible, typename ...types >
class dispatcher;

template< typename ...types >
class dispatcher< true, types... >
{

    using storage = destructor_dispatcher< true, types... >;

    std::size_t which_;
    storage storage_;

public :

    constexpr
    std::size_t
    which() const noexcept
    {
        return which_;
    }

    using default_index = index_of_default_constructible< types... >;

    constexpr
    dispatcher()
        : dispatcher(typename default_index::type{})
    { ; }

    template< typename index, typename ...arguments >
    constexpr
    dispatcher(index, arguments &&... _arguments)
        : which_{index::value}
        , storage_(index{}, std::forward< arguments >(_arguments)...)
    { ; }

    template< typename type >
    constexpr
    operator type const & () const noexcept
    {
        return storage_;
    }

    template< typename type >
    constexpr
    operator type & () noexcept
    {
        return storage_;
    }

};

template< typename ...types >
class dispatcher< false, types... >
{

    using storage = destructor_dispatcher< false, types... >;

    std::size_t which_;
    storage storage_;

public :

    constexpr
    std::size_t
    which() const noexcept
    {
        return which_;
    }

    dispatcher(dispatcher const &) = default;
    dispatcher(dispatcher &) = default;
    dispatcher(dispatcher &&) = default;

    dispatcher & operator = (dispatcher const &) = default;
    dispatcher & operator = (dispatcher &) = default;
    dispatcher & operator = (dispatcher &&) = default;

    ~dispatcher() noexcept(noexcept(storage_.destructor(which_)))
    {
        storage_.destructor(which_);
    }

    using default_index = index_of_default_constructible< types... >;

    constexpr
    dispatcher()
        : dispatcher(typename default_index::type{})
    { ; }

    template< typename index, typename ...arguments >
    constexpr
    dispatcher(index, arguments &&... _arguments)
        : which_{index::value}
        , storage_(index{}, std::forward< arguments >(_arguments)...)
    { ; }

    template< typename type >
    constexpr
    operator type const & () const noexcept
    {
        return storage_;
    }

    template< typename type >
    constexpr
    operator type & () noexcept
    {
        return storage_;
    }

};

template< typename ...types >
using dispatcher_t = dispatcher< (std::is_trivially_destructible_v< types > && ...), types... >;

template< typename ...types >
class versatile
        : enable_default_constructor_t< types... > // akrzemi1's technique
{

    using storage = dispatcher_t< types... >;

    storage storage_;

public :

    template< typename type >
    using index_at = index_at< unwrap_type_t< type >, unwrap_type_t< types >... >;

    using default_index = typename storage::default_index;

    template< typename ...arguments >
    using index_of_constructible = get_index< std::is_constructible_v< types, arguments... >... >;

    constexpr
    std::size_t
    which() const noexcept
    {
        return storage_.which();
    }

    template< typename type >
    constexpr
    bool
    active() const noexcept
    {
        return (storage_.which() == index_at< type >::value);
    }

    versatile() = default;

    template< std::size_t i, typename ...arguments >
    explicit
    constexpr
    versatile(in_place_t (&)(index_t< i >), arguments &&... _arguments)
        : enable_default_constructor_t< types... >({})
        , storage_(index_t< i >{}, std::forward< arguments >(_arguments)...)
    { ; }

    template< typename type, typename index = typename index_at< type >::type, typename ...arguments >
    explicit
    constexpr
    versatile(in_place_t (&)(type), arguments &&... _arguments)
        : versatile(in_place< index >, std::forward< arguments >(_arguments)...)
    { ; }

    template< typename type, typename index = typename index_at< type >::type >
    constexpr
    versatile(type && _value)
        : versatile(in_place< index >, std::forward< type >(_value))
    { ; }

    template< typename ...arguments, typename index = typename index_of_constructible< arguments... >::type >
    explicit
    constexpr
    versatile(in_place_t, arguments &&... _arguments)
        : versatile(in_place< index >, std::forward< arguments >(_arguments)...)
    { ; }

    template< typename type, typename index = typename index_at< type >::type >
    constexpr
    versatile &
    operator = (type && _value) noexcept
    {
        return (*this = versatile(std::forward< type >(_value))); // http://stackoverflow.com/questions/33936295/
    }

    template< std::size_t i, typename ...arguments >
    constexpr
    void
    emplace(arguments &&... _arguments)
    {
        *this = versatile(in_place< i >, std::forward< arguments >(_arguments)...);
    }

    template< typename type, typename ...arguments >
    constexpr
    void
    emplace(arguments &&... _arguments)
    {
        *this = versatile(in_place< type >, std::forward< arguments >(_arguments)...);
    }

    template< typename ...arguments >
    constexpr
    void
    emplace(arguments &&... _arguments)
    {
        *this = versatile(in_place_v, std::forward< arguments >(_arguments)...);
    }

    constexpr
    void
    swap(versatile & _other) noexcept
    {
        versatile other_ = std::move(_other);
        _other = std::move(*this);
        *this = std::move(other_);
    }

    template< typename type, typename index = typename index_at< type >::type >
    explicit
    constexpr
    operator type const & () const
    {
        return active< type >() ? storage_ : throw std::bad_cast{};
    }

    template< typename type, typename index = typename index_at< type >::type >
    explicit
    constexpr
    operator type & ()
    {
        return active< type >() ? storage_ : throw std::bad_cast{};
    }

};

template< typename first, typename ...rest >
struct is_visitable< versatile< first, rest... > >
        : std::true_type
{

};

template< typename first, typename ...rest >
constexpr
void
swap(versatile< first, rest... > & _lhs, versatile< first, rest... > & _rhs) noexcept
{
    _lhs.swap(_rhs);
}

}

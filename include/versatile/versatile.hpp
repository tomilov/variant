#pragma once

#include <versatile/in_place.hpp>

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

    constexpr
    void
    destructor(std::size_t const) const
    { ; }

};

template< typename first, typename ...rest >
class destructor_dispatcher< true, first, rest... >
{

    union
    {

        first head_;
        destructor_dispatcher< true, rest... > tail_;

    };

public :

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
        return head_;
    }

    constexpr
    operator this_type & () noexcept
    {
        return head_;
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

    union
    {

        first head_;
        destructor_dispatcher< false, rest... > tail_;

    };

public :

    destructor_dispatcher(destructor_dispatcher const &) = default;
    destructor_dispatcher(destructor_dispatcher &) = default;
    destructor_dispatcher(destructor_dispatcher &&) = default;

    destructor_dispatcher & operator = (destructor_dispatcher const &) = default;
    destructor_dispatcher & operator = (destructor_dispatcher &) = default;
    destructor_dispatcher & operator = (destructor_dispatcher &&) = default;

    ~destructor_dispatcher() noexcept
    { ; }

    void
    destructor(std::size_t const _which) const noexcept(noexcept(head_.~first()) && noexcept(tail_.destructor(_which)))
    {
        if (_which == (1 + sizeof...(rest))) {
            head_.~first();
        } else {
            tail_.destructor(_which);
        }
    }

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
        return head_;
    }

    constexpr
    operator this_type & () noexcept
    {
        return head_;
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

    std::size_t which_;
    destructor_dispatcher< true, types... > storage_;

public :

    constexpr
    std::size_t
    which() const noexcept
    {
        return which_;
    }

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

    std::size_t which_;
    destructor_dispatcher< false, types... > storage_;

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

template< bool is_default_constructible >
struct enable_default_constructor;

template<>
struct enable_default_constructor< true >
{

    enable_default_constructor() = default;

    constexpr
    enable_default_constructor(void *)
    { ; }

};

template<>
struct enable_default_constructor< false >
{

    enable_default_constructor() = delete;

    constexpr
    enable_default_constructor(void *)
    { ; }

};

template< typename ...types >
using enable_default_constructor_t = enable_default_constructor< (std::is_default_constructible_v< types > || ...) >;

template< typename ...types >
class versatile
        : enable_default_constructor_t< types... >
{

    dispatcher_t< types... > storage_;

public :

    template< typename type >
    using index_at_t = index_at_t< unwrap_type_t< type >, unwrap_type_t< types >... >;

    template< typename ...arguments >
    using index_of_constructible_t = get_index_t< std::is_constructible_v< types, arguments... >... >;

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
        return (storage_.which() == index_at_t< type >::value);
    }

    constexpr
    versatile()
        : versatile(in_place_v)
    { ; }

    template< std::size_t i, typename ...arguments >
    explicit
    constexpr
    versatile(in_place_t (&)(index_t< i >), arguments &&... _arguments)
        : enable_default_constructor_t< types... >({})
        , storage_(index_t< i >{}, std::forward< arguments >(_arguments)...)
    { ; }

    template< typename type, typename index = index_at_t< type >, typename ...arguments >
    explicit
    constexpr
    versatile(in_place_t (&)(type), arguments &&... _arguments)
        : versatile(in_place< index >, std::forward< arguments >(_arguments)...)
    { ; }

    template< typename type, typename index = index_at_t< type > >
    constexpr
    versatile(type && _value)
        : versatile(in_place< index >, std::forward< type >(_value))
    { ; }

    template< typename ...arguments, typename index = index_of_constructible_t< arguments... > >
    explicit
    constexpr
    versatile(in_place_t, arguments &&... _arguments)
        : versatile(in_place< index >, std::forward< arguments >(_arguments)...)
    { ; }

    template< typename type, typename index = index_at_t< type > >
    constexpr
    versatile &
    operator = (type && _value) noexcept
    {
        return (*this = versatile(std::forward< type >(_value))); // http://stackoverflow.com/questions/33936295/
    }

    constexpr
    void
    swap(versatile & _other) noexcept
    {
        versatile other_ = std::move(_other);
        _other = std::move(*this);
        *this = std::move(other_);
    }

    template< typename type, typename index = index_at_t< type > >
    explicit
    constexpr
    operator type const & () const
    {
        return active< type >() ? storage_ : throw std::bad_cast{};
    }

    template< typename type, typename index = index_at_t< type > >
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

template< std::size_t i, typename ...arguments, typename first, typename ...rest >
constexpr
void
emplace(versatile< first, rest... > & _versatile, arguments &&... _arguments)
{
    _versatile = versatile< first, rest... >(in_place< i >, std::forward< arguments >(_arguments)...);
}

template< typename type, typename ...arguments, typename first, typename ...rest >
constexpr
void
emplace(versatile< first, rest... > & _versatile, arguments &&... _arguments)
{
    _versatile = versatile< first, rest... >(in_place< type >, std::forward< arguments >(_arguments)...);
}

template< typename ...arguments, typename first, typename ...rest >
constexpr
void
emplace(versatile< first, rest... > & _versatile, arguments &&... _arguments)
{
    _versatile = versatile< first, rest... >(in_place_v, std::forward< arguments >(_arguments)...);
}

}

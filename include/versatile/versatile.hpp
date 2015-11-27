#pragma once

#include "visit.hpp"

#include <type_traits>
#include <utility>
#include <experimental/optional>

#include <cstddef>
#include <cassert>

namespace versatile
{

template< bool is_trivially_destructible, typename ...types >
struct constructor;

template< bool is_trivially_destructible >
struct constructor< is_trivially_destructible >
{

    constexpr
    void
    destructor(std::size_t & _which) const noexcept
    {
        _which = 0;
    }

};

template< typename first, typename ...rest >
struct constructor< true, first, rest... >
{

    using head = first;
    using tail = constructor< true, rest... >;

    union
    {

        head head_;
        tail tail_;

    };

    constructor() = default;

    template< typename ...arguments >
    constexpr
    constructor(index_t< (sizeof...(rest) + 1) >,
                arguments &&... _arguments) noexcept(__is_nothrow_constructible(head, arguments...))
        : head_(std::forward< arguments >(_arguments)...)
    { ; }

    template< typename ...arguments >
    constexpr
    constructor(arguments &&... _arguments) noexcept(__is_nothrow_constructible(tail, arguments...))
        : tail_(std::forward< arguments >(_arguments)...)
    { ; }

    constexpr
    void
    destructor(std::size_t & _which) const noexcept
    {
        _which = 0;
    }

    using this_type = unwrap_type_t< first >;

    explicit
    constexpr
    operator this_type const & () const noexcept
    {
        return static_cast< this_type const & >(head_);
    }

    explicit
    constexpr
    operator this_type & () noexcept
    {
        return static_cast< this_type & >(head_);
    }

    template< typename type >
    explicit
    constexpr
    operator type const & () const noexcept
    {
        return static_cast< type const & >(tail_);
    }

    template< typename type >
    explicit
    constexpr
    operator type & () noexcept
    {
        return static_cast< type & >(tail_);
    }

};

template< typename first, typename ...rest >
struct constructor< false, first, rest... >
{

    using head = first;
    //using tail = constructor< (__has_trivial_destructor(rest) && ...), rest... >; // redundant overengeneering
    using tail = constructor< false, rest... >;

    union
    {

        head head_;
        tail tail_;

    };

    ~constructor() noexcept
    {
        tail_.~tail(); // trivial tail is not specially processed, because whole versatile type can't get an advantage from it
    }

    constructor() = default;

    template< typename ...arguments >
    constexpr
    constructor(index_t< (sizeof...(rest) + 1) >,
                arguments &&... _arguments) noexcept(__is_nothrow_constructible(head, arguments...))
        : head_(std::forward< arguments >(_arguments)...)
    { ; }

    template< typename ...arguments >
    constexpr
    constructor(arguments &&... _arguments) noexcept(__is_nothrow_constructible(tail, arguments...))
        : tail_(std::forward< arguments >(_arguments)...)
    { ; }

    constexpr
    void
    destructor(std::size_t & _which) const noexcept(noexcept(head_.~head()) && noexcept(tail_.destructor(_which)))
    {
        if (_which == sizeof...(rest) + 1) {
            head_.~head();
        } else {
            tail_.destructor(_which);
        }
    }

    using this_type = unwrap_type_t< first >;

    explicit
    constexpr
    operator this_type const & () const noexcept
    {
        return static_cast< this_type const & >(head_);
    }

    explicit
    constexpr
    operator this_type & () noexcept
    {
        return static_cast< this_type & >(head_);
    }

    template< typename type >
    explicit
    constexpr
    operator type const & () const noexcept
    {
        return static_cast< type const & >(tail_);
    }

    template< typename type >
    explicit
    constexpr
    operator type & () noexcept
    {
        return static_cast< type & >(tail_);
    }

};

template< bool is_trivially_destructible, bool is_trivially_constructible, typename ...types >
struct destructor;

template< bool is_trivially_destructible, bool is_trivially_constructible >
struct destructor< is_trivially_destructible, is_trivially_constructible >
{

    static_assert(is_trivially_destructible);
    static_assert(is_trivially_constructible);

};

template< typename first, typename ...rest >
struct destructor< true, true, first, rest... >
{

    using storage = constructor< true, first, rest... >;

    std::size_t which_;
    storage storage_;

    destructor() = default;

    template< typename index,
              typename ...arguments >
    constexpr
    destructor(index,
               arguments &&... _arguments) noexcept(__is_nothrow_constructible(storage, index, arguments...))
        : which_{index::value}
        , storage_(index{}, std::forward< arguments >(_arguments)...)
    { ; }

    template< typename type >
    explicit
    constexpr
    operator type const & () const noexcept
    {
        return static_cast< type const & >(storage_);
    }

    template< typename type >
    explicit
    constexpr
    operator type & () noexcept
    {
        return static_cast< type & >(storage_);
    }

};

template< typename first, typename ...rest >
struct destructor< false, true, first, rest... >
{

    using storage = constructor< false, first, rest... >;

    std::size_t which_;
    storage storage_;

    ~destructor() noexcept(noexcept(storage_.destructor(which_)))
    {
        storage_.destructor(which_);
    }

    destructor() = default;

    template< typename index,
              typename ...arguments >
    constexpr
    destructor(index,
               arguments &&... _arguments) noexcept(__is_nothrow_constructible(storage, index, arguments...))
        : which_{index::value}
        , storage_(index{},
                   std::forward< arguments >(_arguments)...)
    { ; }

    template< typename type >
    explicit
    constexpr
    operator type const & () const noexcept
    {
        return static_cast< type const & >(storage_);
    }

    template< typename type >
    explicit
    constexpr
    operator type & () noexcept
    {
        return static_cast< type & >(storage_);
    }

};

template< typename first, typename ...rest >
struct destructor< true, false, first, rest... >
{

    using storage = constructor< true, first, rest... >;

    std::size_t which_;
    storage storage_;

    using default_index = index_of_default_constructible< first, rest..., void >;

    constexpr
    destructor() noexcept(__is_nothrow_constructible(destructor, typename default_index::type))
        : destructor(typename default_index::type{})
    { ; }

    template< typename index,
              typename ...arguments >
    constexpr
    destructor(index,
               arguments &&... _arguments) noexcept(__is_nothrow_constructible(storage, index, arguments...))
        : which_{index::value}
        , storage_(index{},
                   std::forward< arguments >(_arguments)...)
    { ; }

    template< typename type >
    explicit
    constexpr
    operator type const & () const noexcept
    {
        return static_cast< type const & >(storage_);
    }

    template< typename type >
    explicit
    constexpr
    operator type & () noexcept
    {
        return static_cast< type & >(storage_);
    }

};

template< typename first, typename ...rest >
struct destructor< false, false, first, rest... >
{

    using storage = constructor< false, first, rest... >;

    std::size_t which_;
    storage storage_;

    ~destructor() noexcept(noexcept(storage_.destructor(which_)))
    {
        storage_.destructor(which_);
    }

    using default_index = index_of_default_constructible< first, rest..., void >;

    constexpr
    destructor() noexcept(__is_nothrow_constructible(destructor, typename default_index::type))
        : destructor(typename default_index::type{})
    { ; }

    template< typename index,
              typename ...arguments >
    constexpr
    destructor(index,
               arguments &&... _arguments) noexcept(__is_nothrow_constructible(storage, index, arguments...))
        : which_{index::value}
        , storage_(index{},
                   std::forward< arguments >(_arguments)...)
    { ; }

    template< typename type >
    explicit
    constexpr
    operator type const & () const noexcept
    {
        return static_cast< type const & >(storage_);
    }

    template< typename type >
    explicit
    constexpr
    operator type & () noexcept
    {
        return static_cast< type & >(storage_);
    }

};

template< typename ...types >
class versatile
{

    using storage = destructor< (__has_trivial_destructor(types) && ...), (__has_trivial_constructor(types) && ...), types... >;

    storage storage_;

public :

    using default_index = index_of_default_constructible< types..., void >;

    constexpr
    std::size_t
    which() const noexcept
    {
        if (storage_.which_ == std::size_t{}) { // is trivially default constructed?
            return sizeof...(types); // if so, then it always point to the first (leftmost) alternative type
        }
        return storage_.which_;
    }

    template< typename type >
    using index_t = index_at_t< unwrap_type_t< type >, unwrap_type_t< types >..., void >;

    template< typename type,
              typename index = index_t< type > >
    constexpr
    bool
    active() const noexcept
    {
        return (which() == index::value);
    }

    versatile() = default;

    template< typename type,
              typename index = index_t< type > >
    constexpr
    versatile(type && _value) noexcept(__is_nothrow_constructible(storage, index, type))
        : storage_(index{},
                   std::forward< type >(_value))
    { ; }

    template< typename ...arguments,
              typename index = get_index_t< __is_constructible(types, arguments...)..., false > > // prohibits using of versatile<>
    constexpr
    versatile(arguments &&... _arguments) noexcept(__is_nothrow_constructible(storage, index, arguments...))
        : storage_(index{},
                   std::forward< arguments >(_arguments)...)
    { ; }

    template< typename type,
              typename index = index_t< type > >
    explicit
    constexpr
    operator type const & () const noexcept
    {
        return static_cast< type const & >(storage_);
    }

    template< typename type,
              typename index = index_t< type > >
    explicit
    constexpr
    operator type & () noexcept
    {
        return static_cast< type & >(storage_);
    }

    template< typename type,
              typename index = index_t< type >,
              typename = get_index_t< (__has_trivial_copy(unwrap_type_t< type >) && ... && __has_trivial_assign(types)) > > // enable_if_t >
    constexpr
    versatile &
    operator = (type && _value) noexcept // trivial per se, but due to operator is user provided - is not
    {
        return (*this = versatile(std::forward< type >(_value))); // http://stackoverflow.com/questions/33936295/
    }


};

template< typename first, typename ...rest >
struct is_visitable< versatile< first, rest... > >
        : std::true_type
{

};

}

#pragma once

#include "visit.hpp"
#include "in_place.hpp"

#include <type_traits>
#include <utility>
#include <experimental/optional>

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

    constexpr
    destructor_dispatcher() = default;

    template< typename ...arguments >
    constexpr
    destructor_dispatcher(index_t< (sizeof...(rest) + 1) >, arguments &&... _arguments) noexcept(noexcept(::new (std::declval< void * >()) head(std::declval< arguments >()...)))
        : head_(std::forward< arguments >(_arguments)...)
    { ; }

    template< typename ...arguments >
    constexpr
    destructor_dispatcher(arguments &&... _arguments) noexcept(noexcept(::new (std::declval< void * >()) tail(std::declval< arguments >()...)))
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

    constexpr
    destructor_dispatcher(destructor_dispatcher const &) = default;
    constexpr
    destructor_dispatcher(destructor_dispatcher &) = default;
    constexpr
    destructor_dispatcher(destructor_dispatcher &&) = default;

    constexpr
    destructor_dispatcher
    & operator = (destructor_dispatcher const &) = default;
    constexpr
    destructor_dispatcher
    & operator = (destructor_dispatcher &) = default;
    constexpr
    destructor_dispatcher
    & operator = (destructor_dispatcher &&) = default;

    ~destructor_dispatcher() noexcept
    {
        //tail_.~tail();
    }

    void
    destructor(std::size_t const _which) const noexcept(noexcept(head_.~head()) && noexcept(tail_.destructor(_which)))
    {
        if (_which == (sizeof...(rest) + 1)) {
            head_.~head();
        } else {
            tail_.destructor(_which);
        }
    }

    constexpr
    destructor_dispatcher() = default;

    template< typename ...arguments >
    constexpr
    destructor_dispatcher(index_t< (sizeof...(rest) + 1) >, arguments &&... _arguments) noexcept(noexcept(::new (std::declval< void * >()) head(std::declval< arguments >()...)))
        : head_(std::forward< arguments >(_arguments)...)
    { ; }

    template< typename ...arguments >
    constexpr
    destructor_dispatcher(arguments &&... _arguments) noexcept(noexcept(::new (std::declval< void * >()) tail(std::declval< arguments >()...)))
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

template< bool is_trivially_destructible, bool is_trivially_constructible, typename ...types >
class dispatcher;

template< typename ...types >
class dispatcher< true, true, types... >
{

    using storage = destructor_dispatcher< true, types... >;

    std::size_t which_;
    storage storage_;

public :

    constexpr
    std::size_t
    which() const noexcept
    {
        if (which_ == std::size_t{}) {
            return sizeof...(types);
        }
        return which_;
    }

    using default_index = index_t< sizeof...(types) >;

    constexpr
    dispatcher() = default;

    template< typename index, typename ...arguments >
    constexpr
    dispatcher(index, arguments &&... _arguments) noexcept(noexcept(::new (std::declval< void * >()) storage(index{}, std::forward< arguments >(_arguments)...)))
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
class dispatcher< false, true, types... >
{

    using storage = destructor_dispatcher< false, types... >;

    std::size_t which_;
    storage storage_;

public :

    constexpr
    std::size_t
    which() const noexcept
    {
        if (which_ == std::size_t{}) {
            return sizeof...(types);
        }
        return which_;
    }

    constexpr
    dispatcher(dispatcher const &) = default;
    constexpr
    dispatcher(dispatcher &) = default;
    constexpr
    dispatcher(dispatcher &&) = default;

    constexpr
    dispatcher
    & operator = (dispatcher const &) = default;
    constexpr
    dispatcher
    & operator = (dispatcher &) = default;
    constexpr
    dispatcher
    & operator = (dispatcher &&) = default;

    ~dispatcher() noexcept(noexcept(storage_.destructor(which_)))
    {
        storage_.destructor(which_);
    }

    using default_index = index_t< sizeof...(types) >;

    constexpr
    dispatcher() = default;

    template< typename index, typename ...arguments >
    constexpr
    dispatcher(index, arguments &&... _arguments) noexcept(noexcept(::new (std::declval< void * >()) storage(index{}, std::forward< arguments >(_arguments)...)))
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
class dispatcher< true, false, types... >
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

    using default_index = index_of_default_constructible< types..., void >;

    constexpr
    dispatcher() noexcept(noexcept(::new (std::declval< void * >()) storage(typename default_index::type{})))
        : dispatcher(typename default_index::type{})
    { ; }

    template< typename index, typename ...arguments >
    constexpr
    dispatcher(index, arguments &&... _arguments) noexcept(noexcept(::new (std::declval< void * >()) storage(index{}, std::forward< arguments >(_arguments)...)))
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
class dispatcher< false, false, types... >
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

    constexpr
    dispatcher(dispatcher const &) = default;
    constexpr
    dispatcher(dispatcher &) = default;
    constexpr
    dispatcher(dispatcher &&) = default;

    constexpr
    dispatcher
    & operator = (dispatcher const &) = default;
    constexpr
    dispatcher
    & operator = (dispatcher &) = default;
    constexpr
    dispatcher
    & operator = (dispatcher &&) = default;

    ~dispatcher() noexcept(noexcept(storage_.destructor(which_)))
    {
        storage_.destructor(which_);
    }

    using default_index = index_of_default_constructible< types..., void >;

    constexpr
    dispatcher() noexcept(noexcept(::new (std::declval< void * >()) storage(typename default_index::type{})))
        : dispatcher(typename default_index::type{})
    { ; }

    template< typename index, typename ...arguments >
    constexpr
    dispatcher(index, arguments &&... _arguments) noexcept(noexcept(::new (std::declval< void * >()) storage(index{}, std::forward< arguments >(_arguments)...)))
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
using dispatcher_t = dispatcher< (std::is_trivially_destructible_v< types > && ...), (std::is_trivially_constructible_v< types > && ...), types... >;

template< bool is_default_constructible >
struct enable_default_constructor;

template<>
struct enable_default_constructor< true >
{

    constexpr
    enable_default_constructor() = default;

    constexpr
    enable_default_constructor(void *) noexcept
    { ; }

};

template<>
struct enable_default_constructor< false >
{

    constexpr
    enable_default_constructor() = delete;

    constexpr
    enable_default_constructor(void *) noexcept
    { ; }

};

template< typename ...types >
using enable_default_constructor_t = enable_default_constructor< (std::is_default_constructible_v< types > || ...) >;

template< typename ...types >
class versatile
        : enable_default_constructor_t< types... > // akrzemi1's technique
{

    using enabler = enable_default_constructor_t< types... >;
    using storage = dispatcher_t< types... >;

    storage storage_;

public :

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
    using index_at_t = index_at_t< unwrap_type_t< type >, unwrap_type_t< types >..., void >;

    template< typename type >
    constexpr
    bool
    active() const noexcept
    {
        return (which() == index_at_t< type >::value);
    }

    constexpr
    versatile() = default;

    template< typename type, typename index = index_at_t< type > >
    constexpr
    versatile(type && _value) noexcept(noexcept(::new (std::declval< void * >()) storage(index{}, std::forward< type >(_value))))
        : enabler({})
        , storage_(index{}, std::forward< type >(_value))
    { ; }

    template< std::size_t i, typename ...arguments >
    explicit
    constexpr
    versatile(in_place_t (&)(index_t< i >), arguments &&... _arguments) noexcept(noexcept(::new (std::declval< void * >()) storage(index_t< i >{}, std::forward< arguments >(_arguments)...)))
        : enabler({})
        , storage_(index_t< i >{}, std::forward< arguments >(_arguments)...)
    { ; }

    template< typename type, typename index = index_at_t< type >, typename ...arguments >
    explicit
    constexpr
    versatile(in_place_t (&)(identity< type >), arguments &&... _arguments) noexcept(noexcept(::new (std::declval< void * >()) storage(index{}, std::forward< arguments >(_arguments)...)))
        : enabler({})
        , storage_(index{}, std::forward< arguments >(_arguments)...)
    { ; }

    template< typename type, typename index = index_at_t< type > >
    explicit
    constexpr
    operator type const & () const noexcept
    {
        return storage_;
    }

    template< typename type, typename index = index_at_t< type > >
    explicit
    constexpr
    operator type & () noexcept
    {
        return storage_;
    }

    template< typename type, typename index = index_at_t< type > >
    constexpr
    versatile &
    operator = (type && _value) noexcept
    {
        static_assert(std::is_trivially_assignable_v< versatile, versatile >, "all alternative types should be trivially move assignable");
        return (*this = versatile(std::forward< type >(_value))); // http://stackoverflow.com/questions/33936295/
    }

    template< std::size_t i = sizeof...(types), typename ...arguments >
    constexpr
    void
    emplace(arguments &&... _arguments) noexcept
    {
        static_assert(std::is_trivially_assignable_v< versatile, versatile >, "all alternative types should be trivially move assignable");
        *this = versatile(in_place< i >, std::forward< arguments >(_arguments)...);
    }

    template< typename type, typename ...arguments >
    constexpr
    void
    emplace(arguments &&... _arguments) noexcept
    {
        static_assert(std::is_trivially_assignable_v< versatile, versatile >, "all alternative types should be trivially move assignable");
        *this = versatile(in_place< type >, std::forward< arguments >(_arguments)...);
    }

};

template< typename first, typename ...rest >
struct is_visitable< versatile< first, rest... > >
        : std::true_type
{

};

}

#pragma once

#include "versatile.hpp"

#include <type_traits>
#include <utility>
#include <experimental/optional>
#include <memory>
#include <typeinfo>

#include <cstddef>
#include <cassert>

namespace versatile
{

template< typename ...types >
class variant
{

    using versatile = versatile< types... >;
    using storage_type = std::unique_ptr< versatile >; // `All problems in computer science can be solved by another level of indirection, except for the problem of too many layers of indirection.`

    storage_type storage_;

public :

    using size_type = typename versatile::size_type;

    static constexpr size_type width = sizeof...(types);

    size_type
    which() const noexcept
    {
        assert(!!storage_);
        return storage_->which();
    }

    template< typename type >
    static
    constexpr
    size_type
    index() noexcept
    {
        return versatile::template index< type >();
    }

    template< typename type >
    bool
    active() const noexcept
    {
        return storage_->template active< type >();
    }

private :

    struct constructor
    {

        template< typename type >
        storage_type
        operator () (type && _value) const
        {
            return std::make_unique< versatile >(std::forward< type >(_value));
        }

    };

public :

    variant(variant & _rhs)
        : storage_(visit(constructor{}, *_rhs.storage_))
    { ; }

    variant(variant const & _rhs)
        : storage_(visit(constructor{}, *_rhs.storage_))
    { ; }

    variant(variant && _rhs)
        : storage_(visit(constructor{}, std::move(*_rhs.storage_)))
    { ; }

    variant(variant const && _rhs)
        : storage_(visit(constructor{}, std::move(*_rhs.storage_)))
    { ; }

    template< typename ...arguments >
    variant(arguments &&... _arguments)
        : storage_(std::make_unique< versatile >(std::forward< arguments >(_arguments)...))
    { ; }

    void
    swap(variant & _other) noexcept
    {
        storage_.swap(_other.storage_);
    }

    template< typename ...arguments >
    void
    emplace(arguments &&... _arguments)
    {
        variant{std::experimental::in_place, std::forward< arguments >(_arguments)...}.swap(*this);
    }

    template< typename type >
    void
    replace(type && _value)
    {
        variant{std::forward< type >(_value)}.swap(*this);
    }

private :

    struct assigner
    {

        versatile & destination_;

        template< typename type >
        void
        operator () (type && _value) const
        {
            static_cast< std::decay_t< type > & >(destination_) = std::forward< type >(_value);
        }

    };

public :

    template< typename type >
    std::enable_if_t< (std::is_same< std::decay_t< type >, variant >{}) >
    assign(type && _rhs)
    {
        assert(&_rhs != this);
        if (which() == _rhs.which()) {
           visit(assigner{*storage_},  std::forward< type >(_rhs));
        } else {
            replace(std::forward< type >(_rhs));
        }
    }

    template< typename rhs >
    std::enable_if_t< !(std::is_same< std::decay_t< rhs >, variant >{}) >
    assign(rhs && _rhs)
    {
        using type = std::decay_t< rhs >;
        if (active< type >()) {
            static_cast< type & >(*storage_) = std::forward< rhs >(_rhs);
        } else {
            replace(std::forward< rhs >(_rhs));
        }
    }

    variant &
    operator = (variant & _rhs) &
    {
        assign(_rhs);
        return *this;
    }

    variant &
    operator = (variant const & _rhs) &
    {
        assign(_rhs);
        return *this;
    }

    variant &
    operator = (variant && _rhs) &
    {
        assign(std::move(_rhs));
        return *this;
    }

    variant &
    operator = (variant const && _rhs) &
    {
        assign(std::move(_rhs));
        return *this;
    }

    template< typename type >
    variant &
    operator = (type && _value) &
    {
        assign(std::forward< type >(_value));
        return *this;
    }

    template< typename type >
    explicit
    operator type & () &
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return static_cast< type & >(*storage_);
    }

    template< typename type >
    explicit
    operator type const & () const &
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return static_cast< type const & >(*storage_);
    }

    template< typename type >
    explicit
    operator type && () &&
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        //return static_cast< type && >(std::move(*storage_)); // There is known clang++ bug #19917 for static_cast to rvalue reference.
        return static_cast< type && >(static_cast< type & >(*storage_)); // workaround
    }

    template< typename type >
    explicit
    operator type const && () const &&
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        //return static_cast< type const && >(std::move(*storage_)); // There is known clang++ bug #19917 for static_cast to rvalue reference.
        return static_cast< type const && >(static_cast< type const & >(*storage_)); // workaround
    }

    template< typename ...arguments >
    decltype(auto)
    operator () (arguments &&... _arguments) &
    {
        return visit([&] (auto & _value) -> decltype(auto)
        {
            return _value(std::forward< arguments >(_arguments)...);
        }, *this);
    }

    template< typename ...arguments >
    decltype(auto)
    operator () (arguments &&... _arguments) const &
    {
        return visit([&] (auto const & _value) -> decltype(auto)
        {
            return _value(std::forward< arguments >(_arguments)...);
        }, *this);
    }

    template< typename ...arguments >
    decltype(auto)
    operator () (arguments &&... _arguments) &&
    {
        return visit([&] (auto && _value) -> decltype(auto)
        {
            return std::move(_value)(std::forward< arguments >(_arguments)...);
        }, std::move(*this));
    }

    template< typename ...arguments >
    decltype(auto)
    operator () (arguments &&... _arguments) const &&
    {
        return visit([&] (auto const && _value) -> decltype(auto)
        {
            return std::move(_value)(std::forward< arguments >(_arguments)...);
        }, std::move(*this));
    }

};

template<>
class variant<>
{

};

template< typename first, typename ...rest >
constexpr
void
swap(variant< first, rest... > & _lhs, variant< first, rest... > & _rhs) noexcept
{
    _lhs.swap(_rhs);
}

template< typename first, typename ...rest >
struct is_visitable< variant< first, rest... > >
        : std::true_type
{

};

template< typename visitable, typename first, typename ...rest >
struct first_type< visitable, variant< first, rest... > >
        : identity< copy_cv_reference_t< visitable, unwrap_type_t< first > > >
{

};

}

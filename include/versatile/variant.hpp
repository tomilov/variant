#pragma once

#include <versatile/versatile.hpp>
#include <versatile/recursive_wrapper.hpp>

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
class variant;

template<>
class variant<>
{

};

template< typename ...types >
class variant
        : enable_default_constructor_t< types... >
{

    using storage = versatile< types... >;

    recursive_wrapper< storage > storage_; // `All problems in computer science can be solved by another level of indirection, except for the problem of too many layers of indirection.`

public :

    template< typename type >
    using index_at = typename storage::template index_at< type >;

    template< typename ...arguments >
    using index_of_constructible = typename storage::template index_of_constructible< arguments... >;

    std::size_t
    which() const noexcept
    {
        return static_cast< storage const & >(storage_).which();
    }

    template< typename type >
    bool
    active() const noexcept
    {
        return static_cast< storage const & >(storage_).template active< type >();
    }

private :

    explicit
    variant(recursive_wrapper< storage > && _storage)
        : enable_default_constructor_t< types... >({})
        , storage_(std::move(_storage))
    { ; }

    struct constructor
    {

        template< typename type >
        recursive_wrapper< storage >
        operator () (type && _value) const
        {
            return std::forward< type >(_value);
        }

    };

public :

    variant() = default;

    variant(variant & _rhs)
        : variant(visit(constructor{}, _rhs.storage_))
    { ; }

    variant(variant const & _rhs)
        : variant(visit(constructor{}, _rhs.storage_))
    { ; }

    variant(variant && _rhs)
        : variant(visit(constructor{}, std::move(_rhs.storage_)))
    { ; }

    variant(variant const && _rhs)
        : variant(visit(constructor{}, std::move(_rhs.storage_)))
    { ; }

    template< std::size_t i, typename ...arguments >
    explicit
    variant(in_place_t (&)(index_t< i >), arguments &&... _arguments)
        : variant({in_place< i >, std::forward< arguments >(_arguments)...})
    { ; }

    template< typename type, typename index = typename index_at< type >::type, typename ...arguments >
    explicit
    variant(in_place_t (&)(type), arguments &&... _arguments)
        : variant(in_place< index >, std::forward< arguments >(_arguments)...)
    { ; }

    template< typename type, typename index = typename index_at< type >::type >
    variant(type && _value)
        : variant(in_place< index >, std::forward< type >(_value))
    { ; }

    template< typename ...arguments, typename index = typename index_of_constructible< arguments... >::type >
    explicit
    variant(in_place_t, arguments &&... _arguments)
        : variant(in_place< index >, std::forward< arguments >(_arguments)...)
    { ; }

    void
    swap(variant & _other) noexcept
    {
        storage_.swap(_other.storage_);
    }

private :

    struct assigner
    {

        storage & destination_;

        template< typename type >
        void
        operator () (type && _value) const
        {
            static_cast< unwrap_type_t< type > & >(destination_) = std::forward< type >(_value);
        }

    };

public :

    template< typename type >
    variant &
    assign(type && _rhs)
    {
        assert(&_rhs != this);
        if (which() == _rhs.which()) {
            visit(assigner{storage_},  std::forward< type >(_rhs));
        } else {
            variant(std::forward< type >(_rhs)).swap(*this);
        }
        return *this;
    }

    variant &
    operator = (variant & _rhs)
    {
        return assign(_rhs);
    }

    variant &
    operator = (variant const & _rhs)
    {
        return assign(_rhs);
    }

    variant &
    operator = (variant && _rhs)
    {
        return assign(std::move(_rhs));
    }

    variant &
    operator = (variant const && _rhs)
    {
        return assign(std::move(_rhs));
    }

    template< typename type, typename index = typename index_at< type >::type >
    variant &
    operator = (type && _value)
    {
        if (active< type >()) {
            static_cast< unwrap_type_t< type > & >(storage_) = std::forward< type >(_value);
        } else {
            variant(std::forward< type >(_value)).swap(*this);
        }
        return *this;
    }

    template< typename type, typename index = typename index_at< type >::type >
    explicit
    operator type & ()
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return static_cast< type & >(storage_);
    }

    template< typename type, typename index = typename index_at< type >::type >
    explicit
    operator type const & () const
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return static_cast< type const & >(storage_);
    }

};

template< typename first, typename ...rest >
struct is_visitable< variant< first, rest... > >
        : std::true_type
{

};

template< typename first, typename ...rest >
void
swap(variant< first, rest... > & _lhs, variant< first, rest... > & _rhs) noexcept
{
    _lhs.swap(_rhs);
}

}

#pragma once

#include "type_traits.hpp"
#include "versatile.hpp"

#include <type_traits>
#include <experimental/optional>
#include <utility>
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
        return index_by_type< type, unwrap_type_t< types >... >();
    }

    template< typename type >
    bool
    active() const noexcept
    {
        return (index< type >() == which());
    }

private :

    template< typename type, typename result_type, typename visitor, typename visitable, typename ...arguments >
    static
    constexpr
    result_type
    caller(visitor & _visitor, visitable & _visitable, arguments &... _arguments)
    {
        //return std::forward< visitor >(_visitor)(static_cast< unwrap_type_t< type > >(std::forward< visitable >(_visitable)), std::forward< arguments >(_arguments)...); // There is known clang++ bug #19917 for static_cast to rvalue reference.
        using T = unwrap_type_t< type >;
        return std::forward< visitor >(_visitor)(static_cast< T >(static_cast< T & >(_visitable)), std::forward< arguments >(_arguments)...); // workaround
    }

public :

    using first_type = typename versatile::this_type;

    template< typename visitor, typename ...arguments >
    decltype(auto)
    visit(visitor && _visitor, arguments &&... _arguments) &
    {
        using result_type = result_of_t< visitor, first_type &, arguments... >;
        using caller_type = result_type (*)(visitor & _visitor, versatile & _visitable, arguments &... _arguments);
        static constexpr caller_type dispatcher_[width] = {variant::caller< types &, result_type, visitor, versatile, arguments... >...};
        return dispatcher_[versatile::index - which()](_visitor, *storage_, _arguments...);
    }

    template< typename visitor, typename ...arguments >
    decltype(auto)
    visit(visitor && _visitor, arguments &&... _arguments) const &
    {
        using result_type = result_of_t< visitor, first_type const &, arguments... >;
        using caller_type = result_type (*)(visitor & _visitor, versatile const & _visitable, arguments &... _arguments);
        static constexpr caller_type dispatcher_[width] = {variant::caller< types const &, result_type, visitor, versatile const, arguments... >...};
        return dispatcher_[versatile::index - which()](_visitor, *storage_, _arguments...);
    }

    template< typename visitor, typename ...arguments >
    decltype(auto)
    visit(visitor && _visitor, arguments &&... _arguments) &&
    {
        using result_type = result_of_t< visitor, first_type &&, arguments... >;
        using caller_type = result_type (*)(visitor & _visitor, versatile & _visitable, arguments &... _arguments);
        static constexpr caller_type dispatcher_[width] = {variant::caller< types &&, result_type, visitor, versatile, arguments... >...};
        return dispatcher_[versatile::index - which()](_visitor, *storage_, _arguments...);
    }

    template< typename visitor, typename ...arguments >
    decltype(auto)
    visit(visitor && _visitor, arguments &&... _arguments) const &&
    {
        using result_type = result_of_t< visitor, first_type const &&, arguments... >;
        using caller_type = result_type (*)(visitor & _visitor, versatile const & _visitable, arguments &... _arguments);
        static constexpr caller_type dispatcher_[width] = {variant::caller< types const &&, result_type, visitor, versatile const, arguments... >...};
        return dispatcher_[versatile::index - which()](_visitor, *storage_, _arguments...);
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
        : storage_(_rhs.visit(constructor{}))
    { ; }

    variant(variant const & _rhs)
        : storage_(_rhs.visit(constructor{}))
    { ; }

    variant(variant && _rhs)
        : storage_(std::move(_rhs).visit(constructor{}))
    { ; }

    variant(variant const && _rhs)
        : storage_(std::move(_rhs).visit(constructor{}))
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
            destination_ = std::forward< type >(_value);
        }

    };

public :

    template< typename type >
    std::enable_if_t< (std::is_same< std::decay_t< type >, variant >{}) >
    assign(type && _rhs)
    {
        assert(&_rhs != this);
        if (which() == _rhs.which()) {
            std::forward< type >(_rhs).visit(assigner{*storage_});
        } else {
            replace(std::forward< type >(_rhs));
        }
    }

    template< typename type >
    std::enable_if_t< !(std::is_same< std::decay_t< type >, variant >{}) >
    assign(type && _rhs)
    {
        if (active< std::decay_t< type > >()) {
            *storage_ = std::forward< type >(_rhs);
        } else {
            replace(std::forward< type >(_rhs));
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
        return visit([&] (auto & _value) -> decltype(auto) { return _value(std::forward< arguments >(_arguments)...); });
    }

    template< typename ...arguments >
    decltype(auto)
    operator () (arguments &&... _arguments) const &
    {
        return visit([&] (auto const & _value) -> decltype(auto) { return _value(std::forward< arguments >(_arguments)...); });
    }

    template< typename ...arguments >
    decltype(auto)
    operator () (arguments &&... _arguments) &&
    {
        return visit([&] (auto && _value) -> decltype(auto) { return std::move(_value)(std::forward< arguments >(_arguments)...); });
    }

    template< typename ...arguments >
    decltype(auto)
    operator () (arguments &&... _arguments) const &&
    {
        return visit([&] (auto const && _value) -> decltype(auto) { return std::move(_value)(std::forward< arguments >(_arguments)...); });
    }

};

template< typename ...types >
constexpr
void
swap(variant< types... > & _lhs, variant< types... > & _rhs) noexcept
{
    _lhs.swap(_rhs);
}

template< typename type >
struct is_variant
        : std::false_type
{

};

template< typename ...types >
struct is_variant< variant< types... > >
        : std::true_type
{

};

template< typename type >
struct is_variant< type const >
        : is_variant< type >
{

};

template< typename type >
struct is_variant< volatile type >
        : is_variant< type >
{

};

template< typename type >
struct is_variant< volatile type const >
        : is_variant< type >
{

};

template< typename type, typename = std::decay_t< type > >
struct first_type
        : identity< type >
{

};

template< typename visitable, typename first, typename ...rest >
struct first_type< visitable, variant< first, rest... > >
        : identity< copy_cv_reference_t< visitable, first > >
{

};

template< typename type >
using first_type_t = typename first_type< type >::type;

}

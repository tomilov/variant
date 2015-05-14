#pragma once

#include "type_traits.hpp"
#include "recursive_wrapper.hpp"
#include "versatile.hpp"

#include <type_traits>
#include <utility>
#include <memory>

#include <cstddef>
#include <cassert>

namespace variant
{

template< typename ...types >
struct variant
{

    static constexpr std::size_t size = sizeof...(types);

private :

    using versatile = versatile< types... >;
    using storage_type = std::unique_ptr< versatile >;

    storage_type storage_;

public :

    template< std::size_t _which = size >
    using at = typename versatile::template at< _which >;

    template< typename type = at<> >
    static
    constexpr
    std::size_t
    index() noexcept
    {
        return versatile::template index< type >();
    }

    constexpr
    std::size_t
    which() const noexcept
    {
        if (!!storage_) {
            return storage_->which();
        } else {
            return 0;
        }
    }

    template< typename type = at<> >
    constexpr
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
    caller(visitor && _visitor, visitable && _visitable, arguments &&... _arguments) noexcept(is_nothrow_callable_v< visitor, unwrap_type_t< type >, arguments... >)
    {
        // There is known clang++ bug #19917 for static_cast to rvalue reference.
#if 0
        return std::forward< visitor >(_visitor)(static_cast< unwrap_type_t< type > >(std::forward< visitable >(_visitable)), std::forward< arguments >(_arguments)...);
#else
        // workaround
        using T = unwrap_type_t< type >;
        return std::forward< visitor >(_visitor)(static_cast< T >(static_cast< T & >(_visitable)), std::forward< arguments >(_arguments)...);
#endif
    }

public :

    template< typename visitor, typename ...arguments >
    decltype(auto)
    apply_visitor(visitor && _visitor, arguments &&... _arguments) const &
    {
        using result_type = result_of< visitor, at<> const &, arguments... >;
        using caller_type = result_type (*)(visitor && _visitor, versatile const & _visitable, arguments &&... _arguments);
        static constexpr caller_type dispatcher_[size] = {variant::caller< types const &, result_type, visitor, versatile const &, arguments... >...};
        return dispatcher_[size - which()](std::forward< visitor >(_visitor), *storage_, std::forward< arguments >(_arguments)...);
    }

    template< typename visitor, typename ...arguments >
    decltype(auto)
    apply_visitor(visitor && _visitor, arguments &&... _arguments) &
    {
        using result_type = result_of< visitor, at<> &, arguments... >;
        using caller_type = result_type (*)(visitor && _visitor, versatile & _visitable, arguments &&... _arguments);
        static constexpr caller_type dispatcher_[size] = {variant::caller< types &, result_type, visitor, versatile &, arguments... >...};
        return dispatcher_[size - which()](std::forward< visitor >(_visitor), *storage_, std::forward< arguments >(_arguments)...);
    }

    template< typename visitor, typename ...arguments >
    decltype(auto)
    apply_visitor(visitor && _visitor, arguments &&... _arguments) const &&
    {
        using result_type = result_of< visitor, at<> const &&, arguments... >;
        using caller_type = result_type (*)(visitor && _visitor, versatile const && _visitable, arguments &&... _arguments);
        static constexpr caller_type dispatcher_[size] = {variant::caller< types const &&, result_type, visitor, versatile const &&, arguments... >...};
        return dispatcher_[size - which()](std::forward< visitor >(_visitor), std::move(*storage_), std::forward< arguments >(_arguments)...);
    }

    template< typename visitor, typename ...arguments >
    decltype(auto)
    apply_visitor(visitor && _visitor, arguments &&... _arguments) &&
    {
        using result_type = result_of< visitor, at<> &&, arguments... >;
        using caller_type = result_type (*)(visitor && _visitor, versatile && _visitable, arguments &&... _arguments);
        static constexpr caller_type dispatcher_[size] = {variant::caller< types &&, result_type, visitor, versatile &&, arguments... >...};
        return dispatcher_[size - which()](std::forward< visitor >(_visitor), std::move(*storage_), std::forward< arguments >(_arguments)...);
    }

private :

    struct constructor
    {

        storage_type & storage_;

        template< typename type >
        void
        operator () (type && _value) const
        {
            storage_ = std::make_unique< versatile >(std::forward< type >(_value));
        }

    };

public :

    constexpr
    variant(variant const & _rhs)
    {
        _rhs.apply_visitor(constructor{storage_});
    }

    constexpr
    variant(variant & _rhs)
    {
        _rhs.apply_visitor(constructor{storage_});
    }

    constexpr
    variant(variant const && _rhs)
    {
        std::move(_rhs).apply_visitor(constructor{storage_});
    }

    constexpr
    variant(variant && _rhs)
    {
        std::move(_rhs).apply_visitor(constructor{storage_});
    }

    template< typename ...arguments >
    constexpr
    variant(arguments &&... _arguments)
        : storage_(std::make_unique< versatile >(std::forward< arguments >(_arguments)...))
    { ; }

    void
    swap(variant & _other) noexcept
    {
        storage_.swap(_other.storage_);
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

    constexpr
    variant &
    operator = (variant const & _rhs) &
    {
        if (which() == _rhs.which()) {
            _rhs.apply_visitor(assigner{*storage_});
        } else {
            variant(_rhs).swap(*this);
        }
        return *this;
    }

    constexpr
    variant &
    operator = (variant & _rhs) &
    {
        if (which() == _rhs.which()) {
            _rhs.apply_visitor(assigner{*storage_});
        } else {
            variant(_rhs).swap(*this);
        }
        return *this;
    }

    constexpr
    variant &
    operator = (variant const && _rhs) &
    {
        if (which() == _rhs.which()) {
            std::move(_rhs).apply_visitor(assigner{*storage_});
        } else {
            variant(std::move(_rhs)).swap(*this);
        }
        return *this;
    }

    constexpr
    variant &
    operator = (variant && _rhs) &
    {
        if (which() == _rhs.which()) {
            std::move(_rhs).apply_visitor(assigner{*storage_});
        } else {
            variant(std::move(_rhs)).swap(*this);
        }
        return *this;
    }

    template< typename rhs >
    constexpr
    variant &
    operator = (rhs && _rhs) &
    {
        if (active< std::decay_t< rhs > >()) {
            *storage_ = std::forward< rhs >(_rhs);
        } else {
            variant(std::forward< rhs >(_rhs)).swap(*this);
        }
        return *this;
    }

    template< typename type >
    explicit
    constexpr
    operator type const & () const & noexcept
    {
        assert(active< type >());
        return static_cast< type const & >(*storage_);
    }

    template< typename type >
    explicit
    constexpr
    operator type & () & noexcept
    {
        assert(active< type >());
        return static_cast< type & >(*storage_);
    }

    template< typename type >
    explicit
    constexpr
    operator type const && () const && noexcept
    {
        assert(active< type >());
        return static_cast< type const && >(*storage_);
    }

    template< typename type >
    explicit
    constexpr
    operator type && () && noexcept
    {
        assert(active< type >());
        return static_cast< type && >(*storage_);
    }

};

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

template< typename type, bool = is_variant< std::remove_cv_t< std::remove_reference_t< type > > >{} >
struct first_type;

template< typename visitable >
struct first_type< visitable, true >
{

    using type = copy_cv_reference_t< visitable, typename std::remove_reference_t< visitable >::template at<> >;

};

template< typename general_type >
struct first_type< general_type, false >
{

    using type = general_type;

};

template< typename type >
using first_type_t = typename first_type< type >::type;

template< typename variant, typename type >
constexpr std::size_t index = variant::template index< type >();

template< typename variant, std::size_t index >
using at = typename variant::template at< index >;

template< typename ...types >
constexpr
void
swap(variant< types... > & _lhs, variant< types... > & _rhs) noexcept
{
    _lhs.swap(_rhs);
}

}

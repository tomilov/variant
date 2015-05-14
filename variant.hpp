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

    using storage_type = versatile< types... >;

    std::unique_ptr< storage_type > storage_;
    std::size_t which_;

    template< typename ...arguments >
    using constructible_type = typename storage_type::template constructible_type< arguments... >;

public :

    template< std::size_t _which = size >
    using at = typename storage_type::template at< _which >;

    template< typename type = at<> >
    static
    constexpr
    std::size_t
    index() noexcept
    {
        return storage_type::template index< type >();
    }

    constexpr
    std::size_t
    which() const noexcept
    {
        return which_;
    }

    template< typename type = at<> >
    constexpr
    bool
    is_active() const noexcept
    {
        return (index< type >() == which_);
    }

private :

    template< typename type >
    static
    void
    destructor(storage_type & _destructible) noexcept(std::is_nothrow_destructible< type >{})
    {/*
        _destructible.destruct< type >();*/
    }

public :

    ~variant() noexcept
    {
        using destructor_type = void (*)(storage_type & _destructible);
        static constexpr destructor_type dispatcher_[size] = {variant::destructor< types >...};
        dispatcher_[size - which_](*storage_);
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
        using caller_type = result_type (*)(visitor && _visitor, storage_type const & _visitable, arguments &&... _arguments);
        static constexpr caller_type dispatcher_[size] = {variant::caller< types const &, result_type, visitor, storage_type const &, arguments... >...};
        return dispatcher_[size - which_](std::forward< visitor >(_visitor), *storage_, std::forward< arguments >(_arguments)...);
    }

    template< typename visitor, typename ...arguments >
    decltype(auto)
    apply_visitor(visitor && _visitor, arguments &&... _arguments) &
    {
        using result_type = result_of< visitor, at<> &, arguments... >;
        using caller_type = result_type (*)(visitor && _visitor, storage_type & _visitable, arguments &&... _arguments);
        static constexpr caller_type dispatcher_[size] = {variant::caller< types &, result_type, visitor, storage_type &, arguments... >...};
        return dispatcher_[size - which_](std::forward< visitor >(_visitor), *storage_, std::forward< arguments >(_arguments)...);
    }

    template< typename visitor, typename ...arguments >
    decltype(auto)
    apply_visitor(visitor && _visitor, arguments &&... _arguments) const &&
    {
        using result_type = result_of< visitor, at<> const &&, arguments... >;
        using caller_type = result_type (*)(visitor && _visitor, storage_type const && _visitable, arguments &&... _arguments);
        static constexpr caller_type dispatcher_[size] = {variant::caller< types const &&, result_type, visitor, storage_type const &&, arguments... >...};
        return dispatcher_[size - which_](std::forward< visitor >(_visitor), std::move(*storage_), std::forward< arguments >(_arguments)...);
    }

    template< typename visitor, typename ...arguments >
    decltype(auto)
    apply_visitor(visitor && _visitor, arguments &&... _arguments) &&
    {
        using result_type = result_of< visitor, at<> &&, arguments... >;
        using caller_type = result_type (*)(visitor && _visitor, storage_type && _visitable, arguments &&... _arguments);
        static constexpr caller_type dispatcher_[size] = {variant::caller< types &&, result_type, visitor, storage_type &&, arguments... >...};
        return dispatcher_[size - which_](std::forward< visitor >(_visitor), std::move(*storage_), std::forward< arguments >(_arguments)...);
    }

private :

    template< typename type >
    void
    construct(type && _value)
    {
        ::new (storage_.get()) storage_type(std::forward< type >(_value));
        which_ = storage_type::template index< std::decay_t< type > >();
    }

    template< typename ...arguments >
    void
    construct(arguments &&... _arguments)
    {
        ::new (storage_.get()) storage_type(std::forward< arguments >(_arguments)...);
        which_ = storage_type::template index_of_constructible< arguments... >();
    }

    struct constructor
    {

        variant & destination_;

        template< typename type >
        void
        operator () (type && _value) const
        {
            return destination_.construct(std::forward< type >(_value));
        }

    };

public :

    constexpr
    variant(variant const & _rhs) //noexcept((std::is_nothrow_copy_constructible< types >{} && ...))
    {
        _rhs.apply_visitor(constructor{*this});
    }

    constexpr
    variant(variant & _rhs) //noexcept((std::is_nothrow_constructible< types, types & >{} && ...))
    {
        _rhs.apply_visitor(constructor{*this});
    }

    constexpr
    variant(variant const && _rhs) //noexcept((std::is_nothrow_constructible< types, types const && >{} && ...))
    {
        std::move(_rhs).apply_visitor(constructor{*this});
    }

    constexpr
    variant(variant && _rhs) //noexcept((std::is_nothrow_move_constructible< types >{} && ...))
    {
        std::move(_rhs).apply_visitor(constructor{*this});
    }

    template< typename ...arguments >
    constexpr
    variant(arguments &&... _arguments) noexcept(std::is_nothrow_constructible< storage_type, arguments... >{})
    {
        construct(std::forward< arguments >(_arguments)...);
    }

    void
    swap(variant & _other) noexcept
    {
        storage_.swap(_other.storage_);
    }

private :

    struct assigner
    {

        storage_type & destination_;

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
    operator = (variant const & _rhs) & //noexcept((std::is_nothrow_copy_assignable< types >{} && ...))
    {
        if (which_ == _rhs.which_) {
            _rhs.apply_visitor(assigner{*storage_});
        } else {
            variant(_rhs).swap(*this);
        }
        return *this;
    }

    constexpr
    variant &
    operator = (variant & _rhs) & noexcept(std::is_nothrow_copy_constructible< storage_type >{} && std::is_nothrow_copy_assignable< storage_type >{} && std::is_nothrow_destructible< storage_type >{})
    {
        if (which_ == _rhs.which_) {
            _rhs.apply_visitor(assigner{*storage_});
        } else {
            variant(_rhs).swap(*this);
        }
        return *this;
    }

    constexpr
    variant &
    operator = (variant const && _rhs) & noexcept(std::is_nothrow_move_constructible< storage_type >{} && std::is_nothrow_move_assignable< storage_type >{} && std::is_nothrow_destructible< storage_type >{})
    {
        if (which_ == _rhs.which_) {
            std::move(_rhs).apply_visitor(assigner{*storage_});
        } else {
            variant(std::move(_rhs)).swap(*this);
        }
        return *this;
    }

    constexpr
    variant &
    operator = (variant && _rhs) & noexcept(std::is_nothrow_move_constructible< storage_type >{} && std::is_nothrow_move_assignable< storage_type >{} && std::is_nothrow_destructible< storage_type >{})
    {
        if (which_ == _rhs.which_) {
            std::move(_rhs).apply_visitor(assigner{*storage_});
        } else {
            variant(std::move(_rhs)).swap(*this);
        }
        return *this;
    }

    template< typename rhs >
    constexpr
    variant &
    operator = (rhs && _rhs) & noexcept(std::is_nothrow_assignable< storage_type &, rhs >{} && std::is_nothrow_constructible< storage_type, rhs >{} && std::is_nothrow_destructible< storage_type >{})
    {
        storage_type & lhs_storage_ = *storage_;
        if (lhs_storage_.template is_active< rhs >()) {
            lhs_storage_ = std::forward< rhs >(_rhs);
        } else {
            toggle_storage(lhs_storage_, std::forward< rhs >(_rhs));
        }
        return *this;
    }

    template< typename type >
    explicit
    constexpr
    operator type const & () const & noexcept
    {
        assert(*storage_.template is_active< type >());
        return static_cast< type const & >(*storage_);
    }

    template< typename type >
    explicit
    constexpr
    operator type & () & noexcept
    {
        assert(*storage_.template is_active< type >());
        return static_cast< type & >(*storage_);
    }

    template< typename type >
    explicit
    constexpr
    operator type const && () const && noexcept
    {
        assert(*storage_.template is_active< type >());
        return static_cast< type const && >(*storage_);
    }

    template< typename type >
    explicit
    constexpr
    operator type && () && noexcept
    {
        assert(*storage_.template is_active< type >());
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

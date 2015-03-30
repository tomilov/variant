#pragma once

#include "type_traits.hpp"
#include "recursive_wrapper.hpp"
#include "versatile.hpp"

#include <type_traits>
#include <utility>

#include <cstddef>
#include <cassert>

namespace variant
{

template< typename ...types >
struct variant
{

    static constexpr std::size_t size = sizeof...(types);

private :

    using deinitializer = versatile<>;
    using versatile = versatile< types... >;

    versatile first_;
    versatile second_{deinitializer{}};

    constexpr
    versatile const &
    storage(bool const _other = false) const & noexcept
    {
        assert(is_valid());
        return (first_.empty() != _other) ? second_ : first_;
    }

    constexpr
    versatile &
    storage(bool const _other = false) & noexcept
    {
        assert(is_valid());
        return (first_.empty() != _other) ? second_ : first_;
    }

    constexpr
    versatile const &&
    storage(bool const _other = false) const && noexcept
    {
        assert(is_valid());
        return std::move((first_.empty() != _other) ? second_ : first_);
    }

    constexpr
    versatile &&
    storage(bool const _other = false) && noexcept
    {
        assert(is_valid());
        return std::move((first_.empty() != _other) ? second_ : first_);
    }

    template< typename rhs >
    void
    toggle_storage(versatile & _old_storage, rhs && _rhs) noexcept(std::is_nothrow_constructible< versatile, rhs >{} && std::is_nothrow_destructible< versatile >{})
    {
        ::new (&storage(true)) versatile(std::forward< rhs >(_rhs));
        _old_storage.~versatile();
    }

public :

    using first_type = typename versatile::this_type;

    template< typename type >
    static
    constexpr
    std::size_t
    index() noexcept
    {
        return versatile::template index< type >();
    }

    template< std::size_t _which >
    using at = typename versatile::template at< _which >;

    constexpr
    std::size_t
    which() const noexcept
    {
        return storage().which();
    }

    constexpr
    bool
    is_valid() const noexcept
    {
        return (first_.empty() != second_.empty());
    }

    template< typename type >
    constexpr
    bool
    is_active() const noexcept
    {
        return storage().template is_active< type >();
    }

    template< typename ...arguments >
    constexpr
    variant(arguments &&... _arguments) noexcept(std::is_nothrow_constructible< versatile, arguments... >{})
        : first_(std::forward< arguments >(_arguments)...)
    { ; }

    constexpr
    variant(variant const & _rhs) noexcept(std::is_nothrow_copy_constructible< versatile >{})
        : first_(_rhs.storage())
    { ; }

    constexpr
    variant(variant & _rhs) noexcept(std::is_nothrow_copy_constructible< versatile >{})
        : first_(_rhs.storage())
    { ; }

    constexpr
    variant(variant const && _rhs) noexcept(std::is_nothrow_move_constructible< versatile >{})
        : first_(std::move(_rhs).storage())
    { ; }

    constexpr
    variant(variant && _rhs) noexcept(std::is_nothrow_move_constructible< versatile >{})
        : first_(std::move(_rhs).storage())
    { ; }

    constexpr
    variant &
    operator = (variant const & _rhs) & noexcept(std::is_nothrow_copy_constructible< versatile >{} && std::is_nothrow_copy_assignable< versatile >{} && std::is_nothrow_destructible< versatile >{})
    {
        versatile & lhs_storage_ = storage();
        versatile const & rhs_storage_ = _rhs.storage();
        if (lhs_storage_.which() == rhs_storage_.which()) {
            lhs_storage_ = rhs_storage_;
        } else {
            toggle_storage(lhs_storage_, rhs_storage_);
        }
        return *this;
    }

    constexpr
    variant &
    operator = (variant & _rhs) & noexcept(std::is_nothrow_copy_constructible< versatile >{} && std::is_nothrow_copy_assignable< versatile >{} && std::is_nothrow_destructible< versatile >{})
    {
        versatile & lhs_storage_ = storage();
        versatile & rhs_storage_ = _rhs.storage();
        if (lhs_storage_.which() == rhs_storage_.which()) {
            lhs_storage_ = rhs_storage_;
        } else {
            toggle_storage(lhs_storage_, rhs_storage_);
        }
        return *this;
    }

    constexpr
    variant &
    operator = (variant const && _rhs) & noexcept(std::is_nothrow_move_constructible< versatile >{} && std::is_nothrow_move_assignable< versatile >{} && std::is_nothrow_destructible< versatile >{})
    {
        versatile & lhs_storage_ = storage();
        versatile const && rhs_storage_ = std::move(_rhs).storage();
        if (lhs_storage_.which() == rhs_storage_.which()) {
            lhs_storage_ = std::move(rhs_storage_);
        } else {
            toggle_storage(lhs_storage_, std::move(rhs_storage_));
        }
        return *this;
    }

    constexpr
    variant &
    operator = (variant && _rhs) & noexcept(std::is_nothrow_move_constructible< versatile >{} && std::is_nothrow_move_assignable< versatile >{} && std::is_nothrow_destructible< versatile >{})
    {
        versatile & lhs_storage_ = storage();
        versatile && rhs_storage_ = std::move(_rhs).storage();
        if (lhs_storage_.which() == rhs_storage_.which()) {
            lhs_storage_ = std::move(rhs_storage_);
        } else {
            toggle_storage(lhs_storage_, std::move(rhs_storage_));
        }
        return *this;
    }

    template< typename rhs >
    constexpr
    variant &
    operator = (rhs && _rhs) & noexcept(std::is_nothrow_assignable< versatile &, rhs >{} && std::is_nothrow_constructible< versatile, rhs >{} && std::is_nothrow_destructible< versatile >{})
    {
        versatile & lhs_storage_ = storage();
        if (lhs_storage_.template is_active< rhs >()) {
            lhs_storage_ = std::forward< rhs >(_rhs);
        } else {
            toggle_storage(lhs_storage_, std::forward< rhs >(_rhs));
        }
        return *this;
    }

    constexpr
    void
    swap(variant & _other) noexcept(std::is_nothrow_move_constructible< versatile >{} && std::is_nothrow_move_assignable< versatile >{} && std::is_nothrow_destructible< versatile >{})
    {
        versatile & this_storage_ = storage();
        versatile & other_storage_ = _other.storage();
        if (other_storage_.which() == this_storage_.which()) {
            this_storage_.swap(other_storage_);
        } else {
            ::new (&_other.storage(true)) versatile(std::move_if_noexcept(this_storage_));
            toggle_storage(this_storage_, std::move_if_noexcept(other_storage_));
            other_storage_.~versatile();
        }
    }

    template< typename type >
    explicit
    constexpr
    operator type const & () const & noexcept
    {
        assert(storage().template is_active< type >());
        return static_cast< type const & >(storage());
    }

    template< typename type >
    explicit
    constexpr
    operator type & () & noexcept
    {
        assert(storage().template is_active< type >());
        return static_cast< type & >(storage());
    }

    template< typename type >
    explicit
    constexpr
    operator type const && () const && noexcept
    {
        assert(storage().template is_active< type >());
        return static_cast< type const && >(storage());
    }

    template< typename type >
    explicit
    constexpr
    operator type && () && noexcept
    {
        assert(storage().template is_active< type >());
        return static_cast< type && >(storage());
    }

private :

    template< typename type, typename result_type, typename visitor, typename visitable, typename ...arguments >
    static
    constexpr
    result_type
    caller(visitor && _visitor, visitable && _visitable, arguments &&... _arguments) noexcept(is_nothrow_callable_v< visitor, unwrap_type_t< type >, arguments... >)
    {
        // There is known clang++ bug #19917 for static_cast to rvalue reference.
        return std::forward< visitor >(_visitor)(static_cast< unwrap_type_t< type > >(std::forward< visitable >(_visitable)), std::forward< arguments >(_arguments)...);
    }

public :

    template< typename visitor, typename ...arguments >
    decltype(auto)
    apply_visitor(visitor && _visitor, arguments &&... _arguments) const &
    {
        using result_type = result_of< visitor, this_type_t< versatile const & >, arguments... >;
        using caller_type = result_type (*)(visitor && _visitor, versatile const & _visitable, arguments &&... _arguments);
        static constexpr caller_type dispatcher_[size] = {variant::caller< types const &, result_type, visitor, versatile const &, arguments... >...};
        return dispatcher_[size - which()](std::forward< visitor >(_visitor), storage(), std::forward< arguments >(_arguments)...);
    }

    template< typename visitor, typename ...arguments >
    decltype(auto)
    apply_visitor(visitor && _visitor, arguments &&... _arguments) &
    {
        using result_type = result_of< visitor, this_type_t< versatile & >, arguments... >;
        using caller_type = result_type (*)(visitor && _visitor, versatile & _visitable, arguments &&... _arguments);
        static constexpr caller_type dispatcher_[size] = {variant::caller< types &, result_type, visitor, versatile &, arguments... >...};
        return dispatcher_[size - which()](std::forward< visitor >(_visitor), storage(), std::forward< arguments >(_arguments)...);
    }

    template< typename visitor, typename ...arguments >
    decltype(auto)
    apply_visitor(visitor && _visitor, arguments &&... _arguments) const &&
    {
        using result_type = result_of< visitor, this_type_t< versatile const && >, arguments... >;
        using caller_type = result_type (*)(visitor && _visitor, versatile const && _visitable, arguments &&... _arguments);
        static constexpr caller_type dispatcher_[size] = {variant::caller< types const &&, result_type, visitor, versatile const &&, arguments... >...};
        return dispatcher_[size - which()](std::forward< visitor >(_visitor), std::move(storage()), std::forward< arguments >(_arguments)...);
    }

    template< typename visitor, typename ...arguments >
    decltype(auto)
    apply_visitor(visitor && _visitor, arguments &&... _arguments) &&
    {
        using result_type = result_of< visitor, this_type_t< versatile && >, arguments... >;
        using caller_type = result_type (*)(visitor && _visitor, versatile && _visitable, arguments &&... _arguments);
        static constexpr caller_type dispatcher_[size] = {variant::caller< types &&, result_type, visitor, versatile &&, arguments... >...};
        return dispatcher_[size - which()](std::forward< visitor >(_visitor), std::move(storage()), std::forward< arguments >(_arguments)...);
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

    using type = copy_cv_reference_t< visitable, typename std::remove_reference_t< visitable >::first_type >;

};

template< typename general_type >
struct first_type< general_type, false >
{

    using type = general_type &&;

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

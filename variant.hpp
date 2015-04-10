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

    using storage_type = versatile< types... >;

    storage_type first_{versatile<>{}};
    storage_type second_{versatile<>{}};
    std::size_t which_;

    template< typename ...arguments >
    using constructible_type = typename storage_type::template constructible_type< arguments... >;

    template< typename ...arguments >
    static
    constexpr
    std::size_t
    index_of_constructible() noexcept
    {
        return storage_type::template index_of_constructible< arguments... >();
    }

    constexpr
    storage_type const &
    storage(bool const _other = false) const & noexcept
    {
        assert(is_valid());
        return ((which_ < size) != _other) ? first_ : second_;
    }

    constexpr
    storage_type &
    storage(bool const _other = false) & noexcept
    {
        assert(is_valid());
        return ((which_ < size) != _other) ? first_ : second_;
    }

    constexpr
    storage_type const &&
    storage(bool const _other = false) const && noexcept
    {
        assert(is_valid());
        return std::move(((which_ < size) != _other) ? first_ : second_);
    }

    constexpr
    storage_type &&
    storage(bool const _other = false) && noexcept
    {
        assert(is_valid());
        return std::move(((which_ < size) != _other) ? first_ : second_);
    }

    template< typename rhs >
    void
    toggle_storage(storage_type & _destructible, rhs && _rhs) noexcept(std::is_nothrow_constructible< storage_type, rhs >{})
    {
        constexpr std::size_t index_ = index< rhs >();
        if (which_ < size) {
            which_ = index_ + size;
        } else {

        }
        ::new (&storage(true)) storage_type(std::forward< rhs >(_rhs));
        _destructible.~storage_type();
    }

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
        return (size - (which_ % size));
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
    constexpr
    void
    destructor(storage_type & _destructible) noexcept(std::is_nothrow_destructible< type >{})
    {
        static_cast< type & >(_destructible).~type();
    }

public :

    ~variant() noexcept((std::is_nothrow_destructible< types >{} && ...))
    {
        using destructor_type = void (*)(storage_type & _destructible);
        static constexpr destructor_type dispatcher_[size] = {variant::destructor< types >...};
        dispatcher_[which()](storage());
    }

private :

    template< typename type, typename ...arguments >
    static
    constexpr
    void
    constructor(storage_type & _constructible, arguments &&... _arguments) noexcept(std::is_nothrow_destructible< type >{})
    {
        ::new (&_constructible) storage_type(std::forward< arguments >(_arguments)...);
    }

    template< typename ...arguments >
    constexpr
    void
    construct(arguments &&... _arguments)
    {
        using constructor_type = void (*)(storage_type & _constructible, arguments &&... _arguments);
        static constexpr constructor_type dispatcher_[size] = {variant::constructor< types, arguments >...};
        dispatcher_[index_](first_, std::forward< arguments >(_arguments)...);
    }

public :

    constexpr
    variant(variant const & _rhs) noexcept((std::is_nothrow_copy_constructible< types >{} && ...))
    {
        construct(_rhs.storage());
    }

    constexpr
    variant(variant & _rhs) noexcept(std::is_nothrow_copy_constructible< storage_type >{})
        : which_(_rhs.which_ % (size + 1))
        , first_(_rhs.storage())
    { ; }

    constexpr
    variant(variant const && _rhs) noexcept(std::is_nothrow_move_constructible< storage_type >{})
        : which_(_rhs.which_ % (size + 1))
        , first_(std::move(_rhs).storage())
    { ; }

    constexpr
    variant(variant && _rhs) noexcept(std::is_nothrow_move_constructible< storage_type >{})
        : which_(_rhs.which_ % (size + 1))
        , first_(std::move(_rhs).storage())
    { ; }

    template< typename argument >
    constexpr
    variant(argument && _argument) noexcept(std::is_nothrow_constructible< storage_type, argument >{})
    { ; }

    template< typename ...arguments >
    constexpr
    variant(arguments &&... _arguments) noexcept(std::is_nothrow_constructible< storage_type, arguments... >{})
        : which_(index_of_constructible< arguments... >())
        , first_(std::forward< arguments >(_arguments)...)
    { ; }

    constexpr
    variant &
    operator = (variant const & _rhs) & noexcept(std::is_nothrow_copy_constructible< storage_type >{} && std::is_nothrow_copy_assignable< storage_type >{} && std::is_nothrow_destructible< storage_type >{})
    {
        storage_type & lhs_storage_ = storage();
        storage_type const & rhs_storage_ = _rhs.storage();
        if (which() == _rhs.which()) {
            lhs_storage_ = rhs_storage_;
        } else {
            toggle_storage(lhs_storage_, rhs_storage_);
        }
        return *this;
    }

    constexpr
    variant &
    operator = (variant & _rhs) & noexcept(std::is_nothrow_copy_constructible< storage_type >{} && std::is_nothrow_copy_assignable< storage_type >{} && std::is_nothrow_destructible< storage_type >{})
    {
        storage_type & lhs_storage_ = storage();
        storage_type & rhs_storage_ = _rhs.storage();
        if (lhs_storage_.which() == rhs_storage_.which()) {
            lhs_storage_ = rhs_storage_;
        } else {
            toggle_storage(lhs_storage_, rhs_storage_);
        }
        return *this;
    }

    constexpr
    variant &
    operator = (variant const && _rhs) & noexcept(std::is_nothrow_move_constructible< storage_type >{} && std::is_nothrow_move_assignable< storage_type >{} && std::is_nothrow_destructible< storage_type >{})
    {
        storage_type & lhs_storage_ = storage();
        storage_type const && rhs_storage_ = std::move(_rhs).storage();
        if (lhs_storage_.which() == rhs_storage_.which()) {
            lhs_storage_ = std::move(rhs_storage_);
        } else {
            toggle_storage(lhs_storage_, std::move(rhs_storage_));
        }
        return *this;
    }

    constexpr
    variant &
    operator = (variant && _rhs) & noexcept(std::is_nothrow_move_constructible< storage_type >{} && std::is_nothrow_move_assignable< storage_type >{} && std::is_nothrow_destructible< storage_type >{})
    {
        storage_type & lhs_storage_ = storage();
        storage_type && rhs_storage_ = std::move(_rhs).storage();
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
    operator = (rhs && _rhs) & noexcept(std::is_nothrow_assignable< storage_type &, rhs >{} && std::is_nothrow_constructible< storage_type, rhs >{} && std::is_nothrow_destructible< storage_type >{})
    {
        storage_type & lhs_storage_ = storage();
        if (lhs_storage_.template is_active< rhs >()) {
            lhs_storage_ = std::forward< rhs >(_rhs);
        } else {
            toggle_storage(lhs_storage_, std::forward< rhs >(_rhs));
        }
        return *this;
    }

    constexpr
    void
    swap(variant & _other) noexcept(std::is_nothrow_move_constructible< storage_type >{} && std::is_nothrow_move_assignable< storage_type >{} && std::is_nothrow_destructible< storage_type >{})
    {
        storage_type & this_storage_ = storage();
        storage_type & other_storage_ = _other.storage();
        if (other_storage_.which() == this_storage_.which()) {
            this_storage_.swap(other_storage_);
        } else {
            ::new (&_other.storage(true)) storage_type(std::move_if_noexcept(this_storage_));
            toggle_storage(this_storage_, std::move_if_noexcept(other_storage_));
            other_storage_.~storage_type();
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
        return std::forward< visitor >(_visitor)(unwrap(static_cast< type >(std::forward< visitable >(_visitable))), std::forward< arguments >(_arguments)...);
    }

public :

    template< typename visitor, typename ...arguments >
    decltype(auto)
    apply_visitor(visitor && _visitor, arguments &&... _arguments) const &
    {
        using result_type = result_of< visitor, at<> const &, arguments... >;
        using caller_type = result_type (*)(visitor && _visitor, storage_type const & _visitable, arguments &&... _arguments);
        static constexpr caller_type dispatcher_[size] = {variant::caller< types const &, result_type, visitor, storage_type const &, arguments... >...};
        return dispatcher_[which()](std::forward< visitor >(_visitor), storage(), std::forward< arguments >(_arguments)...);
    }

    template< typename visitor, typename ...arguments >
    decltype(auto)
    apply_visitor(visitor && _visitor, arguments &&... _arguments) &
    {
        using result_type = result_of< visitor, at<> &, arguments... >;
        using caller_type = result_type (*)(visitor && _visitor, storage_type & _visitable, arguments &&... _arguments);
        static constexpr caller_type dispatcher_[size] = {variant::caller< types &, result_type, visitor, storage_type &, arguments... >...};
        return dispatcher_[which()](std::forward< visitor >(_visitor), storage(), std::forward< arguments >(_arguments)...);
    }

    template< typename visitor, typename ...arguments >
    decltype(auto)
    apply_visitor(visitor && _visitor, arguments &&... _arguments) const &&
    {
        using result_type = result_of< visitor, at<> const &&, arguments... >;
        using caller_type = result_type (*)(visitor && _visitor, storage_type const && _visitable, arguments &&... _arguments);
        static constexpr caller_type dispatcher_[size] = {variant::caller< types const &&, result_type, visitor, storage_type const &&, arguments... >...};
        return dispatcher_[which()](std::forward< visitor >(_visitor), std::move(storage()), std::forward< arguments >(_arguments)...);
    }

    template< typename visitor, typename ...arguments >
    decltype(auto)
    apply_visitor(visitor && _visitor, arguments &&... _arguments) &&
    {
        using result_type = result_of< visitor, at<> &&, arguments... >;
        using caller_type = result_type (*)(visitor && _visitor, storage_type && _visitable, arguments &&... _arguments);
        static constexpr caller_type dispatcher_[size] = {variant::caller< types &&, result_type, visitor, storage_type &&, arguments... >...};
        return dispatcher_[which()](std::forward< visitor >(_visitor), std::move(storage()), std::forward< arguments >(_arguments)...);
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

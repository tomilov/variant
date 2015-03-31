#pragma once

#include "type_traits.hpp"
#include "recursive_wrapper.hpp"

#include <type_traits>
#include <utility>

#include <cstddef>
#include <cassert>

namespace variant
{

template< std::size_t which = 0, typename type = void >
struct indexed
{

    std::size_t which_ = which;
    type value_;

    template< typename ...arguments >
    constexpr
    indexed(arguments &&... _arguments) noexcept(std::is_nothrow_constructible< type, arguments... >{})
        : value_(std::forward< arguments >(_arguments)...)
    { ; }

    ~indexed() noexcept
    {
        which_ = 0;
    }

};

template< typename type >
struct indexed< 0, type >
{

    std::size_t which_ = 0;

};

template< typename ...types >
union versatile;

template<>
union versatile<>
{

    using this_type = void;

    static constexpr std::size_t size = 0;

private :

    using head = indexed<>;

    head head_{};

public :

    constexpr
    versatile() noexcept = default;

    template< typename type = this_type >
    static
    constexpr
    std::size_t
    index() noexcept
    {
        return 0;
    }

    template< std::size_t _which = 0 >
    using at = this_type;

    static
    constexpr
    std::size_t
    which() noexcept
    {
        return 0;
    }

    static
    constexpr
    bool
    empty() noexcept
    {
        return true;
    }

    template< typename type = this_type >
    static
    constexpr
    bool
    is_active() noexcept
    {
        return false;
    }

    static
    constexpr
    void
    swap(versatile &) noexcept
    {
        return;
    }

};

template< typename first, typename ...rest >
union versatile< first, rest... >
{

    static constexpr std::size_t size = (1 + sizeof...(rest));

private :

    using head = indexed< size, first >;
    using tail = versatile< rest... >;

    head head_;
    tail tail_;

    template< typename ...arguments >
    explicit
    constexpr
    versatile(std::true_type, arguments &&... _arguments) noexcept(std::is_nothrow_constructible< first, arguments... >{})
        : head_(std::forward< arguments >(_arguments)...)
    { ; }

    template< typename ...arguments >
    explicit
    constexpr
    versatile(std::false_type, arguments &&... _arguments) noexcept(std::is_nothrow_constructible< tail, arguments... >{})
        : tail_(std::forward< arguments >(_arguments)...)
    { ; }

public :

    using this_type = unwrap_type_t< first >;

    template< typename type = this_type >
    static
    constexpr
    std::size_t
    index() noexcept
    {
        if (std::is_same< type, this_type >{}) {
            return size;
        } else {
            return tail::template index< type >();
        }
    }

    template< std::size_t _which = 0 >
    using at = std::conditional_t< (_which == size), this_type, typename tail::template at< _which > >;

    constexpr
    std::size_t
    which() const noexcept
    {
        return head_.which_;
    }

    constexpr
    bool
    empty() const noexcept
    {
        return (which() == 0);
    }

    template< typename type = this_type >
    constexpr
    bool
    is_active() const noexcept
    {
        if (std::is_same< type, this_type >{}) {
            return (which() == size);
        } else {
            return tail_.template is_active< type >();
        }
    }

    ~versatile() noexcept(std::is_nothrow_destructible< first >{} && std::is_nothrow_destructible< tail >{})
    {
        if (is_active()) {
            head_.~head();
        } else {
            tail_.~tail();
        }
    }

    constexpr
    versatile(this_type const & _rhs) noexcept(std::is_nothrow_copy_constructible< first >{})
        : head_(_rhs)
    { ; }

    constexpr
    versatile(this_type & _rhs) noexcept(std::is_nothrow_copy_constructible< first >{})
        : head_(_rhs)
    { ; }

    constexpr
    versatile(this_type const && _rhs) noexcept(std::is_nothrow_move_constructible< first >{})
        : head_(std::move(_rhs))
    { ; }

    constexpr
    versatile(this_type && _rhs) noexcept(std::is_nothrow_move_constructible< first >{})
        : head_(std::move(_rhs))
    { ; }

    versatile(versatile const & _rhs) noexcept(std::is_nothrow_copy_constructible< first >{} && std::is_nothrow_copy_constructible< tail >{})
    {
        if (_rhs.which() == size) {
            ::new (&head_) head(static_cast< this_type const & >(_rhs.head_.value_));
        } else {
            ::new (&tail_) tail(_rhs.tail_);
        }
    }

    versatile(versatile & _rhs) noexcept(std::is_nothrow_copy_constructible< first >{} && std::is_nothrow_copy_constructible< tail >{})
    {
        if (_rhs.which() == size) {
            ::new (&head_) head(static_cast< this_type & >(_rhs.head_.value_));
        } else {
            ::new (&tail_) tail(_rhs.tail_);
        }
    }

    versatile(versatile const && _rhs) noexcept(std::is_nothrow_move_constructible< first >{} && std::is_nothrow_move_constructible< tail >{})
    {
        if (_rhs.which() == size) {
            ::new (&head_) head(static_cast< this_type const && >(std::move(_rhs.head_.value_)));
        } else {
            ::new (&tail_) tail(std::move(_rhs.tail_));
        }
    }

    versatile(versatile && _rhs) noexcept(std::is_nothrow_move_constructible< first >{} && std::is_nothrow_move_constructible< tail >{})
    {
        if (_rhs.which() == size) {
            ::new (&head_) head(static_cast< this_type && >(std::move(_rhs.head_.value_)));
        } else {
            ::new (&tail_) tail(std::move(_rhs.tail_));
        }
    }

    template< typename argument >
    constexpr
    versatile(argument && _argument) noexcept(std::is_nothrow_constructible< tail, argument >{})
        : tail_(std::forward< argument >(_argument))
    { ; }

    template< typename ...arguments >
    constexpr
    versatile(arguments &&... _arguments) noexcept(std::is_nothrow_constructible< versatile, typename std::is_constructible< this_type, arguments... >::type, arguments... >{})
        : versatile(typename std::is_constructible< this_type, arguments... >::type{}, std::forward< arguments >(_arguments)...)
    { ; }

    constexpr
    versatile &
    operator = (this_type const & _rhs) & noexcept(std::is_nothrow_copy_assignable< first >{})
    {
        operator this_type & () = _rhs;
        return *this;
    }

    constexpr
    versatile &
    operator = (this_type & _rhs) & noexcept(std::is_nothrow_copy_assignable< first >{})
    {
        operator this_type & () = _rhs;
        return *this;
    }

    constexpr
    versatile &
    operator = (this_type const && _rhs) & noexcept(std::is_nothrow_move_assignable< first >{})
    {
        operator this_type & () = std::move(_rhs);
        return *this;
    }

    constexpr
    versatile &
    operator = (this_type && _rhs) & noexcept(std::is_nothrow_move_assignable< first >{})
    {
        operator this_type & () = std::move(_rhs);
        return *this;
    }

    constexpr
    versatile &
    operator = (versatile const & _rhs) & noexcept(std::is_nothrow_copy_assignable< first >{} && std::is_nothrow_copy_assignable< tail >{})
    {
        if (_rhs.which() == size) {
            return operator = (static_cast< this_type const & >(_rhs.head_.value_));
        }
        tail_ = _rhs.tail_;
        return *this;
    }

    constexpr
    versatile &
    operator = (versatile & _rhs) & noexcept(std::is_nothrow_copy_assignable< first >{} && std::is_nothrow_copy_assignable< tail >{})
    {
        if (_rhs.which() == size) {
            return operator = (static_cast< this_type & >(_rhs.head_.value_));
        }
        tail_ = _rhs.tail_;
        return *this;
    }

    constexpr
    versatile &
    operator = (versatile const && _rhs) & noexcept(std::is_nothrow_move_assignable< first >{} && std::is_nothrow_move_assignable< tail >{})
    {
        if (_rhs.which() == size) {
            return operator = (static_cast< this_type const && >(std::move(_rhs.head_.value_)));
        }
        tail_ = std::move(_rhs.tail_);
        return *this;
    }

    constexpr
    versatile &
    operator = (versatile && _rhs) & noexcept(std::is_nothrow_move_assignable< first >{} && std::is_nothrow_move_assignable< tail >{})
    {
        if (_rhs.which() == size) {
            return operator = (static_cast< this_type && >(std::move(_rhs.head_.value_)));
        }
        tail_ = std::move(_rhs.tail_);
        return *this;
    }

    template< typename rhs >
    constexpr
    versatile &
    operator = (rhs && _rhs) & noexcept(std::is_nothrow_assignable< tail &, rhs >{})
    {
        tail_ = std::forward< rhs >(_rhs);
        return *this;
    }

    explicit
    constexpr
    operator this_type const & () const & noexcept
    {
        assert(is_active());
        return static_cast< this_type const & >(head_.value_);
    }

    template< typename type >
    explicit
    constexpr
    operator type const & () const & noexcept
    {
        return static_cast< type const & >(tail_);
    }

    explicit
    constexpr
    operator this_type & () & noexcept
    {
        assert(is_active());
        return static_cast< this_type & >(head_.value_);
    }

    template< typename type >
    explicit
    constexpr
    operator type & () & noexcept
    {
        return static_cast< type & >(tail_);
    }

    explicit
    constexpr
    operator this_type const && () const && noexcept
    {
        assert(is_active());
        return static_cast< this_type const && >(std::move(head_.value_));
    }

    template< typename type >
    explicit
    constexpr
    operator type const && () const && noexcept
    {
        return static_cast< type const && >(tail_);
    }

    explicit
    constexpr
    operator this_type && () && noexcept
    {
        assert(is_active());
        return static_cast< this_type && >(std::move(head_.value_));
    }

    template< typename type >
    explicit
    constexpr
    operator type && () && noexcept
    {
        return static_cast< type && >(tail_);
    }

    constexpr
    void
    swap(versatile & _other) noexcept(std::is_nothrow_move_constructible< versatile >::value && std::is_nothrow_move_assignable< versatile >::value)
    {
        assert(_other.which() == which());
        if (is_active()) {
            using std::swap;
            swap(_other.head_.value_, head_.value_);
        } else {
            tail_.swap(_other.tail_);
        }
    }

};

template< typename type >
struct is_versatile
        : std::false_type
{

};

template< typename ...types >
struct is_versatile< versatile< types... > >
        : std::true_type
{

};

template< typename type, bool = is_versatile< std::remove_cv_t< std::remove_reference_t< type > > >{} >
struct this_type;

template< typename visitable >
struct this_type< visitable, true >
{

    using type = copy_cv_reference_t< visitable, typename std::remove_reference_t< visitable >::this_type >;

};

template< typename general_type >
struct this_type< general_type, false >
{

    using type = general_type &&;

};

template< typename type >
using this_type_t = typename this_type< type >::type;

template< typename ...types >
constexpr
void
swap(versatile< types... > & _lhs, versatile< types... > & _rhs) noexcept
{
    assert(_lhs.which() == _rhs.which());
    _lhs.swap(_rhs);
}

}

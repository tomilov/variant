#pragma once

#include "variant.hpp"

#include <typeinfo>

namespace versatile
{

struct equal_to
{

    template< typename type >
    constexpr
    bool
    operator () (type const & _lhs, type const & _rhs) const
    {
        return static_cast< bool >(_lhs == _rhs);
    }

    template< typename lhs, typename rhs >
    bool
    operator () (lhs const &, rhs const &) const
    {
        throw std::bad_cast{};
    }

};

template< template< typename ...types > class visitable,
          typename ...lhs, typename ...rhs >
constexpr
bool
operator == (visitable< lhs... > const & _lhs, visitable< rhs... > const & _rhs)
{
    return multivisit(equal_to{}, _lhs, _rhs);
}

template< template< typename ...types > class visitable,
          typename ...lhs, typename rhs >
constexpr
bool
operator == (visitable< lhs... > const & _lhs, rhs const & _rhs)
{
    return visit(equal_to{}, _lhs, _rhs);
}

template< template< typename ...types > class visitable,
          typename lhs, typename ...rhs >
constexpr
bool
operator == (lhs const & _lhs, visitable< rhs... > const & _rhs)
{
    return multivisit(equal_to{}, _lhs, _rhs);
}

struct less
{

    template< typename type >
    constexpr
    bool
    operator () (type const & _lhs, type const & _rhs) const
    {
        return static_cast< bool >(_lhs < _rhs);
    }

    template< typename lhs, typename rhs >
    bool
    operator () (lhs const &, rhs const &) const
    {
        throw std::bad_cast{};
    }

};

template< template< typename ...types > class visitable,
          typename ...lhs, typename ...rhs >
constexpr
bool
operator < (visitable< lhs... > const & _lhs, visitable< rhs... > const & _rhs)
{
    return multivisit(less{}, _lhs, _rhs);
}

template< template< typename ...types > class visitable,
          typename rhs, typename ...lhs >
constexpr
bool
operator < (visitable< lhs... > const & _lhs, rhs const & _rhs)
{
    return visit(less{}, _lhs, _rhs);
}

template< template< typename ...types > class visitable,
          typename lhs, typename ...rhs >
constexpr
bool
operator < (lhs const & _lhs, visitable< rhs... > const & _rhs)
{
    return multivisit(less{}, _lhs, _rhs);
}

}

#pragma once

#include "variant.hpp"
#include "visit.hpp"

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

template< typename ...lhs, typename ...rhs >
bool
operator == (variant< lhs... > const & _lhs, variant< rhs... > const & _rhs)
{
    return visit(equal_to{}, _lhs, _rhs);
}

template< typename ...lhs, typename rhs >
bool
operator == (variant< lhs... > const & _lhs, rhs const & _rhs)
{
    return visit(equal_to{}, _lhs, _rhs);
}

template< typename lhs, typename ...rhs >
bool
operator == (lhs const & _lhs, variant< rhs... > const & _rhs)
{
    return visit(equal_to{}, _lhs, _rhs);
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

template< typename ...lhs, typename ...rhs >
bool
operator < (variant< lhs... > const & _lhs, variant< rhs... > const & _rhs)
{
    return visit(less{}, _lhs, _rhs);
}

template< typename rhs, typename ...lhs >
bool
operator < (variant< lhs... > const & _lhs, rhs const & _rhs)
{
    return visit(less{}, _lhs, _rhs);
}

template< typename lhs, typename ...rhs >
bool
operator < (lhs const & _lhs, variant< rhs... > const & _rhs)
{
    return visit(less{}, _lhs, _rhs);
}

}

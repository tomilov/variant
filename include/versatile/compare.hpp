#pragma once

#include "versatile/variant.hpp"
#include "versatile/visitation.hpp"

namespace versatile
{

struct equal_to
{

    template< typename type >
    constexpr
    bool
    operator () (type const & _lhs, type const & _rhs) const
    {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
        return static_cast< bool >(_lhs == _rhs);
#pragma clang diagnostic pop
    }

    template< typename lhs, typename rhs >
    constexpr
    bool
    operator () (lhs const &, rhs const &) const noexcept
    {
        return false;
    }

};

template< typename ...lhs, typename ...rhs >
bool
operator == (variant< lhs... > const & _lhs, variant< rhs... > const & _rhs)
{
    return apply_visitor(equal_to{}, _lhs, _rhs);
}

template< typename ...lhs, typename rhs >
bool
operator == (variant< lhs... > const & _lhs, rhs const & _rhs)
{
    return apply_visitor(equal_to{}, _lhs, _rhs);
}

template< typename lhs, typename ...rhs >
bool
operator == (lhs const & _lhs, variant< rhs... > const & _rhs)
{
    return apply_visitor(equal_to{}, _lhs, _rhs);
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
    constexpr
    bool
    operator () (lhs const &, rhs const &) const noexcept
    {
        return false;
    }

};

template< typename ...lhs, typename ...rhs >
bool
operator < (variant< lhs... > const & _lhs, variant< rhs... > const & _rhs)
{
    return apply_visitor(less{}, _lhs, _rhs);
}

template< typename rhs, typename ...lhs >
bool
operator < (variant< lhs... > const & _lhs, rhs const & _rhs)
{
    return apply_visitor(less{}, _lhs, _rhs);
}

template< typename lhs, typename ...rhs >
bool
operator < (lhs const & _lhs, variant< rhs... > const & _rhs)
{
    return apply_visitor(less{}, _lhs, _rhs);
}

}

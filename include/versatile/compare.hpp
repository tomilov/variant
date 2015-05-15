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
    bool
    operator () (lhs const &, rhs const &) const noexcept
    {
        return false;
    }

};

template< typename ...lhses, typename ...rhses >
bool
operator == (variant< lhses... > const & _lhs, variant< rhses... > const & _rhs)
{
    return apply_visitor(equal_to{}, _lhs, _rhs);
}

template< typename ...lhses, typename rhs >
bool
operator == (variant< lhses... > const & _lhs, rhs const & _rhs)
{
    return apply_visitor(equal_to{}, _lhs, _rhs);
}

template< typename lhs, typename ...rhses >
bool
operator == (lhs const & _lhs, variant< rhses... > const & _rhs)
{
    return apply_visitor(equal_to{}, _lhs, _rhs);
}

template< typename variant_type >
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
        return (index< variant_type, lhs > < index< variant_type, rhs >);
    }

};

template< typename ...types >
bool
operator < (variant< types... > const & _lhs, variant< types... > const & _rhs)
{
    return apply_visitor(less< variant< types... > >{}, _lhs, _rhs);
}

template< typename rhs, typename ...lhses >
bool
operator < (variant< lhses... > const & _lhs, rhs const & _rhs)
{
    return apply_visitor(less< variant< lhses... > >{}, _lhs, _rhs);
}

template< typename lhs, typename ...rhses >
bool
operator < (lhs const & _lhs, variant< rhses... > const & _rhs)
{
    return apply_visitor(less< variant< rhses... > >{}, _lhs, _rhs);
}

}

#pragma once

#include <versatile/wrappers.hpp>

namespace test
{

template< typename type >
struct aggregate
        : ::versatile::identity< ::versatile::aggregate_wrapper< type > >
{

};

template< typename type >
struct recursive_wrapper
        : ::versatile::identity< ::versatile::recursive_wrapper< type > >
{

};

}

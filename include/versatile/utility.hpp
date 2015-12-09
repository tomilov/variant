#pragma once

#include "in_place.hpp"

#include <type_traits>
#include <utility>

namespace versatile
{

template< type_qualifier type_qual, typename type,
          typename result_type = add_type_qualifier_t< type_qual, std::decay_t< type > > >
constexpr
result_type
forward_as(type && _value)
{
    return static_cast< result_type >(_value);
}

template< typename type, typename visitable >
constexpr
bool
is_active(visitable const & _visitable)
{
    return _visitable.template active< type >();
}

template< typename type, typename visitable >
constexpr
type const &
get(visitable const & _visitable)
{
    return static_cast< type const & >(_visitable);
}

template< typename type, typename visitable >
constexpr
type &
get(visitable & _visitable)
{
    return static_cast< type & >(_visitable);
}

template< typename type, typename visitable >
constexpr
type const &&
get(visitable const && _visitable)
{
    return static_cast< type const && >(static_cast< type const & >(_visitable));
}

template< typename type, typename visitable >
constexpr
type &&
get(visitable && _visitable)
{
    return static_cast< type && >(static_cast< type & >(_visitable));
}

template< typename variant, typename ...arguments >
constexpr
variant
make_variant(arguments &&... _arguments)
{
    using index = typename variant::template index_of_constructible< arguments... >;
    return {in_place< index::value >, std::forward< arguments >(_arguments)...};
}

}

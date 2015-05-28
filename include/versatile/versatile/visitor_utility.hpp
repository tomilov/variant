#pragma once

#include "type_traits.hpp"

#include <utility>
#include <type_traits>

namespace versatile
{

namespace details
{

template< typename visitor, typename ...visitors >
struct composite_visitor
    : std::decay_t< visitor >
    , composite_visitor< visitors... >
{

    using std::decay_t< visitor >::operator ();
    using composite_visitor< visitors... >::operator ();

    composite_visitor(visitor && _visitor, visitors &&... _visitors)
        : std::decay_t< visitor >(std::forward< visitor >(_visitor))
        , composite_visitor< visitors... >{std::forward< visitors >(_visitors)...}
    { ; }

};

template< typename visitor >
struct composite_visitor< visitor >
    : std::decay_t< visitor >
{

    using std::decay_t< visitor >::operator ();

    composite_visitor(visitor && _visitor)
        : std::decay_t< visitor >(std::forward< visitor >(_visitor))
    { ; }

};

}

template< typename visitor, typename ...visitors >
details::composite_visitor< visitor, visitors... >
compose_visitors(visitor && _visitor, visitors &&... _visitors)
{
    return {std::forward< visitor >(_visitor), std::forward< visitors >(_visitors)...};
}

namespace details
{

template< typename R, typename visitor >
struct add_result_type
    : std::decay_t< visitor >
{

    using result_type = R;

    add_result_type(visitor && _visitor)
        : std::decay_t< visitor >(std::forward< visitor >(_visitor))
    { ; }

};

}

template< typename result_type = void, typename visitor = void >
details::add_result_type< result_type, visitor >
add_result_type(visitor && _visitor) // for boost::apply_visitor support
{
    return {std::forward< visitor >(_visitor)};
}

}

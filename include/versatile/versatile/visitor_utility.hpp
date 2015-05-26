#pragma once

#include "type_traits.hpp"

#include <utility>
#include <type_traits>

namespace versatile
{

namespace details
{

template< typename visitor >
struct reference_invoker
{

    reference_invoker(visitor && _visitor)
        : visitor_(std::forward< visitor >(_visitor))
    { ; }

    template< typename ...arguments >
    result_of_t< visitor, arguments... >
    operator () (arguments &&... _arguments) const
    {
        return std::forward< visitor >(visitor_)(std::forward< arguments >(_arguments)...);
    }

    template< typename ...arguments >
    result_of_t< visitor, arguments... >
    operator () (arguments &&... _arguments)
    {
        return std::forward< visitor >(visitor_)(std::forward< arguments >(_arguments)...);
    }

private :

    visitor && visitor_;

};

template< typename type, typename visitor = std::decay_t< type > >
using effective_type = std::conditional_t< (std::is_constructible< visitor, type >{}), visitor, reference_invoker< type > >;

template< typename visitor, typename ...visitors >
struct composite_visitor
    : effective_type< visitor >
    , composite_visitor< visitors... >
{

    composite_visitor(visitor && _visitor, visitors &&... _visitors)
        : effective_type< visitor >(std::forward< visitor >(_visitor))
        , composite_visitor< visitors... >{std::forward< visitors >(_visitors)...}
    { ; }

};

template< typename visitor >
struct composite_visitor< visitor >
    : effective_type< visitor >
{

    composite_visitor(visitor && _visitor)
        : effective_type< visitor >(std::forward< visitor >(_visitor))
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
    : effective_type< visitor >
{

    using result_type = R;

    add_result_type(visitor && _visitor)
        : effective_type< visitor >(std::forward< visitor >(_visitor))
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

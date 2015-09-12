#pragma once

#include "recursive_wrapper.hpp"

#include <type_traits>
#include <utility>
#include <typeinfo>

#include <cassert>

namespace versatile
{

template< typename type >
struct is_visitable
        : std::false_type
{

};

namespace details
{

template< typename visitor, typename visitable, typename visitable_type, typename ...arguments >
struct dispatcher;

template< typename visitor,
          typename visitable,
          template< typename ...types > class visitable_type,
          typename ...types,
          typename ...arguments >
class dispatcher< visitor, visitable, visitable_type< types... >, arguments... >
{

    template< typename type >
    static
    decltype(auto)
    caller(visitor & _visitor, visitable & _visitable, arguments &... _arguments)
    {
        //return std::forward< visitor >(_visitor)(static_cast< type >(_visitable), std::forward< arguments >(_arguments)...); // There is known clang++ bug #19917 for static_cast to rvalue reference.
        return std::forward< visitor >(_visitor)(static_cast< type >(static_cast< type & >(_visitable)), std::forward< arguments >(_arguments)...); // workaround
    }

    static constexpr type_qualifier type_qualifier_ = type_qualifier_of< visitable && >;

    template< typename type >
    using qualify_type_t = add_qualifier_t< type_qualifier_, unwrap_type_t< type > >;

public :

    decltype(auto)
    operator () (visitor & _visitor, visitable & _visitable, arguments &... _arguments) const
    {
        assert(!(sizeof...(types) < _visitable.which()));
        using caller_type = decltype(&dispatcher::caller< qualify_type_t< typename identity< types... >::type > >);
        static constexpr caller_type callers_[sizeof...(types)] = {dispatcher::caller< qualify_type_t< types > >...};
        return callers_[sizeof...(types) - _visitable.which()](_visitor, _visitable, _arguments...);
    }

};

}

template< typename visitor, typename visitable, typename ...arguments >
decltype(auto)
visit(visitor && _visitor, visitable && _visitable, arguments &&... _arguments)
{
    static_assert(is_visitable< std::decay_t< visitable > >{});
    return details::dispatcher< visitor, visitable, std::decay_t< visitable >, arguments... >{}(_visitor, _visitable, _arguments...);
}

namespace details
{

template< typename supervisitor, typename type, bool = (is_visitable< std::decay_t< type > >{}) >
struct subvisitor;

template< typename supervisitor, typename visitable >
struct subvisitor< supervisitor, visitable, true >
{

    supervisitor & supervisitor_;
    visitable & visitable_;

    template< typename ...visited >
    constexpr
    decltype(auto)
    operator () (visited &&... _visited) const
    {
        return visit(std::forward< supervisitor >(supervisitor_), std::forward< visitable >(visitable_), std::forward< visited >(_visited)...);
    }

};

template< typename supervisitor, typename type >
struct subvisitor< supervisitor, type, false >
{

    supervisitor & supervisitor_;
    type & value_;

    template< typename ...visited >
    constexpr
    decltype(auto)
    operator () (visited &&... _visited) const
    {
        return std::forward< supervisitor >(supervisitor_)(std::forward< type >(value_), std::forward< visited >(_visited)...);
    }

};

template< typename ...visitables >
struct visitor_partially_applier;

template<>
struct visitor_partially_applier<>
{

    template< typename visitor >
    constexpr
    decltype(auto)
    operator () (visitor && _visitor) const
    {
        return std::forward< visitor >(_visitor)();
    }

};

template< typename first, typename ...rest >
struct visitor_partially_applier< first, rest... >
{

    template< typename visitor >
    constexpr
    decltype(auto)
    operator () (visitor && _visitor, first & _first, rest &... _rest) const
    {
        return visitor_partially_applier< rest... >{}(subvisitor< visitor, first >{_visitor, _first}, _rest...);
    }

};

}

template< typename visitor, typename ...visitables >
constexpr
decltype(auto)
multivisit(visitor && _visitor, visitables &&... _visitables)
{
    return details::visitor_partially_applier< visitables... >{}(std::forward< visitor >(_visitor), _visitables...);
}

namespace details
{

template< typename visitor >
struct delayed_visitor
{

    constexpr
    delayed_visitor(visitor & _visitor) noexcept(std::is_lvalue_reference< visitor >{} || std::is_nothrow_move_constructible< visitor >{})
        : visitor_(std::forward< visitor >(_visitor))
    { ; }

    template< typename ...types >
    decltype(auto)
    operator () (types &&... _values) &
    {
        return multivisit(visitor_, std::forward< types >(_values)...);
    }

    template< typename ...types >
    decltype(auto)
    operator () (types &&... _values) const &
    {
        return multivisit(visitor_, std::forward< types >(_values)...);
    }

    template< typename ...types >
    decltype(auto)
    operator () (types &&... _values) &&
    {
        return multivisit(std::move(visitor_), std::forward< types >(_values)...);
    }

    template< typename ...types >
    decltype(auto)
    operator () (types &&... _values) const &&
    {
        return multivisit(std::move(visitor_), std::forward< types >(_values)...);
    }

private :

    visitor visitor_;

};

}

template< typename visitor >
constexpr
details::delayed_visitor< visitor >
visit(visitor && _visitor) noexcept(std::is_lvalue_reference< visitor >{} || std::is_nothrow_move_constructible< visitor >{})
{
    return _visitor;
}

namespace details
{

template< typename visitor, typename ...visitors >
struct composite_visitor
        : std::decay_t< visitor >
        , composite_visitor< visitors... >
{

    using std::decay_t< visitor >::operator ();
    using composite_visitor< visitors... >::operator ();

    composite_visitor(visitor & _visitor, visitors &... _visitors)
        : std::decay_t< visitor >(std::forward< visitor >(_visitor))
        , composite_visitor< visitors... >{_visitors...}
    { ; }

};

template< typename visitor >
struct composite_visitor< visitor >
        : std::decay_t< visitor >
{

    using std::decay_t< visitor >::operator ();

    composite_visitor(visitor & _visitor)
        : std::decay_t< visitor >(std::forward< visitor >(_visitor))
    { ; }

};

}

template< typename visitor, typename ...visitors >
details::composite_visitor< visitor, visitors... >
compose_visitors(visitor && _visitor, visitors &&... _visitors)
{
    return {_visitor, _visitors...};
}

}

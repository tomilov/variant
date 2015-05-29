#pragma once

#include "type_traits.hpp"
#include "variant.hpp"

#include <type_traits>
#include <utility>

namespace versatile
{

namespace details
{

template< typename result_type, typename supervisitor, typename type, bool = (is_variant< std::remove_reference_t< type > >{}) >
struct subvisitor;

template< typename result_type, typename supervisitor, typename visitable >
struct subvisitor< result_type, supervisitor, visitable, true >
{

    supervisitor && supervisitor_;
    visitable && visitable_;

    template< typename ...visited >
    constexpr
    result_type
    operator () (visited &&... _visited) const
    {
        return std::forward< visitable >(visitable_).visit(std::forward< supervisitor >(supervisitor_), std::forward< visited >(_visited)...);
    }

};

template< typename result_type, typename supervisitor, typename type >
struct subvisitor< result_type, supervisitor, type, false >
{

    supervisitor && supervisitor_;
    type && value_;

    template< typename ...visited >
    constexpr
    result_type
    operator () (visited &&... _visited) const
    {
        return std::forward< supervisitor >(supervisitor_)(std::forward< type >(value_), std::forward< visited >(_visited)...);
    }

};

template< typename result_type, typename ...visitables >
struct visitor_partially_applier;

template< typename result_type >
struct visitor_partially_applier< result_type >
{

    template< typename visitor >
    constexpr
    result_type
    operator () (visitor && _visitor) const
    {
        return std::forward< visitor >(_visitor)();
    }

};

template< typename result_type, typename first, typename ...rest >
struct visitor_partially_applier< result_type, first, rest... >
{

    template< typename visitor >
    constexpr
    result_type
    operator () (visitor && _visitor, first && _first, rest &&... _rest) const
    {
        return visitor_partially_applier< result_type, rest... >{}(subvisitor< result_type, visitor, first >{std::forward< visitor >(_visitor), std::forward< first >(_first)}, std::forward< rest >(_rest)...);
    }

};

}

template< typename visitor, typename ...types >
constexpr
decltype(auto)
visit(visitor && _visitor, types &&... _values)
{
    using result_type = result_of_t< visitor, first_type_t< types && >... >;
    return details::visitor_partially_applier< result_type, types... >{}(std::forward< visitor >(_visitor), std::forward< types >(_values)...);
}

namespace details
{

template< typename visitor >
struct delayed_visitor
{

    constexpr
    delayed_visitor(visitor && _visitor) noexcept(std::is_lvalue_reference< visitor >{} || std::is_nothrow_move_constructible< visitor >{})
        : visitor_(std::forward< visitor >(_visitor))
    { ; }

    template< typename ...types >
    decltype(auto)
    operator () (types &&... _values) const &
    {
        return visit(visitor_, std::forward< types >(_values)...);
    }

    template< typename ...types >
    decltype(auto)
    operator () (types &&... _values) &
    {
        return visit(visitor_, std::forward< types >(_values)...);
    }

    template< typename ...types >
    decltype(auto)
    operator () (types &&... _values) const &&
    {
        return visit(std::move(visitor_), std::forward< types >(_values)...);
    }

    template< typename ...types >
    decltype(auto)
    operator () (types &&... _values) &&
    {
        return visit(std::move(visitor_), std::forward< types >(_values)...);
    }

private :

    visitor visitor_;

};

}

template< typename visitor >
constexpr
details::delayed_visitor< visitor >
make_visitor(visitor && _visitor) noexcept(std::is_lvalue_reference< visitor >{} || std::is_nothrow_move_constructible< visitor >{})
{
    return std::forward< visitor >(_visitor);
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

}

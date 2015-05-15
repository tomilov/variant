#pragma once

#include "type_traits.hpp"
#include "variant.hpp"

#include <type_traits>
#include <utility>

namespace versatile
{

namespace visitation
{

template< typename result_type, typename supervisitor, typename type, bool = (is_variant< std::decay_t< type > >{}) >
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
        return std::forward< visitable >(visitable_).apply_visitor(std::forward< supervisitor >(supervisitor_), std::forward< visited >(_visited)...);
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

template< typename visitor, typename first, typename ...rest >
constexpr
decltype(auto)
apply_visitor(visitor && _visitor, first && _first, rest &&... _rest)
{
    using result_type = result_of< visitor, first_type_t< first && >, first_type_t< rest && >... >;
    return visitation::visitor_partially_applier< result_type, first, rest... >{}(std::forward< visitor >(_visitor), std::forward< first >(_first), std::forward< rest >(_rest)...);
}

namespace visitation
{

template< typename visitor >
struct delayed_visitor_applier
{

    visitor visitor_;

    template< typename first, typename ...rest >
    decltype(auto)
    operator () (first && _first, rest &&... _rest) const &
    {
        return apply_visitor(visitor_, std::forward< first >(_first), std::forward< rest >(_rest)...);
    }

    template< typename first, typename ...rest >
    decltype(auto)
    operator () (first && _first, rest &&... _rest) &
    {
        return apply_visitor(visitor_, std::forward< first >(_first), std::forward< rest >(_rest)...);
    }

    template< typename first, typename ...rest >
    decltype(auto)
    operator () (first && _first, rest &&... _rest) const &&
    {
        return apply_visitor(std::move(visitor_), std::forward< first >(_first), std::forward< rest >(_rest)...);
    }

    template< typename first, typename ...rest >
    decltype(auto)
    operator () (first && _first, rest &&... _rest) &&
    {
        return apply_visitor(std::move(visitor_), std::forward< first >(_first), std::forward< rest >(_rest)...);
    }

};

}

template< typename visitor >
constexpr
visitation::delayed_visitor_applier< visitor >
apply_visitor(visitor && _visitor) noexcept(std::is_lvalue_reference< visitor >{} || std::is_nothrow_move_constructible< visitor >{})
{
    return {std::forward< visitor >(_visitor)};
}

}

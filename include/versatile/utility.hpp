#pragma once

#include "in_place.hpp"
#include "wrappers.hpp"
#include "visit.hpp"

#include <type_traits>
#include <utility>

namespace versatile
{

template< typename visitable, typename type >
struct index;

template< template< typename ...types > class visitable,
          typename ...types,
          typename type >
struct index< visitable< types... >, type >
        : index_at< type, types... >
{

};

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

namespace details
{

template< typename visitor, typename ...visitors >
struct composite_visitor
        : unwrap_type_t< visitor >
        , composite_visitor< visitors... >
{

    using head = unwrap_type_t< visitor >;
    using tail = composite_visitor< visitors... >;

    using head::operator ();
    using tail::operator ();

    constexpr
    composite_visitor(visitor & _visitor, visitors &... _visitors) noexcept(noexcept(::new (std::declval< void * >()) head(std::declval< visitor >())) && noexcept(tail(_visitors...)))
        : head(std::forward< visitor >(_visitor))
        , tail{_visitors...}
    { ; }

};

template< typename visitor >
struct composite_visitor< visitor >
        : unwrap_type_t< visitor >
{

    using base = unwrap_type_t< visitor >;

    using base::operator ();

    constexpr
    composite_visitor(visitor & _visitor) noexcept(noexcept(::new (std::declval< void * >()) base(std::declval< visitor >())))
        : base(std::forward< visitor >(_visitor))
    { ; }

};

}

template< typename visitor, typename ...visitors,
          typename result_type = details::composite_visitor< visitor, visitors... > >
constexpr
result_type
compose_visitors(visitor && _visitor, visitors &&... _visitors) noexcept(noexcept(::new (std::declval< void * >()) result_type{_visitor, _visitors...}))
{
    return {_visitor, _visitors...};
}

template< typename visitable, typename ...arguments >
constexpr // lambda not constexpr yet
decltype(auto)
invoke(visitable && _visitable, arguments &&... _arguments)
{
    return visit([&] (auto && _value) -> decltype(auto)
    {
        return std::forward< decltype(_value) >(_value)(std::forward< arguments >(_arguments)...);
    }, std::forward< visitable >(_visitable));
}

}

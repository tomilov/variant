#pragma once

#include <type_traits>

namespace versatile
{

template< typename ...types >
struct identity;

template<>
struct identity<>
{

    using type = void;

};

template< typename first, typename ...rest >
struct identity< first, rest... >
{

    using type = first;

};

enum class type_qualifier
{
    value,
    lref,
    rref,
    const_value,
    const_lref,
    const_rref,
    volatile_value,
    volatile_lref,
    volatile_rref,
    volatile_const_value,
    volatile_const_lref,
    volatile_const_rref,

    count
};

template< type_qualifier type_qualifier, typename type > struct add_type_qualifier;
template< typename to > struct add_type_qualifier< type_qualifier::value                , to > { using type =          to         ; };
template< typename to > struct add_type_qualifier< type_qualifier::lref                 , to > { using type =          to       & ; };
template< typename to > struct add_type_qualifier< type_qualifier::rref                 , to > { using type =          to       &&; };
template< typename to > struct add_type_qualifier< type_qualifier::const_value          , to > { using type =          to const   ; };
template< typename to > struct add_type_qualifier< type_qualifier::const_lref           , to > { using type =          to const & ; };
template< typename to > struct add_type_qualifier< type_qualifier::const_rref           , to > { using type =          to const &&; };
template< typename to > struct add_type_qualifier< type_qualifier::volatile_value       , to > { using type = volatile to         ; };
template< typename to > struct add_type_qualifier< type_qualifier::volatile_lref        , to > { using type = volatile to       & ; };
template< typename to > struct add_type_qualifier< type_qualifier::volatile_rref        , to > { using type = volatile to       &&; };
template< typename to > struct add_type_qualifier< type_qualifier::volatile_const_value , to > { using type = volatile to const   ; };
template< typename to > struct add_type_qualifier< type_qualifier::volatile_const_lref  , to > { using type = volatile to const & ; };
template< typename to > struct add_type_qualifier< type_qualifier::volatile_const_rref  , to > { using type = volatile to const &&; };

template< type_qualifier type_qualifier, typename to >
using add_qualifier_t = typename add_type_qualifier< type_qualifier, to >::type;

template< typename type > constexpr type_qualifier type_qualifier_of                           = type_qualifier::value                ;
template< typename type > constexpr type_qualifier type_qualifier_of<          type       &  > = type_qualifier::lref                 ;
template< typename type > constexpr type_qualifier type_qualifier_of<          type       && > = type_qualifier::rref                 ;
template< typename type > constexpr type_qualifier type_qualifier_of<          type const    > = type_qualifier::const_value          ;
template< typename type > constexpr type_qualifier type_qualifier_of<          type const &  > = type_qualifier::const_lref           ;
template< typename type > constexpr type_qualifier type_qualifier_of<          type const && > = type_qualifier::const_rref           ;
template< typename type > constexpr type_qualifier type_qualifier_of< volatile type          > = type_qualifier::volatile_value       ;
template< typename type > constexpr type_qualifier type_qualifier_of< volatile type       &  > = type_qualifier::volatile_lref        ;
template< typename type > constexpr type_qualifier type_qualifier_of< volatile type       && > = type_qualifier::volatile_rref        ;
template< typename type > constexpr type_qualifier type_qualifier_of< volatile type const    > = type_qualifier::volatile_const_value ;
template< typename type > constexpr type_qualifier type_qualifier_of< volatile type const &  > = type_qualifier::volatile_const_lref  ;
template< typename type > constexpr type_qualifier type_qualifier_of< volatile type const && > = type_qualifier::volatile_const_rref  ;

template< typename from, typename to >
using copy_cv_reference_t = add_qualifier_t< type_qualifier_of< from >, to >;

template< typename type, typename ...types >
struct index_by_type
{

    static_assert((0 < sizeof...(types)), "type is not contained within types provided");

};

template< typename type, typename ...rest >
struct index_by_type< type, type, rest... >
        : std::integral_constant< std::size_t, sizeof...(rest) >
{

};

template< typename type, typename first, typename ...rest >
struct index_by_type< type, first, rest... >
        : index_by_type< type, rest... >
{

};

template< std::size_t index, typename ...types >
struct type_by_index
{

    static_assert((index < sizeof...(types)), "index exceed the number of types provided");

};

template< typename first, typename ...rest >
struct type_by_index< sizeof...(rest), first, rest... >
    : identity< first >
{

};

template< std::size_t index, typename first, typename ...rest >
struct type_by_index< index, first, rest... >
        : type_by_index< index, rest... >
{

};

}

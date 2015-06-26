#pragma once

#include <type_traits>

namespace versatile
{

template< typename first, typename ...rest >
struct identity
{

    using type = first;

};

enum class type_qualifiers
{
    value,
    const_value,
    lref,
    const_lref,
    rref,
    const_rref,
    volatile_value,
    volatile_const_value,
    volatile_lref,
    volatile_const_lref,
    volatile_rref,
    volatile_const_rref,

    count
};

template< type_qualifiers get_type_qualifier, typename type > struct add_type_qualifier;
template< typename to > struct add_type_qualifier< type_qualifiers::value                , to > { using type =          to         ; };
template< typename to > struct add_type_qualifier< type_qualifiers::const_value          , to > { using type =          to const   ; };
template< typename to > struct add_type_qualifier< type_qualifiers::lref                 , to > { using type =          to       & ; };
template< typename to > struct add_type_qualifier< type_qualifiers::const_lref           , to > { using type =          to const & ; };
template< typename to > struct add_type_qualifier< type_qualifiers::rref                 , to > { using type =          to       &&; };
template< typename to > struct add_type_qualifier< type_qualifiers::const_rref           , to > { using type =          to const &&; };
template< typename to > struct add_type_qualifier< type_qualifiers::volatile_value       , to > { using type = volatile to         ; };
template< typename to > struct add_type_qualifier< type_qualifiers::volatile_const_value , to > { using type = volatile to const   ; };
template< typename to > struct add_type_qualifier< type_qualifiers::volatile_lref        , to > { using type = volatile to       & ; };
template< typename to > struct add_type_qualifier< type_qualifiers::volatile_const_lref  , to > { using type = volatile to const & ; };
template< typename to > struct add_type_qualifier< type_qualifiers::volatile_rref        , to > { using type = volatile to       &&; };
template< typename to > struct add_type_qualifier< type_qualifiers::volatile_const_rref  , to > { using type = volatile to const &&; };

template< type_qualifiers get_type_qualifier, typename to >
using add_qualifier_t = typename add_type_qualifier< get_type_qualifier, to >::type;

template< typename type > constexpr type_qualifiers get_type_qualifier                           = type_qualifiers::value                ;
template< typename type > constexpr type_qualifiers get_type_qualifier<          type const    > = type_qualifiers::const_value          ;
template< typename type > constexpr type_qualifiers get_type_qualifier<          type       &  > = type_qualifiers::lref                 ;
template< typename type > constexpr type_qualifiers get_type_qualifier<          type const &  > = type_qualifiers::const_lref           ;
template< typename type > constexpr type_qualifiers get_type_qualifier<          type       && > = type_qualifiers::rref                 ;
template< typename type > constexpr type_qualifiers get_type_qualifier<          type const && > = type_qualifiers::const_rref           ;
template< typename type > constexpr type_qualifiers get_type_qualifier< volatile type          > = type_qualifiers::volatile_value       ;
template< typename type > constexpr type_qualifiers get_type_qualifier< volatile type const    > = type_qualifiers::volatile_const_value ;
template< typename type > constexpr type_qualifiers get_type_qualifier< volatile type       &  > = type_qualifiers::volatile_lref        ;
template< typename type > constexpr type_qualifiers get_type_qualifier< volatile type const &  > = type_qualifiers::volatile_const_lref  ;
template< typename type > constexpr type_qualifiers get_type_qualifier< volatile type       && > = type_qualifiers::volatile_rref        ;
template< typename type > constexpr type_qualifiers get_type_qualifier< volatile type const && > = type_qualifiers::volatile_const_rref  ;

template< typename from, typename to >
using copy_cv_reference_t = add_qualifier_t< get_type_qualifier< from >, to >;

template< typename type, typename ...arguments >
using result_of_t = decltype(std::declval< type >()(std::declval< arguments >()...));

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

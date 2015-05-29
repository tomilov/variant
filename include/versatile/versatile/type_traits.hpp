#pragma once

#include <type_traits>

namespace versatile
{

template< typename from, typename to >
struct copy_reference
{

    using type = to;

};

template< typename from, typename to >
struct copy_reference< from &, to >
{

    using type = to &;

};

template< typename from, typename to >
struct copy_reference< from &&, to >
{

    using type = to &&;

};

template< typename from, typename to >
struct copy_cv
{

    using type = to;

};

template< typename from, typename to >
struct copy_cv< volatile from const, to >
{

    using type = volatile to const;

};

template< typename from, typename to >
struct copy_cv< from const, to >
{

    using type = to const;

};

template< typename from, typename to >
struct copy_cv< volatile from, to >
{

    using type = volatile to;

};

template< typename from, typename to >
using copy_reference_t = typename copy_reference< from, to >::type;

template< typename from, typename to >
using copy_cv_t = typename copy_cv< from, to >::type;

template< typename from, typename to >
using copy_cv_reference_t = copy_reference_t< from, copy_cv_t< std::remove_reference_t< from >, to > >;

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
{

    using type = first;

};

template< std::size_t index, typename first, typename ...rest >
struct type_by_index< index, first, rest... >
        : type_by_index< index, rest... >
{

};

}

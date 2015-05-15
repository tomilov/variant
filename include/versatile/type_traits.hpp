#pragma once

#include <type_traits>

namespace versatile
{

namespace type_traits
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

}

template< typename from, typename to >
using copy_reference_t = typename type_traits::copy_reference< from, to >::type;

template< typename from, typename to >
using copy_cv_t = typename type_traits::copy_cv< from, to >::type;

template< typename from, typename to >
using copy_cv_reference_t = copy_reference_t< from, copy_cv_t< std::remove_reference_t< from >, to > >;

template< typename type, typename ...arguments >
using result_of = decltype(std::declval< type >()(std::declval< arguments >()...));

template< typename type, typename ...arguments >
constexpr bool is_nothrow_callable_v = noexcept(std::declval< type >()(std::declval< arguments >()...));

}

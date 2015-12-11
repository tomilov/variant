#pragma once

#include "type_traits.hpp"

namespace versatile
{

struct in_place_t {};

template< typename type >
constexpr
in_place_t
in_place(type)
{
    return {};
}

template< std::size_t i >
constexpr
in_place_t
in_place(index_t< i >)
{
    return {};
}

}

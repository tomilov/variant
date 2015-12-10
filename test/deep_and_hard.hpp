#pragma once

#include "prologue.hpp"

#include <boost/variant/static_visitor.hpp>

#include <array>

#ifndef COLS
#define COLS 4
#endif

#ifndef ROWS
#define ROWS COLS
#endif

namespace test
{

template< std::size_t >
struct T
{

};

template< std::size_t N >
struct visitor
        : ::boost::static_visitor< std::array< std::size_t, N > >
{

    template< std::size_t ...I >
    CONSTEXPRF
    typename visitor::result_type
    operator () (T< I >...) const noexcept
    {
        return {{{I}...}};
    }

};


template< std::size_t ...M, std::size_t ...N >
CONSTEXPRF
bool
invoke(std::index_sequence< M... >, std::index_sequence< N... >) noexcept;

template< std::size_t M, std::size_t N = M >
CONSTEXPRF
bool
hard() noexcept
{
    return invoke(std::make_index_sequence< M >{}, std::make_index_sequence< N >{});
}

}

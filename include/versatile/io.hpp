#pragma once

#include "variant.hpp"
#include "visit.hpp"

namespace versatile
{

template< typename first, typename ...rest >
std::ostream &
operator << (std::ostream & _out, variant< first, rest... > const & _variant) // http://stackoverflow.com/questions/23355117/
{
    return _variant.visit([&] (auto const & _value) -> std::ostream & { return _out << _value; });
}

template< typename first, typename ...rest >
std::istream &
operator >> (std::istream & _in, variant< first, rest... > & _variant) // http://stackoverflow.com/questions/23355117/
{
    return _variant.visit([&] (auto & _value) -> std::istream & { return _in >> _value; });
}

}

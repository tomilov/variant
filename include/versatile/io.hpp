#pragma once

#include "variant.hpp"
#include "visit.hpp"

namespace versatile
{

namespace details
{

struct out
{

    template< typename type >
    std::ostream &
    operator () (type const & _value) const
    {
        return out_ << _value;
    }

    std::ostream & out_;

};

}

template< typename first, typename ...rest >
std::ostream &
operator << (std::ostream & _out, variant< first, rest... > const & _variant) // http://stackoverflow.com/questions/23355117/
{
    return visit(details::out{_out}, _variant);
}

namespace details
{

struct in
{

    template< typename type >
    std::istream &
    operator () (type & _value) const
    {
        return in_ >> _value;
    }

    std::istream & in_;

};

}

template< typename first, typename ...rest >
std::istream &
operator >> (std::istream & _in, variant< first, rest... > & _variant) // http://stackoverflow.com/questions/23355117/
{
    return visit(details::in{_in}, _variant);
}

}
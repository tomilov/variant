#pragma once

#include "versatile/variant.hpp"
#include "versatile/visitation.hpp"

namespace versatile
{

namespace io
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
    return _variant.apply_visitor(io::out{_out});
}

namespace io
{

struct in
{

    template< typename type >
    std::istream &
    operator () (type & _value) const
    {
        return out_ >> _value;
    }

    std::istream & out_;

};

}

template< typename first, typename ...rest >
std::istream &
operator >> (std::istream & _in, variant< first, rest... > & _variant) // http://stackoverflow.com/questions/23355117/
{
    return _variant.apply_visitor(io::in{_in});
}

}

#pragma once

#include <utility>
#include <type_traits>

namespace versatile
{

namespace details
{
 
template< typename visitor, typename ...visitors >
struct composite_visitor
    : std::remove_reference_t< visitor >
    , composite_visitor< visitors... >
{
   
    using this_type = std::remove_reference_t< visitor >;
    using base_type = composite_visitor< visitors... >;
   
    composite_visitor(visitor && _visitor, visitors &&... _visitors)
        : this_type(std::forward< visitor >(_visitor))
        , base_type(std::forward< visitors >(_visitors)...)
    { ; }
   
};
 
template<  typename visitor >
struct composite_visitor< visitor >
    : std::remove_reference_t< visitor >
{
   
    using this_type = std::remove_reference_t< visitor >;
   
    composite_visitor(visitor && _visitor)
        : this_type(std::forward< visitor >(_visitor))
    { ; }
   
};

}
 
template< typename visitor, typename ...visitors >
details::composite_visitor< visitor, visitors... >
compose_visitors(visitor && _visitor, visitors &&... _visitors)
{
    return {std::forward< visitor >(_visitor), std::forward< visitors >(_visitors)...};
}

namespace details
{

template< typename R, typename visitor >
struct add_result_type
    : std::remove_reference_t< visitor >
{

    using result_type = R;
    using visitor_type = std::remove_reference_t< visitor >;

    add_result_type(visitor && _visitor)
        : visitor_type{std::forward< visitor >(_visitor)}
    { ; }

};

}

template< typename result_type = void, typename visitor = void >
details::add_result_type< result_type, visitor >
add_result_type(visitor && _visitor)
{
    return {std::forward< visitor >(_visitor)};
}

}

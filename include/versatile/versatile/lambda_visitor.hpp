#pragma once

#include <utility>
#include <type_traits>

namespace versatile
{

template< typename ...lambdas >
struct lambda_visitor;
 
template< typename lambda, typename ...lambdas >
struct lambda_visitor< lambda, lambdas... >
    : std::remove_reference_t< lambda >
    , lambda_visitor< lambdas... >
{
   
    using this_type = std::remove_reference_t< lambda >;
    using base_type = lambda_visitor< lambdas... >;
   
    using this_type::operator ();
    using base_type::operator ();
   
    lambda_visitor(lambda && _lambda, lambdas &&... _lambdas)
        : this_type(std::forward< lambda >(_lambda))
        , base_type(std::forward< lambdas >(_lambdas)...)
    { ; }
   
};
 
template<  typename lambda >
struct lambda_visitor< lambda >
    : std::remove_reference_t< lambda >
{
   
    using this_type = std::remove_reference_t< lambda >;
   
    using this_type::operator ();
   
    lambda_visitor(lambda && _lambda)
        : this_type(std::forward< lambda >(_lambda))
    { ; }
   
};
 
template< typename lambda, typename ...lambdas >
lambda_visitor< lambda, lambdas... >
make_lambda_visitor(lambda && _lambda, lambdas &&... _lambdas)
{
    return {std::forward< lambda >(_lambda), std::forward< lambdas >(_lambdas)...};
}

}

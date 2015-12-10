#include "deep_and_hard.hpp"

#include <boost/variant.hpp>
#include <boost/variant/multivisitors.hpp>

#include <utility>

#include <cstdlib>

namespace test
{

template< std::size_t ...M, std::size_t ...N >
bool
invoke(std::index_sequence< M... >, std::index_sequence< N... >) noexcept
{
    using V = ::boost::variant< T< M >... >;
    V variants_[sizeof...(N)] = {{T< (N % sizeof...(M)) >{}}...};
    visitor< sizeof...(N) > visitor_;
    auto const rhs_ = ::boost::apply_visitor(visitor_, variants_[N]...);
    decltype(rhs_) lhs_ = {{{N % sizeof...(M)}...}};
    return ((lhs_[N] == rhs_[N]) && ...);
}

}

int
main()
{
    assert ((test::hard< ROWS, COLS >()));
    return EXIT_SUCCESS;
}

#include "deep_and_hard.hpp"

#include <eggs/variant.hpp>

#include <utility>

#include <cstdlib>

namespace test
{

template< std::size_t ...M, std::size_t ...N >
CONSTEXPRF
bool
invoke(std::index_sequence< M... >, std::index_sequence< N... >) noexcept
{
    using V = ::eggs::variant< T< M >... >;
    V variants_[sizeof...(N)] = {{T< (N % sizeof...(M)) >{}}...};
    visitor< sizeof...(N) > visitor_;
    auto const rhs_ = ::eggs::variants::apply(visitor_, variants_[N]...);
    decltype(rhs_) lhs_ = {{{N % sizeof...(M)}...}};
    return ((lhs_[N] == rhs_[N]) && ...);
}

}

int
main()
{
    ASSERT ((test::hard< ROWS, COLS >()));
    return EXIT_SUCCESS;
}

#include "traits.hpp"
#include "variant.hpp"
#include "visit.hpp"

#include <cstdlib>

int
main()
{
    using ::versatile::identity;
    using ::test::aggregate;
    using ::test::recursive_wrapper;
    using ::test::literal_type;
    using ::test::common_type;
    using ::test::check_invariants;
    using ::test::check_trivial;
    using ::test::check_destructible;
    using ::test::check_common;
    using ::test::test_perferct_forwarding;
    { // variant
        using ::versatile::variant;
        {
            assert ((check_destructible< identity,          variant >::run()));
            assert ((check_destructible< aggregate,         variant >::run()));
            assert ((check_destructible< recursive_wrapper, variant >::run()));
        }
        {
            assert ((check_common< identity,          variant >::run()));
            assert ((check_common< aggregate,         variant >::run()));
            assert ((check_common< recursive_wrapper, variant >::run()));
        }
        {
            assert ((test_perferct_forwarding< common_type, variant, identity,          2, 2 >::run()));
            assert ((test_perferct_forwarding< common_type, variant, aggregate,         2, 2 >::run()));
            assert ((test_perferct_forwarding< common_type, variant, recursive_wrapper, 2, 2 >::run()));
        }
    }
    return EXIT_SUCCESS;
}

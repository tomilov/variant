#include "traits.hpp"
#include "variant.hpp"
#include "visit.hpp"
#include "boost_variant.hpp"

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
    using ::test::check_common;
    using ::test::test_perferct_forwarding;
    { // boost::variant
        {
            struct L {};
            SA(std::is_literal_type< L >{});
            SA(!std::is_literal_type< ::boost::variant< L > >{});
        }
        using ::test_boost_variant::variant_i;
        using ::test_boost_variant::variant_c;
        using ::test_boost_variant::boost_recursive_wrapper;
        {
            {
                {
                    assert ((test_perferct_forwarding< literal_type, variant_i, identity,                2, 2 >::run()));
                    assert ((test_perferct_forwarding< literal_type, variant_c, boost_recursive_wrapper, 2, 2 >::run()));
                }
                {
                    assert ((test_perferct_forwarding< literal_type, variant_i, identity,                2, 2 >::run()));
                    assert ((test_perferct_forwarding< literal_type, variant_c, boost_recursive_wrapper, 2, 2 >::run()));
                }
            }
            {
                {
                    assert ((test_perferct_forwarding< common_type, variant_i, identity,                2, 2 >::run()));
                    assert ((test_perferct_forwarding< common_type, variant_c, boost_recursive_wrapper, 2, 2 >::run()));
                }
                {
                    assert ((test_perferct_forwarding< common_type, variant_i, identity,                2, 2 >::run()));
                    assert ((test_perferct_forwarding< common_type, variant_c, boost_recursive_wrapper, 2, 2 >::run()));
                }
            }
        }
    }
    return EXIT_SUCCESS;
}

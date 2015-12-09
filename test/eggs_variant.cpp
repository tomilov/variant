#include "eggs_variant.hpp"
#include "visit.hpp"

#include <versatile/type_traits.hpp>

#include <cstdlib>

int
main()
{
    using ::versatile::identity;
    using ::test::literal_type;
    using ::test::common_type;
    using ::test::test_perferct_forwarding;
    { // eggs::variant
        using ::test_eggs_variant::eggs_variant_c;
        {
            ASSERT ((test_perferct_forwarding< literal_type, eggs_variant_c, identity, 2, 2 >::run()));
            assert ((test_perferct_forwarding< common_type,  eggs_variant_c, identity, 2, 2 >::run()));
        }
    }
    return EXIT_SUCCESS;
}

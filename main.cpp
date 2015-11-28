#if 1
#include <versatile.hpp>

#include <type_traits>
#include <utility>
#include <functional>
#include <array>

#if defined(DEBUG) || defined(_DEBUG)
#include <iostream>
#include <iomanip>
#endif

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

//#define VERSATILE_RUNTIME

#define STR(S) #S
// double expansion of next macro argument
#define STRN(N) STR(N)
#define LOCATION "file '" __FILE__ "', line: " STRN(__LINE__)
#ifdef VERSATILE_RUNTIME
#define ASSERT(...) { assert((__VA_ARGS__)); }
#define CONSTEXPR const
#define CONSTEXPRF
#define CHECK(...) { assert((__VA_ARGS__)); }
#define CBRA {
#define CKET }
#else
#define ASSERT(...) { static_assert(__VA_ARGS__, LOCATION); }
#define CONSTEXPR constexpr
#define CONSTEXPRF constexpr
#define CHECK(...) { if (!(__VA_ARGS__)) return false; }
#define CBRA { struct _ { static constexpr bool call() noexcept {
#define CKET return true; } }; static_assert(_::call(), LOCATION); }
#endif

namespace test_traits
{

struct A {};
struct B {};

using ::versatile::copy_cv_reference_t;

static_assert(std::is_same_v< copy_cv_reference_t<          A         , B >,          B          >, LOCATION);
static_assert(std::is_same_v< copy_cv_reference_t<          A const   , B >,          B const    >, LOCATION);
static_assert(std::is_same_v< copy_cv_reference_t< volatile A         , B >, volatile B          >, LOCATION);
static_assert(std::is_same_v< copy_cv_reference_t< volatile A const   , B >, volatile B const    >, LOCATION);
static_assert(std::is_same_v< copy_cv_reference_t<          A        &, B >,          B        & >, LOCATION);
static_assert(std::is_same_v< copy_cv_reference_t<          A const  &, B >,          B const  & >, LOCATION);
static_assert(std::is_same_v< copy_cv_reference_t< volatile A        &, B >, volatile B        & >, LOCATION);
static_assert(std::is_same_v< copy_cv_reference_t< volatile A const  &, B >, volatile B const  & >, LOCATION);
static_assert(std::is_same_v< copy_cv_reference_t<          A       &&, B >,          B       && >, LOCATION);
static_assert(std::is_same_v< copy_cv_reference_t<          A const &&, B >,          B const && >, LOCATION);
static_assert(std::is_same_v< copy_cv_reference_t< volatile A       &&, B >, volatile B       && >, LOCATION);
static_assert(std::is_same_v< copy_cv_reference_t< volatile A const &&, B >, volatile B const && >, LOCATION);

}

namespace test_variant
{

template< typename first >
struct aggregate
        : ::versatile::identity< ::versatile::aggregate_wrapper< first > >
{

};

template< std::size_t I = 0 >
struct literal_type
{

    std::size_t i = I;

    CONSTEXPRF
    operator std::size_t () const noexcept
    {
        return i;
    }

};

static_assert(std::is_literal_type_v< literal_type<> >, LOCATION);

template< std::size_t I = 0 >
struct common_type
{

    std::size_t i = I;

    CONSTEXPRF
    operator std::size_t () const noexcept
    {
        return i;
    }

    common_type() = default;
    common_type(common_type const &) = default;
    ~common_type() noexcept { ; }

};

static_assert(!std::is_literal_type_v< common_type<> >, LOCATION);

template< template< typename ... > class wrapper = ::versatile::identity,
          template< typename ... > class variant = ::versatile::versatile >
struct check_invariants
{

    template< typename ...types >
    using V = variant< typename wrapper< types >::type... >;

    struct literal_type_invariants
    {

        using X = literal_type<>;
        static_assert(std::is_literal_type_v< X >, LOCATION);
        static_assert(std::is_default_constructible_v< X >, LOCATION);
        static_assert(!std::is_trivially_default_constructible_v< X >, LOCATION);
        static_assert(std::is_trivially_copyable_v< X >, LOCATION);
        using U = V< X >;
        static_assert(std::is_literal_type_v< U >, LOCATION);
        static_assert(std::is_default_constructible_v< U >, LOCATION);
        static_assert(!std::is_trivially_default_constructible_v< U >, LOCATION);
        static_assert(std::is_trivially_copyable_v< U >, LOCATION);

    };

    struct common_type_invariants
    {

        using X = common_type<>;
        static_assert(std::is_default_constructible_v< X >, LOCATION);
        static_assert(!std::is_trivially_default_constructible_v< X >, LOCATION);
        static_assert(std::is_destructible_v< X >, LOCATION);
        static_assert(!std::is_trivially_destructible_v< X >, LOCATION);
        static_assert(std::is_copy_constructible_v< X >, LOCATION);
        static_assert(std::is_move_constructible_v< X >, LOCATION);
        static_assert(!std::is_trivially_copy_constructible_v< X >, LOCATION);
        static_assert(!std::is_trivially_move_constructible_v< X >, LOCATION);
        static_assert(std::is_copy_assignable_v< X >, LOCATION);
        static_assert(std::is_move_assignable_v< X >, LOCATION);
        static_assert(std::is_trivially_copy_assignable_v< X >, LOCATION); // user-provided
        static_assert(std::is_trivially_move_assignable_v< X >, LOCATION);
        using U = V< X >;
        static_assert(std::is_default_constructible_v< U >, LOCATION);
        static_assert(!std::is_trivially_default_constructible_v< U >, LOCATION);
        static_assert(std::is_destructible_v< U >, LOCATION);
        static_assert(!std::is_trivially_destructible_v< U >, LOCATION);
        static_assert(std::is_copy_constructible_v< U >, LOCATION);
        static_assert(std::is_move_constructible_v< U >, LOCATION);
        static_assert(!std::is_trivially_copy_constructible_v< U >, LOCATION);
        static_assert(!std::is_trivially_move_constructible_v< U >, LOCATION);
        static_assert(std::is_copy_assignable_v< U >, LOCATION);
        static_assert(std::is_move_assignable_v< U >, LOCATION);
        static_assert(std::is_trivially_copy_assignable_v< U >, LOCATION); // user-provided

    };

    struct trivial
    {

        struct X {};
        static_assert(std::is_trivial_v< X >, LOCATION);
        static_assert(std::is_literal_type_v< X >, LOCATION);
        static_assert(std::is_standard_layout_v< X >, LOCATION);
        static_assert(std::is_pod_v< X >, LOCATION);
        using U = V< X >;
        static_assert(std::is_trivial_v< U >, LOCATION);
        static_assert(std::is_literal_type_v< U >, LOCATION);
        static_assert(std::is_standard_layout_v< U >, LOCATION);
        static_assert(std::is_pod_v< U >, LOCATION);
        static_assert(!std::is_union_v< U >, LOCATION);

    };

    struct default_constructor
    {

        struct X { X() { ; } };
        static_assert(std::is_default_constructible_v< X >, LOCATION);
        static_assert(!std::is_trivially_default_constructible_v< X >, LOCATION); // user-provided
        static_assert(std::is_trivially_copyable_v< X >, LOCATION);
        using U = V< X >;
        static_assert(std::is_default_constructible_v< U >, LOCATION);
        static_assert(!std::is_trivially_default_constructible_v< U >, LOCATION); // U is a union with at least one variant member with non-trivial default constructor
        static_assert(std::is_trivially_copyable_v< U >, LOCATION);

    };

    struct destructor
    {

        struct X { ~X() { ; } };
        static_assert(std::is_destructible_v< X >, LOCATION);
        static_assert(!std::is_trivially_destructible_v< X >, LOCATION); // user-provided
        using U = V< X >;
        static_assert(std::is_destructible_v< U >, LOCATION);
        static_assert(!std::is_trivially_destructible_v< U >, LOCATION); // U is a union and has a variant member with non-trivial destructor

    };

    struct copy_constructor
    {

        struct X { X(X const &) { ; } };
        static_assert(!std::is_default_constructible_v< X >, LOCATION); // If no user-defined constructors of any kind are provided for a class type (struct, class, or union), the compiler will always declare a default constructor as an inline public member of its class.
        static_assert(!std::is_trivially_default_constructible_v< X >, LOCATION); // not default constructible
        static_assert(std::is_destructible_v< X >, LOCATION);
        static_assert(std::is_trivially_destructible_v< X >, LOCATION);
        static_assert(std::is_copy_constructible_v< X >, LOCATION);
        static_assert(std::is_move_constructible_v< X >, LOCATION); // X const & can bind X && => move constructible
        static_assert(!std::is_trivially_copy_constructible_v< X >, LOCATION); // user-provided
        static_assert(!std::is_trivially_move_constructible_v< X >, LOCATION); // just match prev?
        static_assert(std::is_copy_assignable_v< X >, LOCATION);
        static_assert(std::is_move_assignable_v< X >, LOCATION);
        static_assert(std::is_trivially_copy_assignable_v< X >, LOCATION);
        static_assert(std::is_trivially_move_assignable_v< X >, LOCATION);
        using U = V< X >;
        static_assert(!std::is_default_constructible_v< U >, LOCATION); // hard error in non-trivial default constructor of class "destructor"
        static_assert(!std::is_trivially_default_constructible_v< U >, LOCATION); // the same error
        static_assert(std::is_destructible_v< U >, LOCATION);
        static_assert(std::is_trivially_destructible_v< U >, LOCATION);
        static_assert(!std::is_copy_constructible_v< U >, LOCATION); // U is a union and has a variant member with non-trivial copy constructor
        static_assert(std::is_move_constructible_v< U >, LOCATION); // U const & can bind U && => move constructible
        static_assert(!std::is_trivially_copy_constructible_v< U >, LOCATION); // inherits
        static_assert(!std::is_trivially_move_constructible_v< U >, LOCATION); // inherits
        static_assert(std::is_copy_assignable_v< U >, LOCATION);
        static_assert(std::is_move_assignable_v< U >, LOCATION);
        static_assert(std::is_trivially_copy_assignable_v< U >, LOCATION);
        static_assert(std::is_trivially_move_assignable_v< U >, LOCATION);

    };

    struct move_constructor
    {

        struct X { X(X &&) { ; } };
        static_assert(!std::is_default_constructible_v< X >, LOCATION); // If no user-defined constructors of any kind are provided for a class type (struct, class, or union), the compiler will always declare a default constructor as an inline public member of its class.
        static_assert(!std::is_trivially_default_constructible_v< X >, LOCATION); // not default constructible
        static_assert(std::is_destructible_v< X >, LOCATION);
        static_assert(std::is_trivially_destructible_v< X >, LOCATION);
        static_assert(!std::is_copy_constructible_v< X >, LOCATION); // X has a user-defined move constructor or move assignment operator
        static_assert(std::is_move_constructible_v< X >, LOCATION);
        static_assert(!std::is_trivially_copy_constructible_v< X >, LOCATION); // not copy-constructible
        static_assert(!std::is_trivially_move_constructible_v< X >, LOCATION); // user-provided
        static_assert(!std::is_copy_assignable_v< X >, LOCATION); // X  has a user-defined move constructor or move assignment operator
        static_assert(!std::is_move_assignable_v< X >, LOCATION); // If there are no user-declared move constructors then the compiler will declare a move assignment operator as an inline public member of its class with the signature T& T::operator=(T&&).
        static_assert(!std::is_trivially_copy_assignable_v< X >, LOCATION); // not copy-assignable
        static_assert(!std::is_trivially_move_assignable_v< X >, LOCATION); // not move-assignable
        using U = V< X >;
        static_assert(!std::is_default_constructible_v< U >, LOCATION); // hard error in non-trivial default constructor of class "destructor"
        static_assert(!std::is_trivially_default_constructible_v< U >, LOCATION); // the same error
        static_assert(std::is_destructible_v< U >, LOCATION);
        static_assert(std::is_trivially_destructible_v< U >, LOCATION);
        static_assert(!std::is_copy_constructible_v< U >, LOCATION); // U has non-static data members that cannot be copied
        static_assert(std::is_move_constructible_v< U >, LOCATION);
        static_assert(!std::is_trivially_copy_constructible_v< U >, LOCATION); // inherits
        static_assert(!std::is_trivially_move_constructible_v< U >, LOCATION); // inherits
        static_assert(!std::is_copy_assignable_v< U >, LOCATION); // inherits
        static_assert(!std::is_move_assignable_v< U >, LOCATION); // inherits
        static_assert(!std::is_trivially_copy_assignable_v< U >, LOCATION); // not copy-assignable
        static_assert(!std::is_trivially_move_assignable_v< U >, LOCATION); // not move-assignable

    };

    struct copy_assignment
    {

        struct X { X & operator = (X const &) { return *this; } };
        static_assert(std::is_default_constructible_v< X >, LOCATION);
        static_assert(std::is_trivially_default_constructible_v< X >, LOCATION);
        static_assert(std::is_destructible_v< X >, LOCATION);
        static_assert(std::is_trivially_destructible_v< X >, LOCATION);
        static_assert(std::is_copy_constructible_v< X >, LOCATION);
        static_assert(std::is_move_constructible_v< X >, LOCATION);
        static_assert(std::is_trivially_copy_constructible_v< X >, LOCATION);
        static_assert(std::is_trivially_move_constructible_v< X >, LOCATION);
        static_assert(std::is_copy_assignable_v< X >, LOCATION);
        static_assert(std::is_move_assignable_v< X >, LOCATION);
        static_assert(!std::is_trivially_copy_assignable_v< X >, LOCATION); // user-provided
        static_assert(!std::is_trivially_move_assignable_v< X >, LOCATION); // just match prev?
        using U = V< X >;
        static_assert(std::is_default_constructible_v< U >, LOCATION);
        static_assert(std::is_trivially_default_constructible_v< U >, LOCATION);
        static_assert(std::is_destructible_v< U >, LOCATION);
        static_assert(std::is_trivially_destructible_v< U >, LOCATION);
        static_assert(std::is_copy_constructible_v< U >, LOCATION);
        static_assert(std::is_move_constructible_v< U >, LOCATION);
        static_assert(std::is_trivially_copy_constructible_v< U >, LOCATION);
        static_assert(std::is_trivially_move_constructible_v< U >, LOCATION);
        static_assert(!std::is_copy_assignable_v< U >, LOCATION); // U is a union-like class, and has a variant member whose corresponding assignment operator is non-trivial
        static_assert(!std::is_move_assignable_v< U >, LOCATION); // non-trivially-move-assignable data member in union?
        static_assert(!std::is_trivially_copy_assignable_v< U >, LOCATION); // inherits
        static_assert(!std::is_trivially_move_assignable_v< U >, LOCATION); // inherits

    };

    struct move_assignment
    {

        struct X { X & operator = (X &&) { return *this; } };
        static_assert(std::is_default_constructible_v< X >, LOCATION);
        static_assert(std::is_trivially_default_constructible_v< X >, LOCATION);
        static_assert(std::is_destructible_v< X >, LOCATION);
        static_assert(std::is_trivially_destructible_v< X >, LOCATION);
        static_assert(!std::is_copy_constructible_v< X >, LOCATION); // X has a user-defined move constructor or move assignment operator
        static_assert(!std::is_move_constructible_v< X >, LOCATION); // If there are no user-declared move assignment operators then the compiler will declare a move constructor as a non-explicit inline public member of its class with the signature T::T(T&&).
        static_assert(!std::is_trivially_copy_constructible_v< X >, LOCATION); // not copy-constructible
        static_assert(!std::is_trivially_move_constructible_v< X >, LOCATION); // not move-constructible
        static_assert(!std::is_copy_assignable_v< X >, LOCATION); // X  has a user-defined move constructor or move assignment operator
        static_assert(std::is_move_assignable_v< X >, LOCATION);
        static_assert(!std::is_trivially_copy_assignable_v< X >, LOCATION); // not copy-assignable
        static_assert(!std::is_trivially_move_assignable_v< X >, LOCATION); // not move-assignable
        using U = V< X >;
        static_assert(std::is_default_constructible_v< U >, LOCATION); // hard error in non-trivial default constructor of class "destructor"
        static_assert(std::is_trivially_default_constructible_v< U >, LOCATION); // the same error
        static_assert(std::is_destructible_v< U >, LOCATION);
        static_assert(std::is_trivially_destructible_v< U >, LOCATION);
        static_assert(!std::is_copy_constructible_v< U >, LOCATION); // U has non-static data members that cannot be copied
        static_assert(std::is_move_constructible_v< U >, LOCATION); // ????????????! BUG: clang or libc++
        static_assert(!std::is_trivially_copy_constructible_v< U >, LOCATION); // inherits
        static_assert(std::is_trivially_move_constructible_v< U >, LOCATION); // ????????! BUG: clang or libc++
        static_assert(!std::is_copy_assignable_v< U >, LOCATION); // inherits
        static_assert(!std::is_move_assignable_v< U >, LOCATION); // ! still valid? cite: (until C++14) T has a non-static data member or a direct or virtual base without a move assignment operator that is not trivially copyable.
        static_assert(!std::is_trivially_copy_assignable_v< U >, LOCATION); // not copy-assignable
        static_assert(!std::is_trivially_move_assignable_v< U >, LOCATION); // not move-assignable

    };

    constexpr
    static
    bool
    run() noexcept // just for implicit instantiation
    {
        return true;
    }

};

using ::versatile::type_qualifier;

template< type_qualifier type_qual, typename type >
CONSTEXPRF
decltype(auto)
forward_as(type && _value) noexcept
{
    using ::versatile::add_qualifier_t;
    using decay_type = std::remove_cv_t< std::remove_reference_t< type > >;
    return static_cast< add_qualifier_t< type_qual, decay_type > >(_value);
}

template< typename type, typename ...types >
CONSTEXPRF
bool
check_active(::versatile::versatile< types... > const & v) noexcept
{
    return v.template active< type >();
}

template< typename type, typename ...types >
CONSTEXPRF
type
get(::versatile::versatile< types... > const & v) noexcept
{
    return static_cast< type >(static_cast< type & >(v));
}

template< typename type, typename ...types >
CONSTEXPRF
type
get(::versatile::versatile< types... > & v) noexcept
{
    return static_cast< type >(static_cast< type & >(v));
}

template< template< typename ... > class wrapper = ::versatile::identity,
          template< typename ... > class variant = ::versatile::versatile >
struct check_trivial
{

    template< typename ...types >
    using V = variant< typename wrapper< types >::type... >;

    constexpr
    static
    bool
    conversion_assignment_operator() noexcept
    {
        {

        }
        return true;
    }

    static_assert(conversion_assignment_operator(), LOCATION);

    constexpr
    static
    bool
    trivially_copy_constructible() noexcept
    {
        { // really all operations are trivial operations
            struct A {};
            using U = V< A >;
            static_assert(std::is_constructible_v< A, U >, LOCATION);
            static_assert(!std::is_convertible< U, A >::value, LOCATION);
            static_assert(!std::is_assignable_v< A &, U const >, LOCATION);
            static_assert(!std::is_assignable_v< A &, U >, LOCATION);
        }
        {
            using U = variant< int, double >;
            static_assert(std::is_trivial_v< U >, LOCATION);
            constexpr U i = 1;
            static_assert(get< int const & >(i) == 1, LOCATION);
            constexpr U d = -1.1;
            static_assert(!(get< double const & >(d) < -1.1), LOCATION);
            static_assert(!(-1.1 < get< double const & >(d)), LOCATION);
        }
        {
            struct A { int i; };
            struct B { double d; };
            using U = V< A, B >;
            static_assert(std::is_trivial_v< U >, LOCATION);
            constexpr U i = A{1};
            static_assert(get< A const & >(i).i == 1, LOCATION);
            constexpr U d = B{1.1};
            static_assert(!(get< B const & >(d).d < 1.1), LOCATION);
            static_assert(!(1.1 < get< B const & >(d).d), LOCATION);
        }
        return true;
    }

    static_assert(trivially_copy_constructible(), LOCATION);

    constexpr
    static
    bool
    trivially_copy_assignable() noexcept
    {
        { // really all operations are trivial operations
            struct A {};
            struct B {};
            using U = V< A, B >;
            static_assert(std::is_assignable_v< U &, A >, LOCATION);
            static_assert(std::is_assignable_v< U &, B >, LOCATION);
            static_assert(!std::is_trivially_assignable_v< U, A >, LOCATION);
            static_assert(!std::is_trivially_assignable_v< U, B >, LOCATION);
        }
        {
            using U = variant< int, char, double >;
            static_assert(std::is_trivial_v< U >);
            U v{1};
            if (!(check_active< int >(v))) return false;
            if (!(get< int & >(v) == 1)) return false;
            v = 'c';
            if (!(check_active< char & >(v))) return false;
            if (!(get< char & >(v) == 'c')) return false;
            v = -1.1;
            if (!(check_active< double & >(v))) return false;
            if ((get< double & >(v) < -1.1) || (-1.1 < get< double & >(v))) return false;
            v = 2;
            if (!(check_active< int & >(v))) return false;
            if (!(get< int & >(v) == 2)) return false;
        }
        {
            struct A { int i = 1; };
            struct B { int i = 2; };
            struct C { int i = 3; };
            using U = V< A, B, C >;
            static_assert(!std::is_trivially_default_constructible_v< U >);
            static_assert(std::is_trivially_copyable_v< U >);
            U v{A{}};
            if (!(check_active< A >(v))) return false;
            if (!(get< A & >(v).i == 1)) return false;
            v = B{};
            if (!(check_active< B & >(v))) return false;
            if (!(get< B & >(v).i == 2)) return false;
            v = C{};
            if (!(check_active< C & >(v))) return false;
            if (!(get< C & >(v).i == 3)) return false;
            v = A{};
            if (!(check_active< A & >(v))) return false;
            if (!(get< A & >(v).i == 1)) return false;
        }
        {
            enum class E { A = 1 };
            static_assert(std::is_trivially_default_constructible_v< E >, LOCATION);
            constexpr E e{};
            static_assert(e != E::A, LOCATION); // not in domain space => it is better to prohibit using of enums
            static_assert(static_cast< std::underlying_type_t< E > >(e) == 0, LOCATION);
        }
        {
            enum class E { A, B };
            enum class F { C, D };
            using U = variant< E, F >;
            U v = E::B;
            if (!(get< E >(v) == E::B)) return false;
            v = F::C;
            if (!(get< F >(v) == F::C)) return false;
        }
        return true;
    }

    static_assert(trivially_copy_assignable(), LOCATION);

    constexpr
    static
    bool
    run() noexcept
    {
        return true;
    }

};

enum class state
{
    default_constructed,
    copy_constructed,
    move_constructed,
    move_assigned,
    copy_assigned,
    moved_from,
};

struct oracle // akrzemi1's optional test
{

    CONSTEXPRF
    oracle() noexcept
        : s(state::default_constructed)
    { ; }

    CONSTEXPRF
    oracle(oracle const &) noexcept
        : s(state::copy_constructed)
    { ; }

    CONSTEXPRF
    oracle(oracle && o) noexcept
        : s(state::move_constructed)
    {
        o.s = state::moved_from;
    }

    CONSTEXPRF
    oracle & operator = (oracle const &) noexcept
    {
        s = state::copy_assigned;
        return *this;
    }

    CONSTEXPRF
    oracle & operator = (oracle && o) noexcept
    {
        s = state::move_assigned;
        o.s = state::moved_from;
        return *this;
    }

    friend
    CONSTEXPRF
    bool
    operator == (oracle const & o, state const _s) noexcept
    {
        return (o.s == _s);
    }

    friend
    CONSTEXPRF
    bool
    operator == (state const _s, oracle const & o) noexcept
    {
        return operator == (o, _s);
    }

    friend
    CONSTEXPRF
    bool
    operator != (oracle const & o, state const _s) noexcept
    {
        return !operator == (o, _s);
    }

    friend
    CONSTEXPRF
    bool
    operator != (state const _s, oracle const & o) noexcept
    {
        return !operator == (o, _s);
    }

private :

    state s;

};

template< typename from, typename to >
struct is_explicitly_convertible // akrzemi1
        : std::bool_constant< (std::is_constructible_v< to, from > && !std::is_convertible< from, to >::value) >
{

};

template< template< typename ... > class wrapper = ::versatile::identity,
          template< typename ... > class variant = ::versatile::versatile >
class check_literal
{

    template< typename ...types >
    using V = variant< typename wrapper< types >::type... >;

    using O = oracle;

    CONSTEXPRF
    static
    bool
    default_constructor() noexcept
    {
        {
            using U = V< O >;
            CONSTEXPR U v{};
            ASSERT (check_active< O >(v));
            ASSERT (get< O const & >(v) == state::default_constructed);
            U w;
            CHECK (check_active< O >(w));
            CHECK (get< O const & >(w) == state::default_constructed);
        }
        {
            struct A { CONSTEXPRF A(int i = 1) : j(i) { ; } int j; };
            using U = V< A >;
            static_assert(std::is_default_constructible_v< A >, LOCATION);
            CONSTEXPR U v{};
            ASSERT (check_active< A >(v));
            ASSERT (get< A const & >(v).j == 1);
        }
        {
            struct A {};
            struct B {};
            static_assert(std::is_default_constructible_v< A >, LOCATION);
            static_assert(std::is_default_constructible_v< B >, LOCATION);
            using U = V< A, B >;
            static_assert(std::is_default_constructible_v< U >, LOCATION);
            CONSTEXPR U v{};
            ASSERT (check_active< A >(v));
        }
        {
            struct A { CONSTEXPRF A(int) { ; } };
            struct B {};
            static_assert(!std::is_default_constructible_v< A >, LOCATION);
            static_assert(std::is_default_constructible_v< B >, LOCATION);
            using U = V< A, B >;
            static_assert(std::is_default_constructible_v< U >, LOCATION);
            CONSTEXPR U v{};
            ASSERT (check_active< B >(v));
        }
        {
            struct A {};
            struct B { CONSTEXPRF B(int) { ; } };
            static_assert(std::is_default_constructible_v< A >, LOCATION);
            static_assert(!std::is_default_constructible_v< B >, LOCATION);
            using U = V< A, B >;
            static_assert(std::is_default_constructible_v< U >, LOCATION);
            CONSTEXPR U v{};
            ASSERT (check_active< A >(v));
        }
        return true;
    }

    CONSTEXPRF
    static
    bool
    conversion_constructor_and_conversion_operator() noexcept
    {
        {
            struct A {};
            using U = V< A >;
            static_assert(is_explicitly_convertible< U, A >::value, LOCATION);
            static_assert(is_explicitly_convertible< U, A const >::value, LOCATION);
            static_assert(!std::is_assignable_v< A &, U const >, LOCATION);
            static_assert(!std::is_assignable_v< A &, U >, LOCATION);
        }
        { // copy conversion constructor
            using U = V< O >;
            O o;
            CHECK (o == state::default_constructed);
            U v = o;
            CHECK (get< O & >(v) == state::copy_constructed);
            CHECK (o == state::default_constructed);
        }
        { // move conversion constructor
            using U = V< O >;
            O o;
            CHECK (o == state::default_constructed);
            U v = std::move(o);
            CHECK (get< O & >(v) == state::move_constructed);
            CHECK (o == state::moved_from);
        }
        { // copy conversion operator
            using U = V< O >;
            U v;
            CHECK (get< O & >(v) == state::default_constructed);
            //O o = v; // error: only direct initialization or explicit cast is allowed
            // O o(v); // OK
            // O o = get< O & >(v); // OK
            O o{v};
            CHECK (o == state::copy_constructed);
            CHECK (get< O & >(v) == state::default_constructed);
        }
        { // move conversion operator (not works)
            using U = V< O >;
            U v;
            CHECK (get< O & >(v) == state::default_constructed);
            //O o = std::move(v); // error: only direct initialization or explicit cast is allowed
            // O o(std::move(v)); // OK
            // O o = std::move(get< O & >(v)); // OK
            O o{std::move(v)};
            { // ! currently clang++ has bug #19917 for get to rvalue reference and cv-ref-qualified overloaded conversion operators (templated) (if any) causes "ambiguity" error
                CHECK (o != state::move_constructed);
                CHECK (get< O & >(v) != state::moved_from);
            }
            CHECK (o == state::copy_constructed);
            CHECK (get< O & >(v) == state::default_constructed);
        }
        return true;
    }

    CONSTEXPRF
    static
    bool
    copy_assignment_and_move_assignment_operators() noexcept
    {
        {
            struct A {};
            struct B {};
            using U = V< A, B >;
            static_assert(std::is_copy_assignable_v< U >, LOCATION);
            static_assert(std::is_move_assignable_v< U >, LOCATION);
        }
        {
            struct A { int i = 1; };
            struct B { int i = 5; };
            using U = V< A, B >;
            U v = B{2};
            CHECK (get< B & >(v).i == 2);
            U w = B{3};
            CHECK (get< B & >(w).i == 3);
            v = w;
            CHECK (get< B & >(v).i == 3);
            CHECK (get< B & >(w).i == 3);
        }
        {
            struct A { int i = 1; };
            struct B { int i = 5; };
            using U = V< A, B >;
            U v = B{2};
            CHECK (get< B & >(v).i == 2);
            U w = B{3};
            CHECK (get< B & >(w).i == 3);
            v = std::move(w);
            CHECK (get< B & >(v).i == 3);
            CHECK (get< B & >(w).i == 3);
        }
        return true;
    }

    CONSTEXPRF
    static
    bool
    conversion_assignment_operator() noexcept // http://stackoverflow.com/questions/33936295/
    {
        {
            struct A {};
            struct B {};
            using U = V< A, B >;
            static_assert(std::is_assignable_v< U, A >, LOCATION);
            static_assert(std::is_assignable_v< U, B >, LOCATION);
        }
        { // altering active member
            struct A { int i = 1; };
            using U = V< A >;
            U v;
            CHECK (check_active< A >(v));
            CHECK (get< A & >(v).i == 1);
            A a{2};
            v = a;
            CHECK (a.i == 2);
            CHECK (get< A & >(v).i == 2);
            a.i = 3;
            v = std::move(a);
            CHECK (a.i == 3);
            CHECK (get< A & >(v).i == 3);
        }
        // changing active member is only available for trivially copyable types
        return true;
    }

    CONSTEXPRF
    static
    bool
    copy_constructor_and_move_constructor() noexcept
    {
        {
            struct A { int i = 1; };
            struct B { int i = 2; };
            struct C { int i = 3; };
            using U = V< A, B, C >;
            static_assert(std::is_copy_constructible_v< U >, LOCATION);
            static_assert(std::is_move_constructible_v< U >, LOCATION);
        }
        {
            struct A { int i = 1; };
            struct B { char c = 2; };
            using VAOB = V< A, O, B >;
            ASSERT (!__is_trivially_constructible(O, O));
            ASSERT (!__is_trivially_constructible(VAOB, VAOB));
            ASSERT (__is_constructible(VAOB, VAOB)); // wrong
            // clang allow me to write `VAOB v{O{}}; VAOB w = std::move(v);`, but it is invalid code
            CBRA
                    VAOB v{A{}};
            CHECK (check_active< A >(v));
            VAOB w = std::move(v);
            CHECK (check_active< A >(w));
            CKET
        }
        return true;
    }

    CONSTEXPRF
    static
    bool
    emplace_constructor() noexcept
    {
        struct A
        {
            CONSTEXPRF A() : j(1) { ; }
            CONSTEXPRF A(int i) : j(i) { ; }
            CONSTEXPRF A(int a, int b) : j(a + b) { ; }
            int j;
        };
        using U = V< A >;
        U v;
        CHECK (get< A & >(v).j == 1);
        v.emplace(3);
        CHECK (get< A & >(v).j == 3);
        v.emplace(4, 3);
        CHECK (get< A & >(v).j == 7);
        return true;
    }

public :

    CONSTEXPRF
    static
    bool
    run() noexcept
    {
        ASSERT (default_constructor());
        ASSERT (copy_constructor_and_move_constructor());
        ASSERT (copy_assignment_and_move_assignment_operators());
        ASSERT (conversion_constructor_and_conversion_operator());
        ASSERT (conversion_assignment_operator());
        ASSERT (emplace_constructor());
        return true;
    }

};

template< typename variadic >
struct variadic_size;

template< template< typename ...types > class variadic, typename ...types >
struct variadic_size< variadic< types... > >
        : ::versatile::index< sizeof...(types) >
{

};

template< typename F, std::size_t ...indices >
struct enumerator;

template< typename F >
struct enumerator< F >
{

    CONSTEXPRF
    enumerator(F & _f) noexcept
        : f(std::forward< F >(_f))
    { ; }

    template< std::size_t ...I >
    CONSTEXPRF
    bool
    operator () () const noexcept
    {
        return f(std::index_sequence< I... >{});
    }

private :

    F f;

};

template< typename F, std::size_t first, std::size_t ...rest >
struct enumerator< F, first, rest... >
        : enumerator< F, rest... >
{

    using base = enumerator< F, rest... >;

    CONSTEXPRF
    enumerator(F & f) noexcept
        : base(f)
    { ; }

    template< std::size_t ...I >
    CONSTEXPRF
    bool
    operator () () const noexcept
    {
        return enumerator::template operator () < I... >(std::make_index_sequence< first >{}); // ltr
    }

private :

    template< std::size_t ...I, std::size_t ...J >
    CONSTEXPRF
    bool
    operator () (std::index_sequence< J... >) const noexcept
    {
        return (base::template operator () < I..., J >() && ...); // rtl, `< J, I... >` - ltr
    }

};

template< std::size_t ...indices, typename F >
CONSTEXPRF
enumerator< F, indices... >
make_enumerator(F && f)
{
    ASSERT (0 < sizeof...(indices));
    ASSERT (((0 < indices) && ...));
    return f;
}

template< std::size_t M >
struct pair
{

    std::array< type_qualifier, (1 + M) > qual_ids;
    std::array< std::size_t, (1 + M) > type_ids;

    template< std::size_t ...I >
    CONSTEXPRF
    bool
    ids_equal(pair const & _rhs, std::index_sequence< I... >) const noexcept
    {
        return ((qual_ids[I] == _rhs.qual_ids[I]) && ...) && ((type_ids[I] == _rhs.type_ids[I]) && ...);
    }

    CONSTEXPRF
    bool
    operator == (pair const & _rhs) const noexcept
    {
        return ids_equal(_rhs, std::make_index_sequence< 1 + M >{});
    }

    CONSTEXPRF
    std::size_t
    size() const noexcept
    {
        return (1 + M);
    }

};

using ::versatile::type_qualifier_of;

template< std::size_t M, type_qualifier type_qual = type_qualifier::rref >
struct multivisitor
{

    using result_type = pair< M >;

    result_type & result_;

    static constexpr type_qualifier type_qual_ = type_qual;

    CONSTEXPRF
    std::size_t
    which() const noexcept
    {
        return 0;
    }

    template< typename ...types >
    CONSTEXPRF
    decltype(auto)
    operator () (types &&... _values) & noexcept
    {
        //ASSERT (M == sizeof...(types));
        //ASSERT (!(is_visitable< types >{} || ...));
        result_ = {{{type_qualifier_of< multivisitor & >, type_qualifier_of< types && >...}}, {{M, _values...}}};
        return forward_as< type_qual >(result_);
    }

    template< typename ...types >
    CONSTEXPRF
    decltype(auto)
    operator () (types &&... _values) const & noexcept
    {
        result_ = {{{type_qualifier_of< multivisitor const & >, type_qualifier_of< types && >...}}, {{M, _values...}}};
        return forward_as< type_qual >(result_);
    }

    template< typename ...types >
    CONSTEXPRF
    decltype(auto)
    operator () (types &&... _values) && noexcept
    {
        result_ = {{{type_qualifier_of< multivisitor && >, type_qualifier_of< types && >...}}, {{M, _values...}}};
        return forward_as< type_qual >(result_);
    }

    template< typename ...types >
    CONSTEXPRF
    decltype(auto)
    operator () (types &&... _values) const && noexcept
    {
        result_ = {{{type_qualifier_of< multivisitor const && >, type_qualifier_of< types && >...}}, {{M, _values...}}};
        return forward_as< type_qual >(result_);
    }

};

template< std::size_t M, type_qualifier type_qual >
struct variadic_size< multivisitor< M, type_qual > >
        : ::versatile::index< M >
{

};

static CONSTEXPR std::size_t qualifier_id_begin = static_cast< std::size_t >(type_qualifier_of< void * & >);
static CONSTEXPR std::size_t qualifier_id_end = static_cast< std::size_t >(type_qualifier_of< void * volatile & >);

template< typename ...types >
struct fusor
{

    std::tuple< types *... > const & stuff_;

    template< std::size_t ...Q >
    CONSTEXPRF
    bool
    operator () (std::index_sequence< Q... >) const noexcept
    {
        return call< Q... >(std::index_sequence_for< types... >{});
    }

private :

    template< std::size_t ...Q, std::size_t ...K >
    CONSTEXPRF
    bool
    call(std::index_sequence< K... >) const noexcept
    {
        using ::versatile::at_index_t;
        using ::versatile::multivisit;
        decltype(auto) lhs = multivisit(forward_as< static_cast< type_qualifier >(qualifier_id_begin + Q) >(*std::get< K >(stuff_))...);
        if (sizeof...(types) != lhs.size()) {
            return false;
        }
        if (type_qualifier_of< decltype(lhs) > != std::get< 0 >(stuff_)->type_qual_) { // TODO:
            return false;
        }
        using tuple = std::tuple< types... >;
        pair< (sizeof...(types) - 1) > const rhs = {{{static_cast< type_qualifier >(qualifier_id_begin + Q)...}}, {{(variadic_size< std::tuple_element_t< K, tuple > >{} - 1 - std::get< K >(stuff_)->which())...}}};
        if (lhs == rhs) {
            return false;
        }
        return true;
    }

};

template< typename ...types >
CONSTEXPRF
fusor< types... >
make_fusor(std::tuple< types *... > const & _stuff) noexcept
{
    static_assert(((type_qualifier_of< types > == type_qualifier::value) && ...));
    return {_stuff};
}

// variant - variant
// T - type generator
// M - multivisitor arity, N - number of alternative (bounded) types
template< template< std::size_t I > class T,
          template< typename ...types > class variant,
          std::size_t M, std::size_t N = M >
class test_perferct_forwarding
{

    static CONSTEXPR std::size_t ref_count = (qualifier_id_end - qualifier_id_begin); // test only reference types

    template< typename >
    struct variants;

    template< std::size_t ...J >
    struct variants< std::index_sequence< J... > >
    {

        using variant_type = variant< T< J >... >;
        static_assert(N == sizeof...(J));

        variant_type variants_[N] = {T< J >{}...};

    };

    template< type_qualifier type_qual, std::size_t ...I >
    CONSTEXPRF
    static
    bool
    run(std::index_sequence< I... >) noexcept
    {
        static_assert(sizeof...(I) == M);
        std::size_t indices[M] = {};
        for (std::size_t & n : indices) {
            if (n != 0) return false;
        }
        using MV = multivisitor< M, type_qual >;
        typename MV::result_type result_{};
        MV mv{result_};
        variants< std::make_index_sequence< N > > variants_; // non-const
        auto permutation_ = std::make_tuple(&mv, &variants_.variants_[indices[I]]...);
        auto const fusor_ = make_fusor(permutation_);
        auto const enumerator_ = make_enumerator< ref_count, (I, ref_count)... >(fusor_);
        for (;;) {
            { // constexpr version of `permutation_ = std::make_tuple(&mv, &variants_.variants_[indices[I]]...);`
                std::get< 0 >(permutation_) = &mv;
                ((std::get< 1 + I >(permutation_) = &variants_.variants_[indices[I]]), ...);
            }
            if (!enumerator_()) {
                return false;
            }
            std::size_t m = 0;
            for (;;) {
                std::size_t & n = indices[m];
                ++n;
                if (n != N) {
                    break;
                }
                n = 0;
                if (++m == M) {
                    return true;
                }
            }
        }
        return true;
    }

    template< std::size_t ...Q >
    CONSTEXPRF
    static
    bool
    run(std::index_sequence< Q... >) noexcept
    {
        return (run< static_cast< type_qualifier >(qualifier_id_begin + Q) >(std::make_index_sequence< M >{}) && ...);
    }

public :

    CONSTEXPRF
    static
    bool
    run() noexcept
    {
        return run(std::make_index_sequence< ref_count >{});
    }

};

using ::versatile::unwrap_type_t;
using ::versatile::is_visitable;

// value_or
template< typename lhs, typename rhs,
          typename result_type = unwrap_type_t< rhs > >
constexpr
std::enable_if_t< (is_visitable< unwrap_type_t< lhs > >{} && !is_visitable< result_type >{}), result_type >
operator || (lhs && _lhs, rhs && _rhs) noexcept
{
    if (_lhs.template active< result_type >()) {
        return static_cast< result_type >(std::forward< lhs >(_lhs));
    } else {
        return std::forward< rhs >(_rhs);
    }
}

template< typename lhs, typename rhs,
          typename result_type = unwrap_type_t< lhs > >
constexpr
std::enable_if_t< (!is_visitable< result_type >{} && is_visitable< unwrap_type_t< rhs > >{}), result_type >
operator || (lhs && _lhs, rhs && _rhs) noexcept
{
    return (std::forward< rhs >(_rhs) || std::forward< lhs >(_lhs));
}

template< typename lhs, typename rhs >
std::enable_if_t< (is_visitable< unwrap_type_t< lhs > >{} && is_visitable< unwrap_type_t< rhs > >{}) >
operator || (lhs && _lhs, rhs && _rhs) = delete;

} // namespace test_variant

#include <boost/variant.hpp>

template< typename ...types >
struct boost_variant_c
{

    using variant = boost::variant< types... >;

    boost_variant_c(boost_variant_c &) = default;
    boost_variant_c(boost_variant_c const &) = default;
    boost_variant_c(boost_variant_c &&) = default;

    constexpr
    boost_variant_c(boost_variant_c const && _rhs)
        : member_(std::move(_rhs.member_))
    { ; }

    template< typename ...arguments >
    constexpr
    boost_variant_c(arguments &&... _arguments)
        : member_(std::forward< arguments >(_arguments)...)
    { ; }

    boost_variant_c &
    operator = (boost_variant_c const &) = default;
    boost_variant_c &
    operator = (boost_variant_c &) = default;
    boost_variant_c &
    operator = (boost_variant_c &&) = default;

    constexpr
    boost_variant_c &
    operator = (boost_variant_c const && _rhs)
    {
        member_ = std::move(_rhs.member_);
        return *this;
    }

    template< typename argument >
    constexpr
    boost_variant_c &
    operator = (argument && _argument)
    {
        member_ = std::forward< argument >(_argument);
        return *this;
    }

    constexpr
    std::size_t
    which() const
    {
        return (sizeof...(types) - static_cast< std::size_t >(member_.which()));
    }

    template< typename type >
    using index_at_t = ::versatile::index_at_t< ::versatile::unwrap_type_t< type >, ::versatile::unwrap_type_t< types >..., void >;

    template< typename type >
    constexpr
    bool
    active() const noexcept
    {
        return (index_at_t< type >::value == which());
    }

    template< typename type >
    explicit
    constexpr
    operator type const & () const
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return boost::get< type const & >(member_);
    }

    template< typename type >
    explicit
    constexpr
    operator type & ()
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return boost::get< type & >(member_);
    }

private :

    boost::variant< types... > member_;

};

template< typename type, typename ...types >
CONSTEXPRF
bool
check_active(::boost_variant_c< types... > const & v) noexcept
{
    return v.template active< type >();
}

template< typename type, typename ...types >
CONSTEXPRF
type
get(boost_variant_c< types... > const & v) noexcept
{
    return static_cast< type >(static_cast< type & >(v));
}

template< typename type, typename ...types >
CONSTEXPRF
type
get(boost_variant_c< types... > & v) noexcept
{
    return static_cast< type >(static_cast< type & >(v));
}

template< typename ...types >
struct boost_variant_i
        : boost::variant< types... >
{

    using base = boost::variant< types... >;

    using base::base;
    using base::operator =;

    boost_variant_i(boost_variant_i &) = default;
    boost_variant_i(boost_variant_i const &) = default;
    boost_variant_i(boost_variant_i &&) = default;

    constexpr
    boost_variant_i(boost_variant_i const && _rhs)
        : base(std::move(_rhs.member_))
    { ; }

    template< typename ...arguments >
    constexpr
    boost_variant_i(arguments &&... _arguments)
        : base(std::forward< arguments >(_arguments)...)
    { ; }

    boost_variant_i &
    operator = (boost_variant_i const &) = default;
    boost_variant_i &
    operator = (boost_variant_i &) = default;
    boost_variant_i &
    operator = (boost_variant_i &&) = default;

    constexpr
    boost_variant_i &
    operator = (boost_variant_i const && _rhs)
    {
        base::operator = (std::move(_rhs.member_));
        return *this;
    }

    template< typename argument >
    constexpr
    boost_variant_i &
    operator = (argument && _argument)
    {
        base::operator = (std::forward< argument >(_argument));
        return *this;
    }

    constexpr
    std::size_t
    which() const
    {
        return (sizeof...(types) - static_cast< std::size_t >(base::which()));
    }

    template< typename type >
    using index_at_t = ::versatile::index_at_t< ::versatile::unwrap_type_t< type >, ::versatile::unwrap_type_t< types >..., void >;

    template< typename type >
    constexpr
    bool
    active() const noexcept
    {
        return (index_at_t< type >::value == which());
    }

    template< typename type >
    explicit
    constexpr
    operator type const & () const
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return boost::get< type const & >(static_cast< boost_variant_i::base const & >(*this));
    }

    template< typename type >
    explicit
    constexpr
    operator type & ()
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return boost::get< type & >(static_cast< boost_variant_i::base & >(*this));
    }

};

template< typename type, typename ...types >
CONSTEXPRF
bool
check_active(::boost_variant_i< types... > const & v) noexcept
{
    return v.template active< type >();
}

template< typename type, typename ...types >
CONSTEXPRF
type
get(::boost_variant_i< types... > const & v) noexcept
{
    return static_cast< type >(static_cast< type & >(v));
}

template< typename type, typename ...types >
CONSTEXPRF
type
get(::boost_variant_i< types... > & v) noexcept
{
    return static_cast< type >(static_cast< type & >(v));
}

namespace versatile
{

template< typename type >
struct unwrap_type< boost::recursive_wrapper< type > >
        : unwrap_type< type >
{

};

template< typename first, typename ...rest >
struct is_visitable< ::boost_variant_i< first, rest... > >
        : std::true_type
{

};

template< typename first, typename ...rest >
struct is_visitable< ::boost_variant_c< first, rest... > >
        : std::true_type
{

};

template< typename type >
struct unwrap_type< std::reference_wrapper< type > >
        : unwrap_type< type >
{

    static_assert(!std::is_const< type >{});

};

} // namespace versatile

#include <cstdlib>

#ifndef COLS
#define COLS 5
#endif

#ifndef ROWS
#define ROWS COLS
#endif

int
main()
{
    { // versatile
        using ::versatile::versatile;
        {
            using ::versatile::identity;
            using ::test_variant::aggregate;
            {
                using ::test_variant::check_invariants;
                ASSERT (check_invariants< identity,  versatile >::run());
                ASSERT (check_invariants< aggregate, versatile >::run());
            }
            {
                using ::test_variant::check_trivial;
                ASSERT (check_trivial< identity,  versatile >::run());
                ASSERT (check_trivial< aggregate, versatile >::run());
            }
            {
                using ::test_variant::check_literal;
                ASSERT (check_literal< identity,  versatile >::run());
                ASSERT (check_literal< aggregate, versatile >::run());
            }
#if 1
            {
                using ::test_variant::test_perferct_forwarding;
                {
                    using ::test_variant::literal_type;
                    ASSERT (test_perferct_forwarding< literal_type, versatile, 1, 1 >::run());
                    ASSERT (test_perferct_forwarding< literal_type, versatile, 2, 3 >::run());
                    ASSERT (test_perferct_forwarding< literal_type, versatile, 3, 2 >::run());
                }
                {
                    using ::test_variant::common_type;
                    assert ((test_perferct_forwarding< common_type, versatile, 1, 1 >::run()));
                    assert ((test_perferct_forwarding< common_type, versatile, 2, 3 >::run()));
                    assert ((test_perferct_forwarding< common_type, versatile, 3, 2 >::run()));
                }
            }
#endif
        }
    }
    { // variant

    }
    { // boost::variant
        struct L {};
        static_assert(std::is_literal_type< L >{}, LOCATION);
        static_assert(!std::is_literal_type< ::boost::variant< L > >{}, LOCATION); // ...while this is true:
#ifdef VERSATILE_RUNTIME
        using ::versatile::identity;
        using ::test_variant::aggregate;
        {
            using ::test_variant::check_literal;
            {
                ASSERT (check_literal< identity,  ::boost_variant_i >::run());
                ASSERT (check_literal< aggregate, ::boost_variant_i >::run());
            }
            {
                ASSERT (check_literal< identity,  ::boost_variant_c >::run());
                ASSERT (check_literal< aggregate, ::boost_variant_c >::run());
            }
        }
#endif
#if 0
        {
            using ::test_variant::test_perferct_forwarding;
            {
                using ::test_variant::literal_type;
                assert ((test_perferct_forwarding< literal_type, ::boost_variant_i, 2, 2 >::run()));
                assert ((test_perferct_forwarding< literal_type, ::boost_variant_c, 2, 2 >::run()));
            }
            {
                using ::test_variant::common_type;
                assert ((test_perferct_forwarding< common_type, ::boost_variant_i, 2, 2 >::run()));
                assert ((test_perferct_forwarding< common_type, ::boost_variant_c, 2, 2 >::run()));
            }
        }
#endif
    }
    { // eggs::variant

    }
    return EXIT_SUCCESS;
}

#else

#include <boost/variant.hpp>


namespace versatile
{

namespace test_visitation
{

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"

struct visitor
{

    template< std::size_t ...I >
    constexpr
    std::array< std::size_t, sizeof...(I) >
    operator () (T< I >...) const noexcept
    {
        return {I...};
    }

};

template< std::size_t ...M, std::size_t ...N >
constexpr
bool
invoke(std::index_sequence< M... >, std::index_sequence< N... >) noexcept
{
    return (std::array< std::size_t, sizeof...(N) >{(N % sizeof...(M))...} == multivisit(visitor{}, versatile::variant< T< M >... >{T< (N % sizeof...(M)) >{}}...));
}

#pragma clang diagnostic pop

template< std::size_t M, std::size_t N = M >
constexpr
bool
hard() noexcept
{
    return invoke(std::make_index_sequence< M >{}, std::make_index_sequence< N >{});
}

inline int f() { return 1; }
inline int g() { return 2; }

template< typename T >
using AW = versatile::aggregate_wrapper< T >;

template< typename T >
using RW = versatile::recursive_wrapper< T >;

struct R;
struct A {};
using V = versatile::variant< A, RW< R > >;
struct R
        : V
{
    using V::V;
    using V::operator =;
};

} // namespace test

int
main()
{
    using namespace test;
    using versatile::visit;
    using versatile::multivisit;
    using versatile::compose_visitors;
    using versatile::in_place;
    using versatile::variant;
    using versatile::versatile;
    {
        {
            using V = versatile< int >;
            V w; // equivalent to default construction
            assert(static_cast< int >(w) == int{});
            V v{2};
            assert(static_cast< int >(v) == 2);
        }
        {
            using V = versatile< std::true_type, std::false_type >;
            V t;
            assert(static_cast< std::true_type >(t) == std::true_type{});
            V f{std::false_type{}};
            assert(static_cast< std::false_type >(f) == std::false_type{});
        }
        {
            using V = variant< std::true_type, std::false_type >;
            V t;
            assert(static_cast< std::true_type >(t) == std::true_type{});
            V f{std::false_type{}};
            assert(static_cast< std::false_type >(f) == std::false_type{});
        }
#if 0
        { // trivial
            using V = versatile< int, double >;
            //static_assert(std::is_trivially_copy_constructible< V >{}); // ???
            //static_assert(std::is_trivially_move_constructible< V >{}); // ???
            static_assert(std::is_trivially_copy_assignable< V >{});
            static_assert(std::is_trivially_move_assignable< V >{});
            V v{1.0};
            assert(v.active< double >());
            V u{v};
            assert(u.active< double >());
            V w{1};
            v = w;
            assert(v.active< int >());
        }
#endif
#if 0
        { // non-standard layout (really UB http://talesofcpp.fusionfenix.com/post-21/)
            struct A { int i; };
            struct B : A { A a; B(int i, int j) : A{i}, a{j} { ; } };
            static_assert(!std::is_standard_layout< B >{});
            using V = versatile< A, B >;
            static_assert(!std::is_standard_layout< V >{});
            V v{B{1, 2}};
            assert(v.active< B >());
            assert(static_cast< B >(v).A::i == 1);
            assert(static_cast< B >(v).a.i == 2);
        }
        { // non-standard layout (really UB http://talesofcpp.fusionfenix.com/post-21/)
            struct A { int i; A(int j) : i(j) { ; } };
            struct B : A { using A::A; };
            struct C : A { using A::A; };
            struct D : B, C { D(int i, int j) : B{i}, C{j} { ; } };
            static_assert(!std::is_standard_layout< D >{});
            using V = versatile< A, B, C, D >;
            static_assert(!std::is_standard_layout< V >{});
            V v{D{1, 2}};
            assert(v.active< D >());
            assert(static_cast< D >(v).B::i == 1);
            assert(static_cast< D >(v).C::i == 2);
        }
#endif
        {
            using V = variant< int >;
            V v{2};
            assert(static_cast< int >(v) == 2);
        }
        { // value_or
            using V = variant< double, int >;
            {
                V D{1.0};
                int i = D || 2;
                assert(i == 2);
                V I{1};
                int j = I || 2;
                assert(j == 1);
            }
            {
                int i = 2 || V{1.0};
                assert(i == 2);
                int j = 2 || V{1} ;
                assert(j == 1);
            }
        }
        {
            using std::swap;
            using V = variant< int >;
            static_assert(V::width == 1, "V::width != 1");
            V v;
            static_assert(1 == variadic_index< V, int >{});
            assert(v.active< int >());
            assert(static_cast< int >(v) == int{});
            V w(222);
            assert(static_cast< int >(w) == 222);
            v = w;
            assert(static_cast< int >(v) == 222);
            assert(v == w);
            assert(!(v < w));
            assert(!(w < v));
            ++static_cast< int & >(v);
            assert(w < v);
            assert(!(v < w));
            assert(!(v == w));
            assert(static_cast< int >(w) == 222);
            assert(static_cast< int >(v) == 223);
            swap(v, w);
            assert(static_cast< int >(w) == 223);
            assert(static_cast< int >(v) == 222);
            swap(w, v);
            assert(static_cast< int >(w) == 222);
            assert(static_cast< int >(v) == 223);
            V u = w;
            assert(static_cast< int >(u) == 222);
            assert(w.active< int >());
            static_assert(variadic_index< V, int >{} == 1);
            assert(u.active< int >());
        }
        {
            using V = variant< int, float, double, long double >;
            assert((V{}.which() == variadic_index< V, int >{}));
            assert((V{0}.which() == variadic_index< V, int >{}));
            assert((V{1.0f}.which() == variadic_index< V, float >{}));
            assert((V{2.0}.which() == variadic_index< V, double >{}));
            assert((V{3.0L}.which() == variadic_index< V, long double >{}));
            V i;
            assert(i.active< int >());
            V j = 1;
            assert(j.active< int >());
            V f = 1.0f;
            assert(f.active< float >());
            V d = 2.0;
            assert(d.active< double >());
            V l = 3.0L;
            assert(l.active< long double >());
            i = l;
            assert(i.active< long double >());
            l = d;
            assert(l.active< double >());
            d = f;
            assert(d.active< float >());
            f = j;
            assert(f.active< int >());
            using std::swap;
            swap(d, j);
            assert(d.active< int >());
            assert(j.active< float >());
        }
        {
            struct A { A() = delete; };
            struct B {};
            using V =  variant< A, B >;
            V v;
            assert(v.active< B >());
            v = A{};
            assert(v.active< A >());
            V w{A{}};
            assert(w.active< A >());
        }
        { // incomplete
            struct A {};
            struct B;
            using V = variant< A, RW< B > >;
            V v;
            assert(v.active< A >());
            struct B {}; // if declared but not defined then there is compilation error in std::unique_ptr destructor
            v = B{};
            assert(v.active< B >());
            assert(v.active< RW< B > >());
        }
        { // recursive (composition)
            struct R;
            struct A {};
            using V = variant< A, RW< R > >;
            V v;
            assert(v.active< A >());
            struct R { V v; };
            v = R{};
            assert(v.active< R >());
            assert(v.active< RW< R > >());
            assert(static_cast< R & >(v).v.active< A >());
        }
        { // recursive (inheritance)
            struct R;
            struct A {};
            using V = variant< A, RW< R > >;
            V u;
            assert(u.active< A >());
            struct R : V { using V::V; using V::operator =; };
            u = R{};
            assert(u.active< R >());
            assert(u.active< RW< R > >());
            R v;
            assert(v.active< A >());
            v = R{};
            assert(v.active< A >());
            R w{R{}};
            assert(w.active< A >());
            R x{V{R{}}};
            assert(x.active< R >());
            assert(x.active< RW< R > >());
        }
        { // exact
            using V = variant< int, bool >;
            assert(V{} == V{});
            assert(V{} == int{});
            assert(int{} == V{});
            assert(V{1} == 1);
            assert(1 == V{1});
            assert(V{false} == false);
            assert(true == V{true});
        }
        { // relational
            using V = variant< int, double >;
            assert(V{1} < V{2});
            assert(V{1.0} < V{2.0});
            assert(!(V{1} < V{0}));
            assert(!(V{1.0} < V{0.0}));
            assert(1 < V{2});
            assert(V{1} < 2);
            assert(V{1.0} < 2.0);
            assert(1.0 < V{2.0});
            assert(!(V{1} < 0));
            assert(!(1 < V{0}));
            assert(!(V{1.0} < 0.0));
            assert(!(1.0 < V{0.0}));
        }
        {
            struct A
            {
                A(int j) : i(j) { ; }
                A(A && a) : i(a.i) { a.i = 0; }
                void operator = (A && a) { i = a.i; a.i = 0; }
                operator int () const { return i; }
            private :
                int i;
            };
            using V = variant< A >;
            V x{in_place, 1};
            V y{in_place, 2};
            y = std::move(x);
            assert(x == A{0});
            assert(y == A{1});
        }
        {
            struct A { A(int &, int) {} };
            struct B { B(int const &, int) {} };
            using V = variant< A, B >;
            V v{in_place, 1, 2};
            assert(v.active< B >());
            int i{1};
            V w{in_place, i, 2};
            assert(w.active< A >());
        }
        {
            variant< char const * > v;
            assert(static_cast< char const * >(v) == nullptr);
        }
        {
            struct A {};
            struct B { B(B &&) = default; B(B const &) = delete; B & operator = (B &&) = default; void operator = (B const &) = delete; };
            variant< B, A > v;
            assert(v.active< A >());
            v = B{};
            assert(v.active< B >());
        }
        {
            struct A { A(double) { ; } };
            using V = variant< A, int >;
            assert(V(in_place, int{0}).active< A >());
            assert(V{int{0}}.active< int >());
        }
        {
            struct B;
            struct A { A(B const &) { ; } };
            struct B {};
            using V = variant< A, B >;
            assert(V{}.active< B >());
            assert(V{B{}}.active< B >());
            assert((V{in_place, B{}}.active< A >()));
        }
        {
            static int counter = 0;
            struct A {};
            struct B { int i; B(int j) : i(j) { ++counter; } B() = delete; B(B const &) = delete; B(B &&) = delete; };
            struct C {};
            B b{1};
            assert(counter == 1);
            using V = variant< A, B, C >;
            V v;
            assert(v.active< A >());
            assert(counter == 1);
            v.emplace(2);
            assert(v.active< B >());
            assert(static_cast< B const & >(v).i == 2);
            assert(counter == 2);
            v.emplace(1);
            assert(v.active< B >());
            assert(static_cast< B const & >(v).i == 1);
            assert(counter == 3);
        }
        {
            struct A {};
            using V = variant< RW< A > >;
            visitor0 p_;
            visitor0 const cp_{};
            V v;
            V const cv;
            using B3 = std::array< bool, 3 >;
            assert((visit(p_, v)    == B3{{false, false, true }}));
            assert((visit(cp_, v)   == B3{{true,  false, true }}));
            assert((visit(p_, cv)   == B3{{false, true,  true }}));
            assert((visit(cp_, cv)  == B3{{true,  true,  true }}));
            assert((visit(p_, V())  == B3{{false, false, false}}));
            assert((visit(cp_, V()) == B3{{true,  false, false}}));
        }
        {
            struct A {};
            using V = variant< RW< A > >;
            visitor1 p_;
            visitor1 const cp_{};
            V v;
            V const cv{};
            using B4 = std::array< bool, 4 >;
            assert((visit(p_, v)           == B4{{false, true,  false, true }}));
            assert((visit(cp_, v)          == B4{{true,  true,  false, true }}));
            assert((visit(visitor1{}, v)   == B4{{false, false, false, true }}));
            assert((visit(p_, cv)          == B4{{false, true,  true,  true }}));
            assert((visit(cp_, cv)         == B4{{true,  true,  true,  true }}));
            assert((visit(visitor1{}, cv)  == B4{{false, false, true,  true }}));
            assert((visit(p_, V())         == B4{{false, true,  false, false}}));
            assert((visit(cp_, V())        == B4{{true,  true,  false, false}}));
            assert((visit(visitor1{}, V()) == B4{{false, false, false, false}}));
        }
        { // multivisitation with forwarding
            struct A {};
            struct B {};
            using V = variant< RW< A >, RW< B > >;
            visitor2 v_;
            visitor2 const c_{};
            assert(multivisit(v_, V{B{}}) == std::make_tuple(false, std::tie(typeid(B)), false, false));
            assert(multivisit(c_, V{B{}}) == std::make_tuple(true, std::tie(typeid(B)), false, false));
            assert(multivisit(visitor2{}, V{B{}}) == std::make_tuple(false, std::tie(typeid(B)), false, false));
            assert(multivisit(v_, V{}, V{B{}}) == std::make_tuple(false, std::tie(typeid(A)), false, false, std::tie(typeid(B)), false, false));
            assert(multivisit(c_, V{}, V{B{}}) == std::make_tuple(true, std::tie(typeid(A)), false, false, std::tie(typeid(B)), false, false));
            assert(multivisit(visitor2{}, V{}, V{B{}}) == std::make_tuple(false, std::tie(typeid(A)), false, false, std::tie(typeid(B)), false, false));
            // forwarding
            enum class op { eq, lt, gt, nge };
            assert(multivisit(v_, V{}, op::eq, V{B{}}) == std::make_tuple(false, std::tie(typeid(A)), false, false, std::tie(typeid(op)), false, false, std::tie(typeid(B)), false, false));
            assert(multivisit(c_, V{}, op::eq, V{B{}}) == std::make_tuple(true, std::tie(typeid(A)), false, false, std::tie(typeid(op)), false, false, std::tie(typeid(B)), false, false));
            assert(multivisit(visitor2{}, V{}, op::eq, V{B{}}) == std::make_tuple(false, std::tie(typeid(A)), false, false, std::tie(typeid(op)), false, false, std::tie(typeid(B)), false, false));
        }
        { // delayed visitation
            visitor2 v_;
            visitor2 const c_{};

            auto d0 = visit(v_);
            auto const d1 = visit(v_);
            auto d2 = visit(c_);
            auto const d3 = visit(c_);
            auto d4 = visit(visitor2{});
            auto const d5 = visit(visitor2{});

            struct A {};
            struct B {};
            using V = variant< RW< A >, RW< B > >;
            V v;
            V const c(B{});

            assert(d0(v) ==   std::make_tuple(false, std::tie(typeid(A)), false, true ));
            assert(d0(c) ==   std::make_tuple(false, std::tie(typeid(B)), true,  true ));
            assert(d0(V{}) == std::make_tuple(false, std::tie(typeid(A)), false, false));

            assert(d1(v) ==   std::make_tuple(true, std::tie(typeid(A)), false, true ));
            assert(d1(c) ==   std::make_tuple(true, std::tie(typeid(B)), true,  true ));
            assert(d1(V{}) == std::make_tuple(true, std::tie(typeid(A)), false, false));

            assert(d2(v) ==   std::make_tuple(true,  std::tie(typeid(A)), false, true ));
            assert(d2(c) ==   std::make_tuple(true,  std::tie(typeid(B)), true,  true ));
            assert(d2(V{}) == std::make_tuple(true,  std::tie(typeid(A)), false, false));

            assert(d3(v) ==   std::make_tuple(true,  std::tie(typeid(A)), false, true ));
            assert(d3(c) ==   std::make_tuple(true,  std::tie(typeid(B)), true,  true ));
            assert(d3(V{}) == std::make_tuple(true,  std::tie(typeid(A)), false, false));

            assert(d4(v) ==   std::make_tuple(false, std::tie(typeid(A)), false, true ));
            assert(d4(c) ==   std::make_tuple(false, std::tie(typeid(B)), true,  true ));
            assert(d4(V{}) == std::make_tuple(false, std::tie(typeid(A)), false, false));

            assert(d5(v) ==   std::make_tuple(true,  std::tie(typeid(A)), false, true ));
            assert(d5(c) ==   std::make_tuple(true,  std::tie(typeid(B)), true,  true ));
            assert(d5(V{}) == std::make_tuple(true,  std::tie(typeid(A)), false, false));
        }
        { // delayed visitation
            visitor3 v_;
            visitor3 const cv_{};

            auto d = visit(v_);
            auto const cd = visit(v_);
            auto dcv = visit(cv_);
            auto const cdcv = visit(cv_);
            auto dmv = visit(visitor3{});
            auto const cdmv = visit(visitor3{});

            struct A {};
            using V = variant< RW< A > >;
            V v;
            V const cv{};

            assert(d(v)                    == std::make_tuple(false, true,  false, true ));
            assert(cd(v)                   == std::make_tuple(true,  true,  false, true ));
            assert(visit(v_)(v)            == std::make_tuple(false, false, false, true ));

            assert(dcv(v)                  == std::make_tuple(true,  true,  false, true ));
            assert(cdcv(v)                 == std::make_tuple(true,  true,  false, true ));
            assert(visit(cv_)(v)           == std::make_tuple(true,  false, false, true ));

            assert(dmv(v)                  == std::make_tuple(false, true,  false, true ));
            assert(cdmv(v)                 == std::make_tuple(true,  true,  false, true ));
            assert(visit(visitor3{})(v)    == std::make_tuple(false, false, false, true ));

            assert(d(cv)                   == std::make_tuple(false, true,  true,  true ));
            assert(cd(cv)                  == std::make_tuple(true,  true,  true,  true ));
            assert(visit(v_)(cv)           == std::make_tuple(false, false, true,  true ));

            assert(dcv(cv)                 == std::make_tuple(true,  true,  true,  true ));
            assert(cdcv(cv)                == std::make_tuple(true,  true,  true,  true ));
            assert(visit(cv_)(cv)          == std::make_tuple(true,  false, true,  true ));

            assert(dmv(cv)                 == std::make_tuple(false, true,  true,  true ));
            assert(cdmv(cv)                == std::make_tuple(true,  true,  true,  true ));
            assert(visit(visitor3{})(cv)   == std::make_tuple(false, false, true,  true ));

            assert(d(V{})                  == std::make_tuple(false, true,  false, false));
            assert(cd(V{})                 == std::make_tuple(true,  true,  false, false));
            assert(visit(v_)(V{})          == std::make_tuple(false, false, false, false));

            assert(dcv(V{})                == std::make_tuple(true,  true,  false, false));
            assert(cdcv(V{})               == std::make_tuple(true,  true,  false, false));
            assert(visit(cv_)(V{})         == std::make_tuple(true,  false, false, false));

            assert(dmv(V{})                == std::make_tuple(false, true,  false, false));
            assert(cdmv(V{})               == std::make_tuple(true,  true,  false, false));
            assert(visit(visitor3{})(V{})  == std::make_tuple(false, false, false, false));
        }
        {
            using V = variant< RW< int >, double >;
            std::stringstream ss_;
            ss_.str("1");
            V v = 2;
            ss_ >> v;
            assert(v == 1);
            v = 3.5;
            ss_.str();
            ss_.clear();
            ss_ << v;
            assert(ss_.str() == "3.5");
        }
        { // invoke
            struct A { int operator () (int) { return 0; } bool operator () (double) { return true; } };
            struct B { int operator () (int) { return 1; } bool operator () (double) { return false; } };
            using V = variant< A, B >;
            V v;
            static_assert(std::is_same< int, decltype(invoke(v, 0)) >{});
            static_assert(std::is_same< bool, decltype(invoke(v, 0.0)) >{});
            assert(invoke(v, 0) == 0);
            assert(invoke(v, 1.0) == true);
            v = B{};
            assert(invoke(v, 0) == 1);
            assert(invoke(v, 1.0) == false);
        }
        { // invoke
            auto a = [] (auto &&... _values) -> int { return +static_cast< int >(sizeof...(_values)); };
            auto b = [] (auto &&... _values) -> int { return -static_cast< int >(sizeof...(_values)); };
            using V = variant< decltype(a), decltype(b) >;
            V v = a;
            static_assert(std::is_same< int, decltype(invoke(v, 0)) >{});
            assert(invoke(v, 0, 1.0f, 2.0, 3.0L, true) == +5);
            assert(invoke(v) == 0);
            assert(invoke(v, nullptr) == 1);
            v = std::move(b);
            assert(invoke(v, 0, 1.0f, 2.0, 3.0L, true) == -5);
            assert(invoke(v) == 0);
            assert(invoke(v, nullptr) == -1);
        }
        {
            auto const l0 = [] (auto const &) { return 0; };
            auto const l1 = [] (auto &) { return 1; };
            auto const l2 = [] (auto const &&) { return 2; };
            auto const l3 = [] (auto &&) { return 3; };
            auto const l4 = [] (auto && arg) -> std::enable_if_t< std::is_rvalue_reference< decltype(arg) >{}, int > { return 4; };
            {
                struct A {};
                using V = variant< A >;
                V v;
                V const c{};
                auto const l = compose_visitors(l0, l1, l2, l3, [] { ; });
                assert(0 == visit(l, c));
                assert(1 == visit(l, v));
                assert(2 == visit(l, std::move(c)));
                assert(3 == visit(l, std::move(v)));
                assert(3 == visit(l, std::move(v)));
                static_assert(std::is_void< decltype(visit(l)()) >{});
            }
            {
                struct A
                {

                    A(int) { ; }
                    A() = delete;
                    A(A const &) = delete;
                    A(A &) = delete;
                    A(A &&) = delete;
                    A & operator = (A const &) = delete;
                    A & operator = (A &) = delete;
                    A & operator = (A &&) = delete;

                };
                using V = variant< RW< A > >;
                V av{in_place, 0};
                V const ac{in_place, 1};
                {
                    auto const l = compose_visitors(l0, l1, l2, l3);
                    assert(0 == l(ac));
                    assert(1 == l(av));
                    assert(2 == l(std::move(ac)));
                    assert(3 == l(std::move(av)));
                }
                {
                    auto const l = compose_visitors(l0, l1, l2, l4);
                    assert(0 == l(ac));
                    assert(1 == l(av));
                    assert(2 == l(std::move(ac)));
                    assert(4 == l(std::move(av)));
                }
                {
                    auto const l = compose_visitors(l0);
                    assert(0 == l(ac));
                    assert(0 == l(av));
                    assert(0 == l(std::move(ac)));
                    assert(0 == l(std::move(av)));
                }
                {
                    auto const l = compose_visitors(l1);
                    assert(1 == l(ac));
                    assert(1 == l(av));
                    assert(1 == l(std::move(ac)));
                    //assert(1 == l(std::move(v)));
                }
                {
                    auto const l = compose_visitors(l2);
                    //assert(2 == l(c));
                    //assert(2 == l(v));
                    assert(2 == l(std::move(ac)));
                    assert(2 == l(std::move(av)));
                }
                {
                    auto const l = compose_visitors(l3);
                    assert(3 == l(ac));
                    assert(3 == l(av));
                    assert(3 == l(std::move(ac)));
                    assert(3 == l(std::move(av)));
                }
                {
                    auto const l = compose_visitors(l0, l1);
                    assert(0 == l(ac));
                    assert(1 == l(av));
                    assert(0 == l(std::move(ac)));
                    assert(0 == l(std::move(av)));
                }
                {
                    auto const l = compose_visitors(l0, l2);
                    assert(0 == l(ac));
                    assert(0 == l(av));
                    assert(2 == l(std::move(ac)));
                    assert(2 == l(std::move(av)));
                }
                {
                    auto const l = compose_visitors(l0, l3);
                    assert(0 == l(ac));
                    assert(3 == l(av));
                    assert(3 == l(std::move(ac)));
                    assert(3 == l(std::move(av)));
                }
                {
                    auto const l = compose_visitors(l1, l2);
                    assert(1 == l(ac));
                    assert(1 == l(av));
                    assert(2 == l(std::move(ac)));
                    assert(2 == l(std::move(av)));
                }
                {
                    auto const l = compose_visitors(l1, l3);
                    assert(1 == l(ac));
                    assert(1 == l(av));
                    assert(3 == l(std::move(ac)));
                    assert(3 == l(std::move(av)));
                }
                {
                    auto const l = compose_visitors(l2, l3);
                    assert(3 == l(ac));
                    assert(3 == l(av));
                    assert(2 == l(std::move(ac)));
                    assert(3 == l(std::move(av)));
                }
                {
                    auto const l = compose_visitors(l0, l1, l2);
                    assert(0 == l(ac));
                    assert(1 == l(av));
                    assert(2 == l(std::move(ac)));
                    assert(2 == l(std::move(av)));
                }
                {
                    auto const l = compose_visitors(l0, l1, l3);
                    assert(0 == l(ac));
                    assert(1 == l(av));
                    assert(3 == l(std::move(ac)));
                    assert(3 == l(std::move(av)));
                }
                {
                    auto const l = compose_visitors(l0, l1, l4);
                    assert(0 == l(ac));
                    assert(1 == l(av));
                    assert(4 == l(std::move(ac)));
                    assert(4 == l(std::move(av)));
                }
                {
                    auto const l = compose_visitors(l0, l2, l3);
                    assert(0 == l(ac));
                    assert(3 == l(av));
                    assert(2 == l(std::move(ac)));
                    assert(3 == l(std::move(av)));
                }
                {
                    auto const l = compose_visitors(l0, l2, l4);
                    assert(0 == l(ac));
                    assert(0 == l(av));
                    assert(2 == l(std::move(ac)));
                    assert(4 == l(std::move(av)));
                }
                {
                    auto const l = compose_visitors(l1, l2, l3);
                    assert(1 == l(ac));
                    assert(1 == l(av));
                    assert(2 == l(std::move(ac)));
                    assert(3 == l(std::move(av)));
                }
            }
            {
                auto const l = compose_visitors(l0, l1, l2, l3);
                struct A {};
                using V = variant< RW< A > >;
                V a;
                V const c{};
                {
                    auto const la = compose_visitors([] (A const &) { return -1; }, l);
                    assert(-1 == visit(la, c));
                    assert(1 == la(a));
                    assert(2 == la(std::move(c)));
                    assert(3 == la(A{}));
                }
                {
                    struct F { auto operator () (A const &) && { return -11; } };
                    auto lam = compose_visitors(F{}, l);
                    assert(0 == lam(c));
                    assert(-11 == visit(std::move(lam), c));
                }
            }
        }
        { // function pointers
            using V = variant< decltype(&f), decltype(&g) >;
            V v = g;
            assert(v.active< decltype(&f) >());
            assert(static_cast< decltype(&f) >(v) == &g);
            assert(invoke(v) == 2);
            v = f;
            assert(v.active< decltype(&g) >());
            assert(static_cast< decltype(&g) >(v) == &f);
            assert(invoke(v) == 1);
            auto l = [] { return 323; };
            v = static_cast< decltype(&g) >(l);
            assert(invoke(v) == 323);
        }
    }
    { // multivisit mixed visitables
        struct A {};
        struct B {};
        using U = versatile< A, B >;
        using V = variant< A, B >;
        struct
        {
            int operator () (A, A) { return 0; }
            int operator () (A, B) { return 1; }
            int operator () (B, A) { return 2; }
            int operator () (B, B) { return 3; }
        } v;

        A a;
        B b;

        assert(multivisit(v, U{a}, V{a}) == 0);
        assert(multivisit(v, U{a}, V{b}) == 1);
        assert(multivisit(v, U{b}, V{a}) == 2);
        assert(multivisit(v, U{b}, V{b}) == 3);

        assert(multivisit(v, V{a}, U{a}) == 0);
        assert(multivisit(v, V{a}, U{b}) == 1);
        assert(multivisit(v, V{b}, U{a}) == 2);
        assert(multivisit(v, V{b}, U{b}) == 3);
    }
    { // boost::variant visitation
        struct A {};
        struct B {};
        struct
        {
            int operator () (A, A) { return 0; }
            int operator () (A, B) { return 1; }
            int operator () (B, A) { return 2; }
            int operator () (B, B) { return 3; }
        } v;
        using V = boost_variant_i< boost::recursive_wrapper< A >, B >;
        V a{A{}};
        assert(a.active< A >());
        assert(a.active< boost::recursive_wrapper< A > >());
        V b{B{}};
        assert(b.active< B >());
        assert(multivisit(v, a, a) == 0);
        assert(multivisit(v, a, b) == 1);
        assert(multivisit(v, b, a) == 2);
        assert(multivisit(v, b, b) == 3);
    }
    { // boost::variant visitation
        struct A {};
        struct B {};
        struct
        {
            int operator () (A, A) { return 0; }
            int operator () (A, B) { return 1; }
            int operator () (B, A) { return 2; }
            int operator () (B, B) { return 3; }
        } v;
        using V = boost_variant_c< A, boost::recursive_wrapper< B > >;
        V a{A{}};
        assert(a.active< A >());
        V b{B{}};
        assert(b.active< B >());
        assert(b.active< boost::recursive_wrapper< B > >());
        assert(multivisit(v, a, a) == 0);
        assert(multivisit(v, a, b) == 1);
        assert(multivisit(v, b, a) == 2);
        assert(multivisit(v, b, b) == 3);
    }
    { // handling of the empty
        versatile<> empty;
        using V = versatile< int, double, versatile<> >;
        V v;
        auto l = compose_visitors([] (int) { return 0; }, [] (double) { return 1; }, [] (auto x) { static_assert(std::is_same< decltype(x), versatile<> >{}); return 2; });
        assert(0 == multivisit(l, int{}));
        assert(1 == multivisit(l, double{}));
        assert(2 == multivisit(l, empty));
        assert(0 == multivisit(l, v));
        assert(1 == multivisit(l, V{double{}}));
        assert(0 == multivisit(l, V{empty})); // default construction equivalent
    }
    { // aggregate wrapper
        struct X {};
        struct Y {};

        struct XY { X x; Y y; XY() = delete; XY(XY const &) = delete; XY(XY &&) = default; XY & operator = (XY &&) = default; };
        XY xy{}; // value-initialization by empty list initializer
        xy.x = {}; xy.y = {}; // accessible
        xy = XY{};
        static_assert(std::is_constructible< XY, XY >{});
        static_assert(!std::is_copy_constructible< XY >{});
        static_assert(std::is_move_constructible< XY >{});
        static_assert(!std::is_default_constructible< XY >{});

        using WXY = AW< XY >;
        static_assert(std::is_assignable< WXY &, XY >{});
        static_assert(std::is_constructible< WXY, XY >{});
        static_assert(!std::is_copy_constructible< WXY >{}); // mimic
        static_assert(std::is_move_constructible< WXY >{});
        static_assert(!std::is_default_constructible< WXY >{});
        WXY wxy{std::move(xy)};
        wxy = XY{};
        wxy = WXY{XY{}};

        using V = variant< WXY >;
        static_assert(std::is_assignable< V &, XY >{});
        static_assert(std::is_assignable< V &, WXY >{});
        static_assert(std::is_constructible< V, XY >{});
        static_assert(std::is_copy_constructible< V >{}); // lie
        static_assert(std::is_move_constructible< V >{});
        static_assert(std::is_default_constructible< V >{});
        V v{in_place, X{}, Y{}};
        assert(v.active< XY >());
        assert(v.active< WXY >());
        v = XY{};
        v = WXY{XY{}};
        v = V{WXY{XY{}}};
    }
    { // aggregates
        struct X {};
        struct Y {};
        struct XY { X x; Y y = Y{}; };
        using WXY = AW< XY >;
        using V = variant< WXY, X, Y >;
        V v;
        assert(v.active< XY >());
        v.replace(X{});
        assert(v.active< X >());
        v.replace(Y{});
        assert(v.active< Y >());
        v.emplace(X{});
        assert(v.active< XY >());
        v.emplace(Y{});
        assert(v.active< Y >());
    }
    { // aggregates
        struct X {};
        struct Y {};
        struct XY { X x; Y y; };
        using AXY = AW< XY >;
        using RAXY = RW< AXY >;
        using V = variant< RAXY >;
        V v;
        v = XY{};
        v = AXY{};
        v = RAXY{};
        v = V{};
    }
    { // constexpr
        struct A {};
        struct B {};
        struct C {};
        using V = cvariant< A, B, C >;
        static_assert(std::is_literal_type< V >{});

        struct visitor3
        {
            constexpr auto operator () (A, int i = 1) const { return 100 + i; }
            constexpr auto operator () (B, int i = 2) const { return 200 + i; }
            constexpr auto operator () (C, int i = 3) const { return 300 + i; }
        };
        static_assert(std::is_literal_type< visitor3 >{});

        // rrefs
        static_assert(visit(visitor3{}, V{A{}}) == 101);
        static_assert(visit(visitor3{}, V{B{}}) == 202);
        static_assert(visit(visitor3{}, V{C{}}) == 303);

        static_assert(visit(visitor3{}, V{A{}}, 10) == 110);
        static_assert(visit(visitor3{}, V{B{}}, 20) == 220);
        static_assert(visit(visitor3{}, V{C{}}, 30) == 330);

        // lrefs
        constexpr visitor3 visitor3_{};
        constexpr V a{A{}};
        constexpr V b{B{}};
        constexpr V c{C{}};

        static_assert(visit(visitor3_, a) == 101);
        static_assert(visit(visitor3_, b) == 202);
        static_assert(visit(visitor3_, c) == 303);

        static_assert(visit(visitor3_, a, 11) == 111);
        static_assert(visit(visitor3_, b, 22) == 222);
        static_assert(visit(visitor3_, c, 33) == 333);

        struct multivisitor3
        {
            constexpr auto operator () (A, int, A) const { return 111; }
            constexpr auto operator () (A, int, B) const { return 112; }
            constexpr auto operator () (A, int, C) const { return 113; }
            constexpr auto operator () (B, int, A) const { return 121; }
            constexpr auto operator () (B, int, B) const { return 122; }
            constexpr auto operator () (B, int, C) const { return 123; }
            constexpr auto operator () (C, int, A) const { return 131; }
            constexpr auto operator () (C, int, B) const { return 132; }
            constexpr auto operator () (C, int, C) const { return 133; }
            constexpr auto operator () (A, double, A) const { return 211; }
            constexpr auto operator () (A, double, B) const { return 212; }
            constexpr auto operator () (A, double, C) const { return 213; }
            constexpr auto operator () (B, double, A) const { return 221; }
            constexpr auto operator () (B, double, B) const { return 222; }
            constexpr auto operator () (B, double, C) const { return 223; }
            constexpr auto operator () (C, double, A) const { return 231; }
            constexpr auto operator () (C, double, B) const { return 232; }
            constexpr auto operator () (C, double, C) const { return 233; }
        };
        static_assert(std::is_literal_type< multivisitor3 >{});

        constexpr auto multivisitor3_ = visit(multivisitor3{});
        static_assert(multivisitor3_(a,   0, a) == 111);
        static_assert(multivisitor3_(a,   0, b) == 112);
        static_assert(multivisitor3_(a,   0, c) == 113);
        static_assert(multivisitor3_(b,   0, a) == 121);
        static_assert(multivisitor3_(b,   0, b) == 122);
        static_assert(multivisitor3_(b,   0, c) == 123);
        static_assert(multivisitor3_(c,   0, a) == 131);
        static_assert(multivisitor3_(c,   0, b) == 132);
        static_assert(multivisitor3_(c,   0, c) == 133);
        static_assert(multivisitor3_(a, 0.0, a) == 211);
        static_assert(multivisitor3_(a, 0.0, b) == 212);
        static_assert(multivisitor3_(a, 0.0, c) == 213);
        static_assert(multivisitor3_(b, 0.0, a) == 221);
        static_assert(multivisitor3_(b, 0.0, b) == 222);
        static_assert(multivisitor3_(b, 0.0, c) == 223);
        static_assert(multivisitor3_(c, 0.0, a) == 231);
        static_assert(multivisitor3_(c, 0.0, b) == 232);
        static_assert(multivisitor3_(c, 0.0, c) == 233);
    }
    { // reference_wrapper
        struct A {};
        using RA = std::reference_wrapper< A >;
        using V = variant< RA >;
        A a;
        RA ra{a};
        V v{a};
        V w{ra};
        v = a;
        v = ra;
        v = w;
    }
    {
        assert((test_perferct_forwarding< variant, 2, 6 >{}()));
    }
    { // -ftemplate-depth=40 for 5, 5
        assert((hard< ROWS, COLS >()));
    }
    return EXIT_SUCCESS;
}
#endif

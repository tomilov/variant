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

namespace versatile
{

namespace test_traits
{

struct A {};
struct B {};

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

namespace test
{

template< typename from, typename to >
struct is_explicitly_convertible
        : std::bool_constant< (std::is_constructible< to, from >::value && !std::is_convertible< from, to >::value) >
{

};

template< typename type, typename variant >
CONSTEXPRF
bool
check_active(variant const & _variant) noexcept
{
    return _variant.template active< type >();
}

enum class state
{
    zero,
    default_constructed,
    copy_constructed,
    move_constructed,
    move_assigned,
    copy_assigned,
    moved_from,
};

struct oracle // akrzemi1's optional test
{

    CONSTEXPRF oracle() noexcept : s(state::default_constructed) { ; }

    CONSTEXPRF oracle(oracle const &) noexcept : s(state::copy_constructed) { ; }
    CONSTEXPRF oracle(oracle && o) noexcept : s(state::move_constructed) { o.s = state::moved_from; }

    CONSTEXPRF oracle & operator = (oracle const &) noexcept { s = state::copy_assigned; return *this; }
    CONSTEXPRF oracle & operator = (oracle && o) noexcept { s = state::move_assigned; o.s = state::moved_from; return *this; }

    friend
    CONSTEXPRF
    inline
    bool
    operator == (oracle const & o, state const _s) noexcept
    {
        return (o.s == _s);
    }

    friend
    CONSTEXPRF
    inline
    bool
    operator == (state const _s, oracle const & o) noexcept
    {
        return operator == (o, _s);
    }

    friend
    CONSTEXPRF
    inline
    bool
    operator != (oracle const & o, state const _s) noexcept
    {
        return !operator == (o, _s);
    }

    friend
    CONSTEXPRF
    inline
    bool
    operator != (state const _s, oracle const & o) noexcept
    {
        return !operator == (o, _s);
    }

private :

    state s;

};

template< typename first >
struct aggregate_wrapper
        : identity< ::versatile::aggregate_wrapper< first > >
{

};

template< template< typename ... > class wrapper = identity, template< typename ... > class variant = versatile >
class case_constexpr
{

    template< typename ...types >
    using V = variant< typename wrapper< types >::type... >;

    using O = oracle;

    CONSTEXPRF
    static
    bool
    triviality() noexcept
    {
        {
            struct S {};
            ASSERT (std::is_trivial_v< S >);
            ASSERT (std::is_literal_type_v< S >);
            ASSERT (std::is_standard_layout_v< S >);
            ASSERT (std::is_pod_v< S >);
            using U = V< S >;
            ASSERT (std::is_trivial_v< U >);
            ASSERT (std::is_literal_type_v< U >);
            ASSERT (std::is_standard_layout_v< U >);
            ASSERT (std::is_pod_v< U >);
            ASSERT (!std::is_union_v< U >);
        }
        {
            struct S { S() { ; } };
            ASSERT (std::is_default_constructible_v< S >);
            ASSERT (!std::is_trivially_default_constructible_v< S >); // user-provided
            ASSERT (std::is_trivially_copyable_v< S >);
            using U = V< S >;
            ASSERT (std::is_default_constructible_v< U >);
            ASSERT (!std::is_trivially_default_constructible_v< U >); // U is a union with at least one variant member with non-trivial default constructor
            ASSERT (std::is_trivially_copyable_v< U >);
        }
        {
            struct S { ~S() { ; } };
            ASSERT (std::is_destructible_v< S >);
            ASSERT (!std::is_trivially_destructible_v< S >); // user-provided
            using U = V< S >;
            ASSERT (std::is_destructible_v< U >);
            ASSERT (!std::is_trivially_destructible_v< U >); // U is a union and has a variant member with non-trivial destructor
        }
        {
            struct S { S(S const &) { ; } };
            ASSERT (!std::is_default_constructible_v< S >); // If no user-defined constructors of any kind are provided for a class type (struct, class, or union), the compiler will always declare a default constructor as an inline public member of its class.
            ASSERT (!std::is_trivially_default_constructible_v< S >); // not default constructible
            ASSERT (std::is_destructible_v< S >);
            ASSERT (std::is_trivially_destructible_v< S >);
            ASSERT (std::is_copy_constructible_v< S >);
            ASSERT (std::is_move_constructible_v< S >); // S const & can bind S && => move constructible
            ASSERT (!std::is_trivially_copy_constructible_v< S >); // user-provided
            ASSERT (!std::is_trivially_move_constructible_v< S >); // match prev?
            ASSERT (std::is_copy_assignable_v< S >);
            ASSERT (std::is_move_assignable_v< S >);
            ASSERT (std::is_trivially_copy_assignable_v< S >);
            ASSERT (std::is_trivially_move_assignable_v< S >);
            using U = V< S >;
            ASSERT (!std::is_default_constructible_v< U >); // hard error in non-trivial default constructor of class "destructor"
            ASSERT (!std::is_trivially_default_constructible_v< U >); // the same error
            ASSERT (std::is_destructible_v< U >);
            ASSERT (std::is_trivially_destructible_v< U >);
            ASSERT (!std::is_copy_constructible_v< U >); // U is a union and has a variant member with non-trivial copy constructor
            ASSERT (std::is_move_constructible_v< U >); // U const & can bind U && => move constructible
            ASSERT (!std::is_trivially_copy_constructible_v< U >); // inherits
            ASSERT (!std::is_trivially_move_constructible_v< U >); // inherits
            ASSERT (std::is_copy_assignable_v< U >);
            ASSERT (std::is_move_assignable_v< U >);
            ASSERT (std::is_trivially_copy_assignable_v< U >);
            ASSERT (std::is_trivially_move_assignable_v< U >);
        }
        {
            struct S { S(S &&) { ; } };
            ASSERT (!std::is_default_constructible_v< S >); // If no user-defined constructors of any kind are provided for a class type (struct, class, or union), the compiler will always declare a default constructor as an inline public member of its class.
            ASSERT (!std::is_trivially_default_constructible_v< S >); // not default constructible
            ASSERT (std::is_destructible_v< S >);
            ASSERT (std::is_trivially_destructible_v< S >);
            ASSERT (!std::is_copy_constructible_v< S >); // S has a user-defined move constructor or move assignment operator
            ASSERT (std::is_move_constructible_v< S >);
            ASSERT (!std::is_trivially_copy_constructible_v< S >); // match next?
            ASSERT (!std::is_trivially_move_constructible_v< S >); // user-provided
            ASSERT (!std::is_copy_assignable_v< S >); // S  has a user-defined move constructor or move assignment operator
            ASSERT (!std::is_move_assignable_v< S >); // If there are no user-declared move constructors then the compiler will declare a move assignment operator as an inline public member of its class with the signature T& T::operator=(T&&).
            ASSERT (!std::is_trivially_copy_assignable_v< S >); // not copy-assignable
            ASSERT (!std::is_trivially_move_assignable_v< S >); // not move-assignable
            using U = V< S >;
            ASSERT (!std::is_default_constructible_v< U >); // hard error in non-trivial default constructor of class "destructor"
            ASSERT (!std::is_trivially_default_constructible_v< U >); // the same error
            ASSERT (std::is_destructible_v< U >);
            ASSERT (std::is_trivially_destructible_v< U >);
            ASSERT (!std::is_copy_constructible_v< U >); // U has non-static data members that cannot be copied
            ASSERT (std::is_move_constructible_v< U >);
            ASSERT (!std::is_trivially_copy_constructible_v< U >); // inherits
            ASSERT (!std::is_trivially_move_constructible_v< U >); // inherits
            ASSERT (!std::is_copy_assignable_v< U >); // inherits
            ASSERT (!std::is_move_assignable_v< U >); // inherits
            ASSERT (!std::is_trivially_copy_assignable_v< U >); // not copy-assignable
            ASSERT (!std::is_trivially_move_assignable_v< U >); // not move-assignable
        }
        {
            struct S { S & operator = (S const &) { return *this; } };
            ASSERT (std::is_default_constructible_v< S >);
            ASSERT (std::is_trivially_default_constructible_v< S >);
            ASSERT (std::is_destructible_v< S >);
            ASSERT (std::is_trivially_destructible_v< S >);
            ASSERT (std::is_copy_constructible_v< S >);
            ASSERT (std::is_move_constructible_v< S >);
            ASSERT (std::is_trivially_copy_constructible_v< S >);
            ASSERT (std::is_trivially_move_constructible_v< S >);
            ASSERT (std::is_copy_assignable_v< S >);
            ASSERT (std::is_move_assignable_v< S >);
            ASSERT (!std::is_trivially_copy_assignable_v< S >); // user-provided
            ASSERT (!std::is_trivially_move_assignable_v< S >); // match prev?
            using U = V< S >;
            ASSERT (std::is_default_constructible_v< U >);
            ASSERT (std::is_trivially_default_constructible_v< U >);
            ASSERT (std::is_destructible_v< U >);
            ASSERT (std::is_trivially_destructible_v< U >);
            ASSERT (std::is_copy_constructible_v< U >);
            ASSERT (std::is_move_constructible_v< U >);
            ASSERT (std::is_trivially_copy_constructible_v< U >);
            ASSERT (std::is_trivially_move_constructible_v< U >);
            ASSERT (!std::is_copy_assignable_v< U >); // U is a union-like class, and has a variant member whose corresponding assignment operator is non-trivial
            ASSERT (!std::is_move_assignable_v< U >); // match prev?
            ASSERT (!std::is_trivially_copy_assignable_v< U >); // inherits
            ASSERT (!std::is_trivially_move_assignable_v< U >); // inherits
        }
        {
            struct S { S & operator = (S &&) { return *this; } };
            ASSERT (std::is_default_constructible_v< S >);
            ASSERT (std::is_trivially_default_constructible_v< S >);
            ASSERT (std::is_destructible_v< S >);
            ASSERT (std::is_trivially_destructible_v< S >);
            ASSERT (!std::is_copy_constructible_v< S >); // S has a user-defined move constructor or move assignment operator
            ASSERT (!std::is_move_constructible_v< S >); // If there are no user-declared move assignment operators then the compiler will declare a move constructor as a non-explicit inline public member of its class with the signature T::T(T&&).
            ASSERT (!std::is_trivially_copy_constructible_v< S >); // match next?
            ASSERT (!std::is_trivially_move_constructible_v< S >); // user-provided
            ASSERT (!std::is_copy_assignable_v< S >); // S  has a user-defined move constructor or move assignment operator
            ASSERT (std::is_move_assignable_v< S >);
            ASSERT (!std::is_trivially_copy_assignable_v< S >); // not copy-assignable
            ASSERT (!std::is_trivially_move_assignable_v< S >); // not move-assignable
            using U = V< S >;
            ASSERT (std::is_default_constructible_v< U >); // hard error in non-trivial default constructor of class "destructor"
            ASSERT (std::is_trivially_default_constructible_v< U >); // the same error
            ASSERT (std::is_destructible_v< U >);
            ASSERT (std::is_trivially_destructible_v< U >);
            ASSERT (!std::is_copy_constructible_v< U >); // U has non-static data members that cannot be copied
            ASSERT (std::is_move_constructible_v< U >);
            ASSERT (!std::is_trivially_copy_constructible_v< U >); // inherits
            ASSERT (std::is_trivially_move_constructible_v< U >); // ????????! BUG: clang
            ASSERT (!std::is_copy_assignable_v< U >); // inherits
            ASSERT (!std::is_move_assignable_v< U >); // ! still valid? (until C++14) T has a non-static data member or a direct or virtual base without a move assignment operator that is not trivially copyable.
            ASSERT (!std::is_trivially_copy_assignable_v< U >); // not copy-assignable
            ASSERT (!std::is_trivially_move_assignable_v< U >); // not move-assignable
        }
        return true;
    }

    CONSTEXPRF
    static
    bool
    default_constructor() noexcept
    {
        {
            struct II { II() { ; }     ~II() { ; } };
            struct OI { OI() = delete; ~OI() { ; } };
            struct IO { IO() { ; }     ~IO() = delete; };
            struct OO { OO() = delete; ~OO() = delete; };
            CONSTEXPR auto c_ii = __is_constructible(II);
            CONSTEXPR auto c_oi = __is_constructible(OI);
            CONSTEXPR auto c_io = __is_constructible(IO);
            CONSTEXPR auto c_oo = __is_constructible(OO);
            {
                using VII = V< II >;
                ASSERT (__is_constructible(VII) == c_ii);
            }
            {
                using VOI = V< OI >;
                ASSERT (__is_constructible(VOI) == c_oi);
            }
            {
                using VIO = V< IO >;
                ASSERT (__is_constructible(VIO) == c_io);
            }
            {
                using VOO = V< OO >;
                ASSERT (__is_constructible(VOO) == c_oo);
            }
        }
        {
            struct A { int i; };
            using VA = V< A >;
            ASSERT (__has_trivial_constructor(VA));
            CONSTEXPR VA v{};
            ASSERT (check_active< A >(v));
            ASSERT (static_cast< A >(v).i == int{});
        }
        {
            struct A {};
            struct B { B() = delete; };
            using VAB = V< A, B >;
            using VBA = V< B, A >;
            ASSERT (!std::is_trivially_default_constructible_v< VAB >);
            ASSERT (!std::is_trivially_default_constructible_v< VBA >);
        }
        {
            struct A {};
            struct B {};
            using VAOB = variant< O, A, B >;
            ASSERT (__is_constructible(VAOB));
            ASSERT (!__has_trivial_constructor(O));
            ASSERT (!__has_trivial_constructor(VAOB));
            CBRA
                    VAOB v;
            CHECK (check_active< O >(v));
            CHECK (static_cast< O & >(v) == state::default_constructed);
            CKET
                    CONSTEXPR VAOB c{};
            ASSERT (check_active< O >(c));
            ASSERT (static_cast< O const & >(c) == state::default_constructed);
        }
        {
            struct A { A() = delete; int i; };
            struct B { CONSTEXPRF B() : d{} { ; } double d; }; // if there is not constructor-list initializer for d, then "is not integral constant expression"
            using VAB = V< A, B >;
            ASSERT (__is_constructible(VAB));
            ASSERT (!__has_trivial_constructor(VAB));
            CONSTEXPR VAB v{};
            ASSERT ((check_active< B >(v)));
            ASSERT (!(check_active< A >(v)));
        }
        {
            struct A { CONSTEXPRF A(int) { ; } };
            struct B { CONSTEXPRF B(char) { ; } };
            struct C {};
            using VABC = V< A, B, C >;
            CBRA
                    VABC v;
            CHECK ((check_active< C >(v)));
            CKET
        }
        {
            struct A { CONSTEXPRF A(char) { ; } char c; };
            using VA = variant< A >;
            ASSERT (!__is_constructible(VA));
            ASSERT (!__has_trivial_constructor(VA));
            CBRA
                    VA v{'1'};
            CHECK (check_active< A >(v));
            CKET
        }
        return true;
    }

    CONSTEXPRF
    static
    bool
    conversion_constructor_and_conversion_operator() noexcept
    {
        struct A { int i = 1; };
        struct B { int i = 2; };
        struct C { int i = 3; };
        {
            using VAB = V< A, B >;
            CONSTEXPR VAB v{};
            ASSERT (static_cast< A const & >(v).i == 1);
            ASSERT (static_cast< A >(v).i == 1);
            CONSTEXPR VAB b{B{}};
            ASSERT (static_cast< B const & >(b).i == 2);
            ASSERT (static_cast< B >(b).i == 2);
            CONSTEXPR VAB a{A{3}};
            ASSERT (static_cast< A const & >(a).i == 3);
            ASSERT (static_cast< A >(a).i == 3);
        }
        {
            using VAB = V< A, B >;
            ASSERT (is_explicitly_convertible< VAB, A >::value);
            ASSERT (is_explicitly_convertible< VAB, B >::value);
            ASSERT (!is_explicitly_convertible< VAB, C >::value);
            //A x = VAB{A{10}}; // implicit conversion is not allowed
            CONSTEXPR A a(VAB{A{10}}); // direct initialization is allowed
            ASSERT (a.i == 10);
            CONSTEXPR B b(VAB{B{20}}); // direct initialization is allowed
            ASSERT (b.i == 20);
            CBRA
                    O o(V< O >{O{}});
            CHECK (o != state::move_constructed); // ! currently clang++ has bug #19917 for static_cast to rvalue reference and cv-ref-qualified overloaded conversion operators causes "ambiguity" error
            CHECK (o == state::copy_constructed);
            CKET
        }
        {
            using VAOB = V< A, O, B >;
            CBRA
                    O o;
            CHECK (o == state::default_constructed);
            VAOB v = o;
            CHECK (check_active< O >(v));
            CHECK (o == state::default_constructed);
            CHECK (static_cast< O & >(v) == state::copy_constructed);
            CHECK (o == state::default_constructed);
            VAOB m = std::move(o);
            CHECK (o == state::moved_from);
            CHECK (check_active< O >(m));
            CHECK (static_cast< O & >(m) == state::move_constructed);
            CKET
        }
        {
            using VAOB = V< A, O, B >;
            CBRA
                    VAOB v{O{}};
            CHECK (check_active< O >(v));
            CHECK (static_cast< O & >(v) == state::move_constructed);
            O o = static_cast< O & >(v);
            CHECK (static_cast< O & >(v) == state::move_constructed);
            CHECK (o == state::copy_constructed);
            CKET
        }
        {
            using VAOB = V< A, O, B >;
            CBRA
                    VAOB v{O{}};
            CHECK (check_active< O >(v));
            CHECK (static_cast< O & >(v) == state::move_constructed);
            O o = static_cast< O && >(static_cast< O & >(v));
            CHECK (static_cast< O & >(v) == state::moved_from);
            CHECK (o == state::move_constructed);
            CKET
        }
        {
            struct A { int i = 1; };
            struct B { int i = 2; };
            using VAB = V< A, B >;
            CBRA
                    VAB v;
            CHECK ((static_cast< A const & >(v).i == 1));
            CHECK ((static_cast< A & >(v).i == 1));
            static_cast< A & >(v) = A{2};
            CHECK ((static_cast< A & >(v).i == 2));
            VAB b{B{}};
            CHECK ((static_cast< B const & >(b).i == 2));
            CHECK ((static_cast< B & >(b).i == 2));
            static_cast< B & >(b) = B{1};
            CHECK ((static_cast< B & >(b).i == 1));
            VAB a{A{3}};
            CHECK ((static_cast< A const & >(a).i == 3));
            CHECK ((static_cast< A & >(a).i == 3));
            static_cast< A & >(a) = A{1};
            CHECK ((static_cast< A & >(a).i == 1));
            CKET
        }
        return true;
    }

    CONSTEXPRF
    static
    bool
    conversion_assignment() noexcept
    { // http://stackoverflow.com/questions/33936295/
        {
            struct A {};
            struct B {};
            using VAB = V< A, B >;
            ASSERT (__is_trivially_assignable(VAB, VAB));
            ASSERT (!__is_trivially_constructible(VAB, A)); // really operation is trivial in some sense
            ASSERT (!__is_trivially_assignable(VAB, A)); // really operation is trivial in some sense
            CBRA
                    VAB v{};
            CHECK (check_active< A >(v));
            v = B{};
            CHECK (check_active< B >(v));
            CKET
        }
        {
            using VX = variant< int, char, double >;
            ASSERT (std::is_trivial_v< VX >);
            CBRA
                    VX x = 1;
            CHECK (check_active< int >(x));
            x = 1.0;
            CHECK (check_active< double >(x));
            x = '\0';
            CHECK (check_active< char >(x));
            CKET
        }
        return true;
    }

    CONSTEXPRF
    static
    bool
    copy_constructor_and_move_constructor() noexcept
    {
        {
            struct A { int i = 1; };
            struct B { char c = 2; };
            using VAB = V< A, B >;
            CONSTEXPR VAB v{B{}};
            ASSERT (check_active< B >(v));
            CONSTEXPR VAB w = v;
            ASSERT (check_active< B >(w));
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
    copy_assignment_and_move_assignment() noexcept
    {
        struct A { int i = 1; };
        struct B { int j = 2; };
        struct C { int k = 3; };
        ASSERT (__has_trivial_copy(C));
        ASSERT (__has_trivial_assign(C));
        using VCBA = V< C, B, A >;
        ASSERT (std::is_trivially_copy_assignable< VCBA >{});
        {
            CONSTEXPR VCBA c{};
            ASSERT (check_active< C >(c));
            CONSTEXPR VCBA b = B{};
            ASSERT (check_active< B >(b));
            CONSTEXPR VCBA a = A{};
            ASSERT (check_active< A >(a));
        }
        {
            CBRA
                    VCBA x = A{};
            VCBA y = B{};
            VCBA z;
            VCBA w = A{};
            CHECK (check_active< A >(w));
            x = y;
            CHECK (check_active< B >(x));
            CHECK (check_active< B >(y));
            y = z;
            CHECK (check_active< C >(y));
            CHECK (check_active< C >(z));
            z = w;
            CHECK (check_active< B >(x));
            CHECK (check_active< C >(y));
            CHECK (check_active< A >(z));
            CKET
        }
        return true;
    }

    CONSTEXPRF
    static
    bool
    emplace_constructor() noexcept
    {
        {
            struct A {};
            struct B {};
            struct S { CONSTEXPRF S(A, B) { ; } };
            using VABS = V< S, A, B >;
            CONSTEXPR VABS v{};
            ASSERT (check_active< A >(v));
            CONSTEXPR VABS s{A{}, B{}};
            ASSERT (check_active< S >(s));
        }
        {
            struct A {};
            struct B {};
            struct S { CONSTEXPRF S(A) { ; } };
            using VSAB = V< S, B, A >;
            CONSTEXPR VSAB b{};
            ASSERT (check_active< B >(b));
            CONSTEXPR VSAB a{A{}};
            ASSERT (!check_active< S >(a));
            ASSERT (check_active< A >(a));
        }
        {
            struct A {};
            struct B {};
            struct C { A a; };
            struct D { B b; };
            CONSTEXPR variant< int, ::versatile::aggregate_wrapper< C >, ::versatile::aggregate_wrapper< D > > c{A{}};
            ASSERT (check_active< C >(c));
            CONSTEXPR variant< int, ::versatile::aggregate_wrapper< C >, ::versatile::aggregate_wrapper< D > > d{B{}};
            ASSERT (check_active< D >(d));
        }
        return true;
    }

public :

    CONSTEXPRF
    static
    bool
    crun() noexcept
    {
        ASSERT (triviality());
        ASSERT (default_constructor());
        ASSERT (copy_constructor_and_move_constructor());
        ASSERT (copy_assignment_and_move_assignment());
        ASSERT (conversion_constructor_and_conversion_operator());
        ASSERT (conversion_assignment());
        ASSERT (emplace_constructor());
        return true;
    }

};

template< typename variadic >
struct variadic_size;

template< template< typename ...types > class variadic, typename ...types >
struct variadic_size< variadic< types... > >
        : index_t< sizeof...(types) >
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

template< std::size_t M >
struct multivisitor
{

    using result_type = pair< M >;

    CONSTEXPRF
    std::size_t
    which() const noexcept
    {
        return 0;
    }

    template< typename ...types >
    CONSTEXPRF
    result_type
    operator () (types &&... _values) & noexcept
    {
        //ASSERT (M == sizeof...(types));
        //ASSERT (!(is_visitable< types >{} || ...));
        return {{{type_qualifier_of< multivisitor & >, type_qualifier_of< types && >...}}, {{M, _values...}}};
    }

    template< typename ...types >
    CONSTEXPRF
    result_type
    operator () (types &&... _values) const & noexcept
    {
        return {{{type_qualifier_of< multivisitor const & >, type_qualifier_of< types && >...}}, {{M, _values...}}};
    }

    template< typename ...types >
    CONSTEXPRF
    result_type
    operator () (types &&... _values) && noexcept
    {
        return {{{type_qualifier_of< multivisitor && >, type_qualifier_of< types && >...}}, {{M, _values...}}};
    }

    template< typename ...types >
    CONSTEXPRF
    result_type
    operator () (types &&... _values) const && noexcept
    {
        return {{{type_qualifier_of< multivisitor const && >, type_qualifier_of< types && >...}}, {{M, _values...}}};
    }

};

template< std::size_t M >
struct variadic_size< multivisitor< M > >
        : index_t< M >
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
        auto const lhs = multivisit(static_cast< add_qualifier_t< static_cast< type_qualifier >(qualifier_id_begin + Q), types > >(*std::get< K >(stuff_))...);
        ASSERT (sizeof...(types) == lhs.size());
        pair< (sizeof...(types) - 1) > const rhs = {{{static_cast< type_qualifier >(qualifier_id_begin + Q)...}}, {{(variadic_size< at_index_t< K, types... > >{} - 1 - std::get< K >(stuff_)->which())...}}};
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
          std::size_t M, std::size_t N = M,
          template< typename ...types > class variant = versatile >
class test_perferct_forwarding
{

    template< typename >
    struct variants;

    template< std::size_t ...J >
    struct variants< std::index_sequence< J... > >
    {

        using variant_type = variant< T< J >... >;
        static_assert(N == sizeof...(J));

        variant_type variants_[N] = {T< J >{}...};

    };

    template< std::size_t ...I >
    CONSTEXPRF
    static
    bool
    crun(std::index_sequence< I... >) noexcept
    {
        static_assert(sizeof...(I) == M);
        std::size_t indices[M] = {};
        for (std::size_t & n : indices) {
            n = 0;
        }
        multivisitor< M > mv;
        variants< std::make_index_sequence< N > > variants_; // non-const
        auto permutation_ = std::make_tuple(&mv, &variants_.variants_[indices[I]]...);
        auto const fusor_ = make_fusor(permutation_);
        CONSTEXPR std::size_t ref_count = (qualifier_id_end - qualifier_id_begin); // test only reference types
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

public :

    CONSTEXPRF
    static
    bool
    crun() noexcept
    {
        return crun(std::make_index_sequence< M >{});
    }

};

template< std::size_t I = 0 >
struct T
{

    std::size_t i = I;

    CONSTEXPRF
    operator std::size_t () const noexcept
    {
        return i;
    }

};

static_assert(std::is_standard_layout< T<> >{}, LOCATION);

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

} // namespace test
} // namespace versatile

#include <boost/variant.hpp>

template< typename ...types >
struct boost_variant_i
        : boost::variant< types... >
{

    using base = boost::variant< types... >;

    // can't correctly inherit constructors and assignment operators by `using base::base;` and `using base::operator =;`

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
    using index_t = ::versatile::index_at_t< ::versatile::unwrap_type_t< type >, ::versatile::unwrap_type_t< types >..., void >;

    template< typename type >
    constexpr
    bool
    active() const noexcept
    {
        return (index_t< type >::value == which());
    }

    template< typename type >
    explicit
    constexpr
    operator type const & () const &
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return boost::get< type const & >(static_cast< boost_variant_i::base const & >(*this));
    }

    template< typename type >
    explicit
    constexpr
    operator type & () &
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return boost::get< type & >(static_cast< boost_variant_i::base & >(*this));
    }

    template< typename type >
    explicit
    constexpr
    operator type const && () const &&
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return boost::get< type const && >(static_cast< boost_variant_i::base const && >(*this));
    }

    template< typename type >
    explicit
    constexpr
    operator type && () &&
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return boost::get< type && >(static_cast< boost_variant_i::base && >(*this));
    }

};

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
    using index_t = ::versatile::index_at_t< ::versatile::unwrap_type_t< type >, ::versatile::unwrap_type_t< types >..., void >;

    template< typename type >
    constexpr
    bool
    active() const noexcept
    {
        return (index_t< type >::value == which());
    }

    template< typename type >
    explicit
    constexpr
    operator type const & () const &
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return boost::get< type const & >(member_);
    }

    template< typename type >
    explicit
    constexpr
    operator type & () &
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return boost::get< type & >(member_);
    }

    template< typename type >
    explicit
    constexpr
    operator type const && () const &&
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return boost::get< type const && >(member_);
    }

    template< typename type >
    explicit
    constexpr
    operator type && () &&
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return boost::get< type && >(member_);
    }

private :

    boost::variant< types... > member_;

};

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
    using namespace versatile::test;
    {
        // either compile time or runtime:
        ASSERT (case_constexpr<>::crun());
        ASSERT (case_constexpr< aggregate_wrapper >::crun());
        //ASSERT (test_perferct_forwarding< T, 1, 1 >::crun());
        //ASSERT (test_perferct_forwarding< T, 2, 3 >::crun());
        //ASSERT (test_perferct_forwarding< T, 3, 2 >::crun());
    }
    {
        // boost::variant is olny runtime...
        static_assert(!std::is_literal_type< boost_variant_i< int, char, double > >{}, LOCATION); // ...while this is true:
        //assert ((test_perferct_forwarding< T, 2, 2, ::boost_variant_i >::crun()));
        //assert ((test_perferct_forwarding< T, 2, 2, ::boost_variant_c >::crun()));
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

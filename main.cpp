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

#define VERSATILE_RUNTIME

#define STR(S) #S
// double expansion of next macro argument
#define STRN(N) STR(N)
#define LOCATION "file '" __FILE__ "', line: " STRN(__LINE__)
#define SA(...) static_assert((__VA_ARGS__), LOCATION)
#ifdef VERSATILE_RUNTIME
#define ASSERT(...) { assert((__VA_ARGS__)); }
#define CONSTEXPR const
#define CONSTEXPRF
#define CHECK(...) { assert((__VA_ARGS__)); }
#define DESTRUCTOR { ; }
//#define CBRA {
//#define CKET }
#else
#define ASSERT(...) { SA(__VA_ARGS__); }
#define CONSTEXPR constexpr
#define CONSTEXPRF constexpr
#define CHECK(...) { if (!(__VA_ARGS__)) return false; }
#define DESTRUCTOR = default;
//#define CBRA { struct _ { static constexpr bool call() noexcept {
//#define CKET return true; } }; SA(_::call()); }
#endif

namespace test_traits
{

struct A {};
struct B {};

using ::versatile::copy_cv_reference_t;

SA(std::is_same_v< copy_cv_reference_t<          A         , B >,          B          >);
SA(std::is_same_v< copy_cv_reference_t<          A const   , B >,          B const    >);
SA(std::is_same_v< copy_cv_reference_t< volatile A         , B >, volatile B          >);
SA(std::is_same_v< copy_cv_reference_t< volatile A const   , B >, volatile B const    >);
SA(std::is_same_v< copy_cv_reference_t<          A        &, B >,          B        & >);
SA(std::is_same_v< copy_cv_reference_t<          A const  &, B >,          B const  & >);
SA(std::is_same_v< copy_cv_reference_t< volatile A        &, B >, volatile B        & >);
SA(std::is_same_v< copy_cv_reference_t< volatile A const  &, B >, volatile B const  & >);
SA(std::is_same_v< copy_cv_reference_t<          A       &&, B >,          B       && >);
SA(std::is_same_v< copy_cv_reference_t<          A const &&, B >,          B const && >);
SA(std::is_same_v< copy_cv_reference_t< volatile A       &&, B >, volatile B       && >);
SA(std::is_same_v< copy_cv_reference_t< volatile A const &&, B >, volatile B const && >);

}

namespace versatile
{ // little extension

template< type_qualifier type_qual, typename type >
constexpr
decltype(auto)
forward_as(type && _value) noexcept
{
    using ::versatile::add_qualifier_t;
    using decay_type = std::remove_cv_t< std::remove_reference_t< type > >;
    return static_cast< add_qualifier_t< type_qual, decay_type > >(_value);
}

template< typename type, typename ...types >
constexpr
bool
is_active(::versatile::versatile< types... > const & v) noexcept
{
    return v.template active< type >();
}

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

SA(std::is_literal_type_v< literal_type<> >);

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

SA(!std::is_literal_type_v< common_type<> >);

template< typename type >
static constexpr bool is_vcopy_constructible_v = std::is_constructible_v< type, type & >;

template< typename type >
static constexpr bool is_cmove_constructible_v = std::is_constructible_v< type, type const && >;

template< typename type >
static constexpr bool is_vcopy_assignable_v = std::is_assignable_v< type &, type & >;

template< typename type >
static constexpr bool is_cmove_assignable_v = std::is_assignable_v< type &, type const && >;

template< typename type >
static constexpr bool is_trivially_vcopy_constructible_v = std::is_trivially_constructible_v< type, type & >;

template< typename type >
static constexpr bool is_trivially_cmove_constructible_v = std::is_trivially_constructible_v< type, type const && >;

template< typename type >
static constexpr bool is_trivially_vcopy_assignable_v = std::is_trivially_assignable_v< type &, type & >;

template< typename type >
static constexpr bool is_trivially_cmove_assignable_v = std::is_trivially_assignable_v< type &, type const && >;

template< template< typename ... > class wrapper = ::versatile::identity,
          template< typename ... > class variant = ::versatile::versatile >
struct check_invariants
{

    template< typename ...types >
    using V = variant< typename wrapper< types >::type... >;

    struct literal_type_invariants
    {

        using L = literal_type<>;

        SA(std::is_literal_type_v< L >);
        SA(std::is_default_constructible_v< L >);
        SA(!std::is_trivially_default_constructible_v< L >);
        SA(std::is_trivially_copyable_v< L >);

        using U = V< L >;

        SA(std::is_literal_type_v< U >);
        SA(std::is_default_constructible_v< U >);
        SA(!std::is_trivially_default_constructible_v< U >);
        SA(std::is_trivially_copyable_v< U >);

        constexpr
        static
        bool
        run() noexcept
        {
            return true;
        }

    };

    struct common_type_invariants
    {

        using C = common_type<>;

        SA(std::is_default_constructible_v< C >);
        SA(!std::is_trivially_default_constructible_v< C >);
        SA(std::is_destructible_v< C >);
        SA(!std::is_trivially_destructible_v< C >);
        SA(std::is_copy_constructible_v< C >);
        SA(std::is_move_constructible_v< C >);
        SA(!std::is_trivially_copy_constructible_v< C >);
        SA(!std::is_trivially_move_constructible_v< C >);
        SA(std::is_copy_assignable_v< C >);
        SA(std::is_move_assignable_v< C >);
        SA(std::is_trivially_copy_assignable_v< C >); // user-provided
        SA(std::is_trivially_move_assignable_v< C >);

        using U = V< C >;

        SA(std::is_default_constructible_v< U >);
        SA(!std::is_trivially_default_constructible_v< U >);
        SA(std::is_destructible_v< U >);
        SA(!std::is_trivially_destructible_v< U >);
        SA(std::is_copy_constructible_v< U >);
        SA(std::is_move_constructible_v< U >);
        SA(!std::is_trivially_copy_constructible_v< U >);
        SA(!std::is_trivially_move_constructible_v< U >);
        SA(std::is_copy_assignable_v< U >);
        SA(std::is_move_assignable_v< U >);
        SA(std::is_trivially_copy_assignable_v< U >); // user-provided
        SA(std::is_trivially_move_assignable_v< U >);

        constexpr
        static
        bool
        run() noexcept
        {
            return true;
        }

    };

    struct trivial
    {

        struct S
        {

            int i;
            constexpr S(int j) : i(j) { ; }

            CONSTEXPRF S() = default;

            CONSTEXPRF S(S const &) = default;
            //CONSTEXPRF S(S &) = default; // sould not be user-declared to be trivial
            //CONSTEXPRF S(S const &&) { ; } // sould not be user-declared to be trivial
            CONSTEXPRF S(S &&) = default;

            CONSTEXPRF S & operator = (S const &) = default;
            //CONSTEXPRF S & operator = (S &) = default; // sould not be user-declared to be trivial
            //CONSTEXPRF S & operator = (S const &&) { return *this; } // sould not be user-declared to be trivial
            CONSTEXPRF S & operator = (S &&) = default;

            ~S() = default;

        };

        SA(std::is_trivial_v< S >);
        SA(std::is_literal_type_v< S >);
        SA(std::is_standard_layout_v< S >);
        SA(std::is_pod_v< S >);

        using U = V< S >;

        SA(std::is_trivial_v< U >);
        SA(std::is_literal_type_v< U >);
        SA(std::is_standard_layout_v< U >);
        SA(std::is_pod_v< U >);

        constexpr
        static
        bool
        run() noexcept
        {
            return true;
        }

    };

    // S - strictest type

    struct default_constructor
    {

        struct S
        {

            int i;
            constexpr S(int j) : i(j) { ; }

            CONSTEXPRF S() { ; }

            CONSTEXPRF S(S const &) = default;
            //CONSTEXPRF S(S &) = default; // sould not be user-declared to be trivially copyable
            //CONSTEXPRF S(S const &&) { ; } // sould not be user-declared to be trivially copyable
            CONSTEXPRF S(S &&) = default;

            CONSTEXPRF S & operator = (S const &) = default;
            //CONSTEXPRF S & operator = (S &) = default; // sould not be user-declared to be trivially copyable
            //CONSTEXPRF S & operator = (S const &&) { return *this; } // sould not be user-declared to be trivially copyable
            CONSTEXPRF S & operator = (S &&) = default;

            ~S() = default;

        };

        SA(std::is_default_constructible_v< S >);
        SA(!std::is_trivially_default_constructible_v< S >); // user-provided
        SA(std::is_trivially_copyable_v< S >);

        using U = V< S >;

        SA(std::is_default_constructible_v< U >);
        SA(!std::is_trivially_default_constructible_v< U >); // U is a union with at least one variant member with non-trivial default constructor
        SA(std::is_trivially_copyable_v< U >);

        constexpr
        static
        bool
        run() noexcept
        {
            return true;
        }

    };

    struct destructor
    {

        struct S
        {

            int i;
            constexpr S(int j) : i(j) { ; }

            CONSTEXPRF S() = default;

            CONSTEXPRF S(S const &) = default;
            CONSTEXPRF S(S &) = default;
            //CONSTEXPRF S(S const &&) { ; }
            CONSTEXPRF S(S &&) = default;

            CONSTEXPRF S & operator = (S const &) = default;
            CONSTEXPRF S & operator = (S &) = default;
            //CONSTEXPRF S & operator = (S const &&) { return *this; }
            CONSTEXPRF S & operator = (S &&) = default;

            ~S() { ; }

        };

        SA(std::is_destructible_v< S >);
        SA(!std::is_trivially_destructible_v< S >); // user-provided

        using U = V< S >;

        SA(std::is_destructible_v< U >);
        SA(!std::is_trivially_destructible_v< U >); // U is a union and has a variant member with non-trivial destructor

        constexpr
        static
        bool
        run() noexcept
        {
            return true;
        }

    };

    struct copy_constructor
    {

        struct S
        {

            int i;
            constexpr S(int j) : i(j) { ; }

            CONSTEXPRF S() = default;

            CONSTEXPRF S(S const &) { ; }
            CONSTEXPRF S(S &) = default;
            //CONSTEXPRF S(S const &&) { ; }
            CONSTEXPRF S(S &&) = default;

            CONSTEXPRF S & operator = (S const &) = default;
            CONSTEXPRF S & operator = (S &) = default;
            //CONSTEXPRF S & operator = (S const &&) { return *this; }
            CONSTEXPRF S & operator = (S &&) = default;

            ~S() = default;

        };

        using U = V< S >;
        SA(std::is_default_constructible_v< S >);
        SA(std::is_default_constructible_v< U >);
        SA(std::is_trivially_default_constructible_v< S >);
        SA(std::is_trivially_default_constructible_v< U >);

        SA(std::is_destructible_v< S >);
        SA(std::is_destructible_v< U >);
        SA(std::is_trivially_destructible_v< S >);
        SA(std::is_trivially_destructible_v< U >);

        SA(std::is_copy_constructible_v< S >);
        SA(!std::is_copy_constructible_v< U >);
        SA(is_vcopy_constructible_v< S >);
        SA(!is_vcopy_constructible_v< U >);

        SA(std::is_move_constructible_v< S >);
        SA(std::is_move_constructible_v< U >);
        SA(is_cmove_constructible_v< S >);
        SA(!is_cmove_constructible_v< U >);

        SA(!std::is_trivially_copy_constructible_v< S >);
        SA(!std::is_trivially_copy_constructible_v< U >);
        SA(!is_trivially_vcopy_constructible_v< S >);
        SA(!is_trivially_vcopy_constructible_v< U >);

        SA(std::is_trivially_move_constructible_v< S >);
        SA(std::is_trivially_move_constructible_v< U >);
        SA(!is_trivially_cmove_constructible_v< S >);
        SA(!is_trivially_cmove_constructible_v< U >);

        SA(std::is_copy_assignable_v< S >);
        SA(std::is_copy_assignable_v< U >);
        SA(is_vcopy_assignable_v< S >);
        SA(is_vcopy_assignable_v< U >);

        SA(std::is_move_assignable_v< S >);
        SA(std::is_move_assignable_v< U >);
        SA(is_cmove_assignable_v< S >);
        SA(is_cmove_assignable_v< U >);

        SA(std::is_trivially_copy_assignable_v< S >);
        SA(std::is_trivially_copy_assignable_v< U >);
        SA(!is_trivially_vcopy_assignable_v< S >);
        SA(is_trivially_vcopy_assignable_v< U >);

        SA(std::is_trivially_move_assignable_v< S >);
        SA(std::is_trivially_move_assignable_v< U >);
        SA(is_trivially_cmove_assignable_v< S >); // depends on whether cmove assignment operator user provided or not
        SA(is_trivially_cmove_assignable_v< U >);

        constexpr
        static
        bool
        run() noexcept
        {
            return true;
        }

    };

    constexpr
    static
    bool
    run() noexcept // just for implicit instantiation
    {
        SA(literal_type_invariants::run());
        SA(common_type_invariants ::run());
        SA(trivial                ::run());
        SA(default_constructor    ::run());
        SA(destructor             ::run());
        SA(copy_constructor       ::run());
        //SA(vcopy_constructor      ::run());
        //SA(move_constructor       ::run());
        //SA(copy_assignment        ::run());
        //SA(vcopy_assignment       ::run());
        //SA(move_assignment        ::run());
        return true;
    }

};

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

using ::versatile::is_active;
using ::versatile::forward_as;

template< template< typename ... > class wrapper = ::versatile::identity,
          template< typename ... > class variant = ::versatile::versatile >
struct check_trivially_copyable
{

    template< typename ...types >
    using V = variant< typename wrapper< types >::type... >;

    CONSTEXPRF
    static
    bool
    trivially_default_constructible() noexcept
    {
        struct S
        {
            CONSTEXPRF S() = default;
            CONSTEXPRF S(S const &) { ; }
            CONSTEXPRF S(S &) { ; }
            //CONSTEXPRF S(S const &&) { ; }
            CONSTEXPRF S(S &&) { ; }
            CONSTEXPRF S & operator = (S const &)  { return *this; }
            CONSTEXPRF S & operator = (S &)  { return *this; }
            //CONSTEXPRF S & operator = (S const &&) { return *this; }
            CONSTEXPRF S & operator = (S &&) { return *this; }
            ~S() DESTRUCTOR
        };
        struct B {};
        SA(std::is_trivially_default_constructible_v< S >);
        using U = V< S, B >;
        SA(std::is_trivially_default_constructible_v< U >);
        U v;
        CHECK (is_active< S >(v));
        return false;
    }

    CONSTEXPRF
    static
    bool
    trivially_copy_move_constructible() noexcept
    {
        { // copy constructor operator = (const &)
            struct S // strictest type
            {
                CONSTEXPRF S() { ; }
                CONSTEXPRF S(S const &) = default;
                CONSTEXPRF S(S &) { ; }
                //CONSTEXPRF S(S const &&) { ; }
                CONSTEXPRF S(S &&) { ; }
                CONSTEXPRF S & operator = (S const &) { return *this; }
                CONSTEXPRF S & operator = (S &) { return *this; }
                //CONSTEXPRF S & operator = (S const &&) { return *this; }
                CONSTEXPRF S & operator = (S &&) { return *this; }
                ~S() DESTRUCTOR
            };
            constexpr auto tcc = std::is_trivially_copy_constructible_v< S >;
            constexpr auto tmc = std::is_trivially_move_constructible_v< S >;
            constexpr auto cc = std::is_copy_constructible_v< S >;
            constexpr auto mc = std::is_move_constructible_v< S >;
            SA(cc);
            SA(mc);
            struct N {};
            {
                using U = variant< S, N >;
                SA(tcc == std::is_trivially_copy_constructible_v< U >);
                SA(tmc == std::is_trivially_move_constructible_v< U >);
                SA(std::is_copy_constructible_v< S >);
                SA(std::is_move_constructible_v< S >);
                {
                    U const v{N{}};
                    CHECK (is_active< N >(v));
                    U w{v};
                    CHECK (is_active< N >(w));
                }
                {
                    U const v{S{}};
                    CHECK (is_active< S >(v));
                    U w{v};
                    CHECK (is_active< S >(w));
                }
            }
            {
                using U = variant< N, S >;
                SA(tcc == std::is_trivially_copy_constructible_v< U >);
                SA(tmc == std::is_trivially_move_constructible_v< U >);
                SA(std::is_copy_constructible_v< S >);
                SA(std::is_move_constructible_v< S >);
                {
                    U const v{N{}};
                    CHECK (is_active< N >(v));
                    U w{v};
                    CHECK (is_active< N >(w));
                }
                {
                    U const v{S{}};
                    CHECK (is_active< S >(v));
                    U w{v};
                    CHECK (is_active< S >(w));
                }
            }
        }
#if 0
        { // copy constructor operator = (&)
            struct S // strictest type
            {
                CONSTEXPRF S() { ; }
                CONSTEXPRF S(S const &) { ; }
                CONSTEXPRF S(S &) = default;
                CONSTEXPRF S(S &&) { ; }
                CONSTEXPRF S & operator = (S const &) { return *this; }
                CONSTEXPRF S & operator = (S &) { return *this; }
                CONSTEXPRF S & operator = (S &&) { return *this; }
                ~S() DESTRUCTOR
            };
            constexpr auto tcc = std::is_trivially_copy_constructible_v< S >;
            constexpr auto tmc = std::is_trivially_move_constructible_v< S >;
            constexpr auto cc = std::is_copy_constructible_v< S >;
            constexpr auto mc = std::is_move_constructible_v< S >;
            SA(cc);
            SA(mc);
            struct N {};
            {
                using U = variant< S, N >;
                SA(tcc == std::is_trivially_copy_constructible_v< U >);
                SA(tmc == std::is_trivially_move_constructible_v< U >);
                SA(std::is_copy_constructible_v< S >);
                SA(std::is_move_constructible_v< S >);
                {
                    U v{N{}};
                    CHECK (is_active< N >(v));
                    U w{v};
                    CHECK (is_active< N >(w));
                }
                {
                    U v{S{}};
                    CHECK (is_active< S >(v));
                    U w{v};
                    CHECK (is_active< S >(w));
                }
            }
            {
                using U = variant< N, S >;
                SA(tcc == std::is_trivially_copy_constructible_v< U >);
                SA(tmc == std::is_trivially_move_constructible_v< U >);
                SA(std::is_copy_constructible_v< S >);
                SA(std::is_move_constructible_v< S >);
                {
                    U v{N{}};
                    CHECK (is_active< N >(v));
                    U w{v};
                    CHECK (is_active< N >(w));
                }
                {
                    U v{S{}};
                    CHECK (is_active< S >(v));
                    U w{v};
                    CHECK (is_active< S >(w));
                }
            }
        }
#endif
        { // move constructor operator = (&&)
            struct S // strictest type
            {
                CONSTEXPRF S() { ; }
                CONSTEXPRF S(S const &) { ; }
                CONSTEXPRF S(S &) { ; }
                CONSTEXPRF S(S &&) = default;
                CONSTEXPRF S & operator = (S const &) { return *this; }
                CONSTEXPRF S & operator = (S &) { return *this; }
                CONSTEXPRF S & operator = (S &&) { return *this; }
                ~S() DESTRUCTOR
            };
            constexpr auto tcc = std::is_trivially_copy_constructible_v< S >;
            constexpr auto tmc = std::is_trivially_move_constructible_v< S >;
            constexpr auto cc = std::is_copy_constructible_v< S >;
            constexpr auto mc = std::is_move_constructible_v< S >;
            SA(cc);
            SA(mc);
            struct N {};
            {
                using U = variant< S, N >;
                SA(tcc == std::is_trivially_copy_constructible_v< U >);
                SA(tmc == std::is_trivially_move_constructible_v< U >);
                SA(std::is_copy_constructible_v< S >);
                SA(std::is_move_constructible_v< S >);
                {
                    U v{N{}};
                    CHECK (is_active< N >(v));
                    U w{std::move(v)};
                    CHECK (is_active< N >(w));
                }
                {
                    U v{S{}};
                    CHECK (is_active< S >(v));
                    U w{std::move(v)};
                    CHECK (is_active< S >(w));
                }
            }
            {
                using U = variant< N, S >;
                SA(tcc == std::is_trivially_copy_constructible_v< U >);
                SA(tmc == std::is_trivially_move_constructible_v< U >);
                SA(std::is_copy_constructible_v< S >);
                SA(std::is_move_constructible_v< S >);
                {
                    U v{N{}};
                    CHECK (is_active< N >(v));
                    U w{std::move(v)};
                    CHECK (is_active< N >(w));
                }
                {
                    U v{S{}};
                    CHECK (is_active< S >(v));
                    U w{std::move(v)};
                    CHECK (is_active< S >(w));
                }
            }
        }/*
        { // conversion constructor (scalars)
            using U = variant< int, double >;
            SA(std::is_trivial_v< U >);
            constexpr U i = 1;
            ASSERT (get< int const & >(i) == 1);
            constexpr U d = -1.1;
            ASSERT (!(get< double const & >(d) < -1.1));
            ASSERT (!(-1.1 < get< double const & >(d)));
        }
        { // conversion constructor (aggregates)
            struct A { int i; };
            struct B { double d; };
            using U = V< A, B >;
            SA(std::is_trivial_v< U >);
            constexpr U i = A{1};
            ASSERT (get< A const & >(i).i == 1);
            constexpr U d = B{1.1};
            ASSERT (!(get< B const & >(d).d < 1.1));
            ASSERT (!(1.1 < get< B const & >(d).d));
        }*/
        return true;
    }

    CONSTEXPRF
    static
    bool
    trivially_copy_move_assignable() noexcept
    {
        { // really all operations are trivial operations
            struct A {};
            struct B {};
            using U = V< A, B >;
            SA(std::is_assignable_v< U &, A >);
            SA(std::is_assignable_v< U &, B >);
            SA(!std::is_trivially_assignable_v< U, A >);
            SA(!std::is_trivially_assignable_v< U, B >);
        }
        {
            using U = variant< int, char, double >;
            SA(std::is_trivial_v< U >);
            U v{1};
            CHECK (is_active< int >(v));
            CHECK (get< int & >(v) == 1);
            v = 'c';
            CHECK (is_active< char & >(v));
            CHECK (get< char & >(v) == 'c');
            v = -1.1;
            CHECK (is_active< double & >(v));
            CHECK (!(get< double & >(v) < -1.1));
            CHECK (!(-1.1 < get< double & >(v)));
            v = 2;
            CHECK (is_active< int & >(v));
            CHECK (get< int & >(v) == 2);
        }
        {
            struct A { int i = 1; };
            struct B { int i = 2; };
            struct C { int i = 3; };
            using U = V< A, B, C >;
            SA(!std::is_trivially_default_constructible_v< U >);
            SA(std::is_trivially_copyable_v< U >);
            { // conversion assignment
                U v{A{}};
                CHECK (is_active< A >(v));
                CHECK (get< A & >(v).i == 1);
                v = B{};
                CHECK (is_active< B & >(v));
                CHECK (get< B & >(v).i == 2);
                v = C{};
                CHECK (is_active< C & >(v));
                CHECK (get< C & >(v).i == 3);
                v = A{};
                CHECK (is_active< A & >(v));
                CHECK (get< A & >(v).i == 1);
            }
            { // copy-move assignment
                U v;
                CHECK (is_active< A >(v));
            }
        }
        {
            enum class E { A = 1 };
            SA(std::is_trivially_default_constructible_v< E >);
            constexpr E e{};
            SA(e != E::A); // not in domain space => it is better to prohibit using of enums
            SA(static_cast< std::underlying_type_t< E > >(e) == 0);
        }
        {
            enum class E { A, B };
            enum class F { C, D };
            using U = variant< E, F >;
            U v = E::B;
            CHECK (get< E >(v) == E::B);
            v = F::C;
            CHECK (get< F >(v) == F::C);
        }
        return true;
    }

    CONSTEXPRF
    static
    bool
    run() noexcept
    {
        ASSERT (trivially_copy_move_constructible());
        ASSERT (trivially_copy_move_assignable());
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
struct is_explicitly_convertible // akrzemi1's answer http://stackoverflow.com/a/16894048/1430927
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
            ASSERT (is_active< O >(v));
            ASSERT (get< O const & >(v) == state::default_constructed);
            U w;
            CHECK (is_active< O >(w));
            CHECK (get< O const & >(w) == state::default_constructed);
        }
        {
            struct A { CONSTEXPRF A(int i = 1) : j(i) { ; } int j; };
            using U = V< A >;
            SA(std::is_default_constructible_v< A >);
            CONSTEXPR U v{};
            ASSERT (is_active< A >(v));
            ASSERT (get< A const & >(v).j == 1);
        }
        {
            struct A {};
            struct B {};
            SA(std::is_default_constructible_v< A >);
            SA(std::is_default_constructible_v< B >);
            using U = V< A, B >;
            SA(std::is_default_constructible_v< U >);
            CONSTEXPR U v{};
            ASSERT (is_active< A >(v));
        }
        {
            struct A { CONSTEXPRF A(int) { ; } };
            struct B {};
            SA(!std::is_default_constructible_v< A >);
            SA(std::is_default_constructible_v< B >);
            using U = V< A, B >;
            SA(std::is_default_constructible_v< U >);
            CONSTEXPR U v{};
            ASSERT (is_active< B >(v));
        }
        {
            struct A {};
            struct B { CONSTEXPRF B(int) { ; } };
            SA(std::is_default_constructible_v< A >);
            SA(!std::is_default_constructible_v< B >);
            using U = V< A, B >;
            SA(std::is_default_constructible_v< U >);
            CONSTEXPR U v{};
            ASSERT (is_active< A >(v));
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
            SA(is_explicitly_convertible< U, A >::value);
            SA(is_explicitly_convertible< U, A const >::value);
            SA(!std::is_assignable_v< A &, U const >);
            SA(!std::is_assignable_v< A &, U >);
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
            SA(std::is_copy_assignable_v< U >);
            SA(std::is_move_assignable_v< U >);
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
            SA(std::is_assignable_v< U, A >);
            SA(std::is_assignable_v< U, B >);
        }
        { // altering active member
            struct A { int i = 1; };
            using U = V< A >;
            U v;
            CHECK (is_active< A >(v));
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
            SA(std::is_copy_constructible_v< U >);
            SA(std::is_move_constructible_v< U >);
        }
        {

        }
        return true;
    }

    CONSTEXPRF
    static
    bool
    emplace() noexcept
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
        ASSERT (emplace());
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
        return (base::template operator () < I..., J >() && ...); // rtl
        //return (base::template operator () < J, I... >() && ...); // ltr
    }

};

template< std::size_t ...indices, typename F >
CONSTEXPRF
enumerator< F, indices... >
make_enumerator(F && f)
{
    SA(0 < sizeof...(indices));
    SA(((0 < indices) && ...));
    return f;
}

using ::versatile::type_qualifier;

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
    SA(((type_qualifier_of< types > == type_qualifier::value) && ...));
    return {_stuff};
}

// variant - variant
// T - type generator
// M - multivisitor arity, N - number of alternative (bounded) types
template< template< std::size_t I > class T,
          template< typename ...types > class variant,
          std::size_t M = 2, std::size_t N = M >
class test_perferct_forwarding
{

    static CONSTEXPR std::size_t ref_count = (qualifier_id_end - qualifier_id_begin); // test only reference types

    template< typename >
    struct variants;

    template< std::size_t ...J >
    struct variants< std::index_sequence< J... > >
    {

        using variant_type = variant< T< J >... >;
        SA(N == sizeof...(J));

        variant_type variants_[N] = {T< J >{}...};

    };

    template< type_qualifier type_qual, std::size_t ...I >
    CONSTEXPRF
    static
    bool
    run(std::index_sequence< I... >) noexcept
    {
        SA(sizeof...(I) == M);
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

public :

    CONSTEXPRF
    static
    bool
    run() noexcept
    {
        CHECK (run< type_qualifier::value       >(std::make_index_sequence< M >{}));
        CHECK (run< type_qualifier::const_value >(std::make_index_sequence< M >{}));
        CHECK (run< type_qualifier::lref        >(std::make_index_sequence< M >{}));
        CHECK (run< type_qualifier::rref        >(std::make_index_sequence< M >{}));
        CHECK (run< type_qualifier::const_lref  >(std::make_index_sequence< M >{}));
        CHECK (run< type_qualifier::const_rref  >(std::make_index_sequence< M >{}));
        return true;
    }

};

} // namespace test_variant

#include <boost/variant.hpp>

namespace test_boost_variant
{

template< typename ...types >
struct variant_c
{

    using variant = boost::variant< types... >;

    variant_c(variant_c &) = default;
    variant_c(variant_c const &) = default;
    variant_c(variant_c &&) = default;

    constexpr
    variant_c(variant_c const && _rhs)
        : member_(std::move(_rhs.member_))
    { ; }

    template< typename ...arguments >
    constexpr
    variant_c(arguments &&... _arguments)
        : member_(std::forward< arguments >(_arguments)...)
    { ; }

    variant_c &
    operator = (variant_c const &) = default;
    variant_c &
    operator = (variant_c &) = default;
    variant_c &
    operator = (variant_c &&) = default;

    constexpr
    variant_c &
    operator = (variant_c const && _rhs)
    {
        member_ = std::move(_rhs.member_);
        return *this;
    }

    template< typename argument >
    constexpr
    variant_c &
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
is_active(variant_c< types... > const & v) noexcept
{
    return v.template active< type >();
}

template< typename type, typename ...types >
CONSTEXPRF
type
get(variant_c< types... > const & v) noexcept
{
    return static_cast< type >(static_cast< type & >(v));
}

template< typename type, typename ...types >
CONSTEXPRF
type
get(variant_c< types... > & v) noexcept
{
    return static_cast< type >(static_cast< type & >(v));
}

template< typename ...types >
struct variant_i
        : boost::variant< types... >
{

    using base = boost::variant< types... >;

    //using base::base; // seems there is wrong design of boost::variant constructor
    //using base::operator =;

    variant_i(variant_i &) = default;
    variant_i(variant_i const &) = default;
    variant_i(variant_i &&) = default;

    constexpr
    variant_i(variant_i const && _rhs)
        : base(std::move(_rhs.member_))
    { ; }

    template< typename ...arguments >
    constexpr
    variant_i(arguments &&... _arguments)
        : base(std::forward< arguments >(_arguments)...)
    { ; }

    variant_i &
    operator = (variant_i const &) = default;
    variant_i &
    operator = (variant_i &) = default;
    variant_i &
    operator = (variant_i &&) = default;

    constexpr
    variant_i &
    operator = (variant_i const && _rhs)
    {
        base::operator = (std::move(_rhs.member_));
        return *this;
    }

    template< typename argument >
    constexpr
    variant_i &
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
        return boost::get< type const & >(static_cast< variant_i::base const & >(*this));
    }

    template< typename type >
    explicit
    constexpr
    operator type & ()
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return boost::get< type & >(static_cast< variant_i::base & >(*this));
    }

};

template< typename type, typename ...types >
CONSTEXPRF
bool
is_active(variant_i< types... > const & v) noexcept
{
    return v.template active< type >();
}

template< typename type, typename ...types >
CONSTEXPRF
type
get(variant_i< types... > const & v) noexcept
{
    return static_cast< type >(static_cast< type & >(v));
}

template< typename type, typename ...types >
CONSTEXPRF
type
get(variant_i< types... > & v) noexcept
{
    return static_cast< type >(static_cast< type & >(v));
}

} // namespace test_boost_variant

namespace versatile
{

template< typename type >
struct unwrap_type< boost::recursive_wrapper< type > >
        : unwrap_type< type >
{

};

template< typename first, typename ...rest >
struct is_visitable< ::test_boost_variant::variant_i< first, rest... > >
        : std::true_type
{

};

template< typename first, typename ...rest >
struct is_visitable< ::test_boost_variant::variant_c< first, rest... > >
        : std::true_type
{

};

template< typename type >
struct unwrap_type< std::reference_wrapper< type > >
        : unwrap_type< type >
{

    SA(!std::is_const< type >{});

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
                using ::test_variant::check_trivially_copyable;
                ASSERT (check_trivially_copyable< identity,  versatile >::run());
                ASSERT (check_trivially_copyable< aggregate, versatile >::run());
            }
            {
                using ::test_variant::check_literal;
                ASSERT (check_literal< identity,  versatile >::run());
                ASSERT (check_literal< aggregate, versatile >::run());
            }
#if 0
            {
                using ::test_variant::test_perferct_forwarding;
                {
                    using ::test_variant::literal_type;
                    ASSERT (test_perferct_forwarding< literal_type, versatile, 2, 2 >::run());
                }
                {
                    using ::test_variant::common_type;
                    assert ((test_perferct_forwarding< common_type, versatile, 2, 2 >::run()));
                }
            }
#endif
        }
    }
    { // variant

    }
    { // boost::variant
        using ::test_boost_variant::variant_i;
        using ::test_boost_variant::variant_c;
#if 0
        struct L {};
        SA(std::is_literal_type< L >{});
        SA(!std::is_literal_type< ::boost::variant< L > >{});
#ifdef VERSATILE_RUNTIME
        using ::versatile::identity;
        using ::test_variant::aggregate;
        {
            using ::test_variant::check_literal;
            {
                ASSERT (check_literal< identity,  variant_i >::run());
                ASSERT (check_literal< aggregate, variant_i >::run());
            }
            {
                ASSERT (check_literal< identity,  variant_c >::run());
                ASSERT (check_literal< aggregate, variant_c >::run());
            }
        }
#endif
#endif
#if 0
        {
            using ::test_variant::test_perferct_forwarding;
            {
                using ::test_variant::literal_type;
                assert ((test_perferct_forwarding< literal_type, variant_i, 2, 2 >::run()));
                assert ((test_perferct_forwarding< literal_type, variant_c, 2, 2 >::run()));
            }
            {
                using ::test_variant::common_type;
                assert ((test_perferct_forwarding< common_type, variant_i, 2, 2 >::run()));
                assert ((test_perferct_forwarding< common_type, variant_c, 2, 2 >::run()));
            }
        }
#endif
    }
    { // eggs::variant

    }
    return EXIT_SUCCESS;
}

#if 0

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
            //SA(std::is_trivially_copy_constructible< V >{}); // ???
            //SA(std::is_trivially_move_constructible< V >{}); // ???
            SA(std::is_trivially_copy_assignable< V >{});
            SA(std::is_trivially_move_assignable< V >{});
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
            SA(!std::is_standard_layout< B >{});
            using V = versatile< A, B >;
            SA(!std::is_standard_layout< V >{});
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
            SA(!std::is_standard_layout< D >{});
            using V = versatile< A, B, C, D >;
            SA(!std::is_standard_layout< V >{});
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
            SA(V::width == 1, "V::width != 1");
            V v;
            SA(1 == variadic_index< V, int >{});
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
            SA(variadic_index< V, int >{} == 1);
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
            SA(std::is_same< int, decltype(invoke(v, 0)) >{});
            SA(std::is_same< bool, decltype(invoke(v, 0.0)) >{});
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
            SA(std::is_same< int, decltype(invoke(v, 0)) >{});
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
                SA(std::is_void< decltype(visit(l)()) >{});
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
        using V = variant_i< boost::recursive_wrapper< A >, B >;
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
        using V = variant_c< A, boost::recursive_wrapper< B > >;
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
        auto l = compose_visitors([] (int) { return 0; }, [] (double) { return 1; }, [] (auto x) { SA(std::is_same< decltype(x), versatile<> >{}); return 2; });
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
        SA(std::is_constructible< XY, XY >{});
        SA(!std::is_copy_constructible< XY >{});
        SA(std::is_move_constructible< XY >{});
        SA(!std::is_default_constructible< XY >{});

        using WXY = AW< XY >;
        SA(std::is_assignable< WXY &, XY >{});
        SA(std::is_constructible< WXY, XY >{});
        SA(!std::is_copy_constructible< WXY >{}); // mimic
        SA(std::is_move_constructible< WXY >{});
        SA(!std::is_default_constructible< WXY >{});
        WXY wxy{std::move(xy)};
        wxy = XY{};
        wxy = WXY{XY{}};

        using V = variant< WXY >;
        SA(std::is_assignable< V &, XY >{});
        SA(std::is_assignable< V &, WXY >{});
        SA(std::is_constructible< V, XY >{});
        SA(std::is_copy_constructible< V >{}); // lie
        SA(std::is_move_constructible< V >{});
        SA(std::is_default_constructible< V >{});
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
        SA(std::is_literal_type< V >{});

        struct visitor3
        {
            constexpr auto operator () (A, int i = 1) const { return 100 + i; }
            constexpr auto operator () (B, int i = 2) const { return 200 + i; }
            constexpr auto operator () (C, int i = 3) const { return 300 + i; }
        };
        SA(std::is_literal_type< visitor3 >{});

        // rrefs
        SA(visit(visitor3{}, V{A{}}) == 101);
        SA(visit(visitor3{}, V{B{}}) == 202);
        SA(visit(visitor3{}, V{C{}}) == 303);

        SA(visit(visitor3{}, V{A{}}, 10) == 110);
        SA(visit(visitor3{}, V{B{}}, 20) == 220);
        SA(visit(visitor3{}, V{C{}}, 30) == 330);

        // lrefs
        constexpr visitor3 visitor3_{};
        constexpr V a{A{}};
        constexpr V b{B{}};
        constexpr V c{C{}};

        SA(visit(visitor3_, a) == 101);
        SA(visit(visitor3_, b) == 202);
        SA(visit(visitor3_, c) == 303);

        SA(visit(visitor3_, a, 11) == 111);
        SA(visit(visitor3_, b, 22) == 222);
        SA(visit(visitor3_, c, 33) == 333);

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
        SA(std::is_literal_type< multivisitor3 >{});

        constexpr auto multivisitor3_ = visit(multivisitor3{});
        SA(multivisitor3_(a,   0, a) == 111);
        SA(multivisitor3_(a,   0, b) == 112);
        SA(multivisitor3_(a,   0, c) == 113);
        SA(multivisitor3_(b,   0, a) == 121);
        SA(multivisitor3_(b,   0, b) == 122);
        SA(multivisitor3_(b,   0, c) == 123);
        SA(multivisitor3_(c,   0, a) == 131);
        SA(multivisitor3_(c,   0, b) == 132);
        SA(multivisitor3_(c,   0, c) == 133);
        SA(multivisitor3_(a, 0.0, a) == 211);
        SA(multivisitor3_(a, 0.0, b) == 212);
        SA(multivisitor3_(a, 0.0, c) == 213);
        SA(multivisitor3_(b, 0.0, a) == 221);
        SA(multivisitor3_(b, 0.0, b) == 222);
        SA(multivisitor3_(b, 0.0, c) == 223);
        SA(multivisitor3_(c, 0.0, a) == 231);
        SA(multivisitor3_(c, 0.0, b) == 232);
        SA(multivisitor3_(c, 0.0, c) == 233);
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

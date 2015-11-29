#pragma once

#include "prologue.hpp"

#include <type_traits>
#include <utility>

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

    struct trivial
    {

        struct S
        {

            int i;
            S(int j) : i(j) { ; }

            S() = default;

            S(S const &) = default;
            //S(S &) = default; // ?sould not be user-declared to be trivial
            //S(S const &&) { ; } // ?sould not be user-declared/provided to be trivial
            S(S &&) = default;

            S & operator = (S const &) = default;
            //S & operator = (S &) = default; // ?sould not be user-declared to be trivial
            //S & operator = (S const &&) { return *this; } // ?sould not be user-declared/provided to be trivial
            S & operator = (S &&) = default;

            ~S() = default;

        };

        using U = V< S >;

        SA(std::is_trivial_v< S >);
        SA(std::is_trivial_v< U >);

        SA(std::is_standard_layout_v< S >);
        SA(std::is_standard_layout_v< U >);

        SA(std::is_literal_type_v< S >); // constexpr defaulted default c-tor
        SA(std::is_literal_type_v< U >);

        SA(std::is_pod_v< S >);
        SA(std::is_pod_v< U >);

        constexpr
        static
        bool
        run() noexcept
        {
            return true;
        }

    };

    struct trivially_copyable
    {

        struct S
        {

            int i;
            S(int j) : i(j) { ; }

            S() { ; }

            S(S const &) = default;
            //S(S &) = default; // ?sould not be user-declared to be trivially copyable
            //S(S const &&) { ; } // ?sould not be user-declared/provided to be trivially copyable
            S(S &&) = default;

            S & operator = (S const &) = default;
            //S & operator = (S &) = default; // ?sould not be user-declared to be trivially copyable
            //S & operator = (S const &&) { return *this; } // ?sould not be user-declared/provided to be trivially copyable
            S & operator = (S &&) = default;

            ~S() = default;

        };

        using U = V< S >;

        SA(!std::is_trivially_default_constructible_v< S >);
        SA(!std::is_trivially_default_constructible_v< U >);

        SA(std::is_trivially_copyable_v< S >);
        SA(std::is_trivially_copyable_v< U >);

        SA(std::is_standard_layout_v< S >);
        SA(std::is_standard_layout_v< U >);

        SA(!std::is_literal_type_v< S >);
        SA(!std::is_literal_type_v< U >);

        SA(!std::is_pod_v< S >);
        SA(!std::is_pod_v< U >);

        constexpr
        static
        bool
        run() noexcept
        {
            return true;
        }

    };

    struct default_constructor
    {

        struct S
        {

            int i;
            S(int j) : i(j) { ; }

            S() { ; }

            S(S const &) = default;
            S(S &) = default;
            //S(S const &&) { ; }
            S(S &&) = default;

            S & operator = (S const &) = default;
            S & operator = (S &) = default;
            //S & operator = (S const &&) { return *this; }
            S & operator = (S &&) = default;

            ~S() = default;

        };

        using U = V< S >;

        SA(std::is_default_constructible_v< S >);
        SA(std::is_default_constructible_v< U >);
        SA(!std::is_trivially_default_constructible_v< S >);
        SA(!std::is_trivially_default_constructible_v< U >);

        SA(std::is_destructible_v< S >);
        SA(std::is_destructible_v< U >);
        SA(std::is_trivially_destructible_v< S >);
        SA(std::is_trivially_destructible_v< U >);

        SA(std::is_copy_constructible_v< S >);
        SA(std::is_copy_constructible_v< U >);
        SA(is_vcopy_constructible_v< S >);
        SA(is_vcopy_constructible_v< U >);

        SA(std::is_move_constructible_v< S >);
        SA(std::is_move_constructible_v< U >);
        SA(is_cmove_constructible_v< S >);
        SA(is_cmove_constructible_v< U >);

        SA(std::is_trivially_copy_constructible_v< S >);
        SA(std::is_trivially_copy_constructible_v< U >);
        SA(!is_trivially_vcopy_constructible_v< S >);
        SA(is_trivially_vcopy_constructible_v< U >);

        SA(std::is_trivially_move_constructible_v< S >);
        SA(std::is_trivially_move_constructible_v< U >);
        SA(is_trivially_cmove_constructible_v< S >);
        SA(is_trivially_cmove_constructible_v< U >);

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
        SA(is_trivially_cmove_assignable_v< S >);
        SA(is_trivially_cmove_assignable_v< U >);

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
            S(int j) : i(j) { ; }

            S() = default;

            S(S const &) = default;
            S(S &) = default;
            //S(S const &&) { ; }
            S(S &&) = default;

            S & operator = (S const &) = default;
            S & operator = (S &) = default;
            //S & operator = (S const &&) { return *this; }
            S & operator = (S &&) = default;

            ~S() { ; }

        };

        using U = V< S >;

        SA(std::is_default_constructible_v< S >);
        SA(std::is_default_constructible_v< U >);
        SA(!std::is_trivially_default_constructible_v< S >);
        SA(!std::is_trivially_default_constructible_v< U >);

        SA(std::is_destructible_v< S >);
        SA(std::is_destructible_v< U >);
        SA(!std::is_trivially_destructible_v< S >);
        SA(!std::is_trivially_destructible_v< U >);

        SA(std::is_copy_constructible_v< S >);
        SA(std::is_copy_constructible_v< U >);
        SA(is_vcopy_constructible_v< S >);
        SA(is_vcopy_constructible_v< U >);

        SA(std::is_move_constructible_v< S >);
        SA(std::is_move_constructible_v< U >);
        SA(is_cmove_constructible_v< S >);
        SA(is_cmove_constructible_v< U >);

        SA(!std::is_trivially_copy_constructible_v< S >);
        SA(!std::is_trivially_copy_constructible_v< U >);
        SA(!is_trivially_vcopy_constructible_v< S >);
        SA(!is_trivially_vcopy_constructible_v< U >);

        SA(!std::is_trivially_move_constructible_v< S >);
        SA(!std::is_trivially_move_constructible_v< U >);
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
        SA(is_trivially_cmove_assignable_v< S >);
        SA(is_trivially_cmove_assignable_v< U >);

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
            S(int j) : i(j) { ; }

            S() = default;

            S(S const &) { ; }
            S(S &) = default;
            //S(S const &&) { ; }
            S(S &&) = default;

            S & operator = (S const &) = default;
            S & operator = (S &) = default;
            //S & operator = (S const &&) { return *this; }
            S & operator = (S &&) = default;

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
        SA(is_trivially_cmove_assignable_v< S >);
        SA(is_trivially_cmove_assignable_v< U >);

        constexpr
        static
        bool
        run() noexcept
        {
            return true;
        }

    };

    struct vcopy_constructor
    {

        struct S
        {

            int i;
            S(int j) : i(j) { ; }

            S() = default;

            S(S const &) = default;
            S(S &) { ; }
            //S(S const &&) { ; }
            S(S &&) = default;

            S & operator = (S const &) = default;
            S & operator = (S &) = default;
            //S & operator = (S const &&) { return *this; }
            S & operator = (S &&) = default;

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
        SA(std::is_copy_constructible_v< U >);
        SA(is_vcopy_constructible_v< S >);
        SA(is_vcopy_constructible_v< U >);

        SA(std::is_move_constructible_v< S >);
        SA(std::is_move_constructible_v< U >);
        SA(is_cmove_constructible_v< S >);
        SA(is_cmove_constructible_v< U >);

        SA(std::is_trivially_copy_constructible_v< S >);
        SA(std::is_trivially_copy_constructible_v< U >);
        SA(!is_trivially_vcopy_constructible_v< S >);
        SA(is_trivially_vcopy_constructible_v< U >);

        SA(std::is_trivially_move_constructible_v< S >);
        SA(std::is_trivially_move_constructible_v< U >);
        SA(is_trivially_cmove_constructible_v< S >);
        SA(is_trivially_cmove_constructible_v< U >);

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
        SA(is_trivially_cmove_assignable_v< S >);
        SA(is_trivially_cmove_assignable_v< U >);

        constexpr
        static
        bool
        run() noexcept
        {
            return true;
        }

    };

    struct move_constructor
    {

        struct S
        {

            int i;
            S(int j) : i(j) { ; }

            S() = default;

            S(S const &) = default;
            S(S &) = default;
            //S(S const &&) { ; }
            S(S &&) { ; }

            S & operator = (S const &) = default;
            S & operator = (S &) = default;
            //S & operator = (S const &&) { return *this; }
            S & operator = (S &&) = default;

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
        SA(std::is_copy_constructible_v< U >);
        SA(is_vcopy_constructible_v< S >);
        SA(is_vcopy_constructible_v< U >);

        SA(std::is_move_constructible_v< S >);
        SA(std::is_move_constructible_v< U >);
        SA(is_cmove_constructible_v< S >);
        SA(is_cmove_constructible_v< U >);

        SA(std::is_trivially_copy_constructible_v< S >);
        SA(std::is_trivially_copy_constructible_v< U >);
        SA(!is_trivially_vcopy_constructible_v< S >);
        SA(is_trivially_vcopy_constructible_v< U >);

        SA(!std::is_trivially_move_constructible_v< S >);
        SA(!std::is_trivially_move_constructible_v< U >);
        SA(is_trivially_cmove_constructible_v< S >);
        SA(is_trivially_cmove_constructible_v< U >);

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
        SA(is_trivially_cmove_assignable_v< S >);
        SA(is_trivially_cmove_assignable_v< U >);

        constexpr
        static
        bool
        run() noexcept
        {
            return true;
        }

    };

    struct cmove_constructor
    {

        struct S
        {

            int i;
            S(int j) : i(j) { ; }

            S() = default;

            S(S const &) = default;
            S(S &) = default;
            /*!*/S(S const &&) { ; }
            S(S &&) { ; }

            S & operator = (S const &) = default;
            S & operator = (S &) = default;
            //S & operator = (S const &&) { return *this; }
            S & operator = (S &&) = default;

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
        SA(std::is_copy_constructible_v< U >);
        SA(is_vcopy_constructible_v< S >);
        SA(is_vcopy_constructible_v< U >);

        SA(std::is_move_constructible_v< S >);
        SA(std::is_move_constructible_v< U >);
        SA(is_cmove_constructible_v< S >);
        SA(is_cmove_constructible_v< U >);

        SA(std::is_trivially_copy_constructible_v< S >);
        SA(std::is_trivially_copy_constructible_v< U >);
        SA(!is_trivially_vcopy_constructible_v< S >);
        SA(is_trivially_vcopy_constructible_v< U >);

        SA(!std::is_trivially_move_constructible_v< S >);
        SA(!std::is_trivially_move_constructible_v< U >);
        SA(!is_trivially_cmove_constructible_v< S >);
        SA(is_trivially_cmove_constructible_v< U >);

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
        SA(is_trivially_cmove_assignable_v< S >);
        SA(is_trivially_cmove_assignable_v< U >);

        constexpr
        static
        bool
        run() noexcept
        {
            return true;
        }

    };

    struct copy_assignment
    {

        struct S
        {

            int i;
            S(int j) : i(j) { ; }

            S() = default;

            S(S const &) = default;
            S(S &) = default;
            //S(S const &&) { ; }
            S(S &&) = default;

            S & operator = (S const &) { ; }
            S & operator = (S &) = default;
            //S & operator = (S const &&) { return *this; }
            S & operator = (S &&) = default;

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
        SA(std::is_copy_constructible_v< U >);
        SA(is_vcopy_constructible_v< S >);
        SA(is_vcopy_constructible_v< U >);

        SA(std::is_move_constructible_v< S >);
        SA(std::is_move_constructible_v< U >);
        SA(is_cmove_constructible_v< S >);
        SA(is_cmove_constructible_v< U >);

        SA(std::is_trivially_copy_constructible_v< S >);
        SA(std::is_trivially_copy_constructible_v< U >);
        SA(!is_trivially_vcopy_constructible_v< S >);
        SA(is_trivially_vcopy_constructible_v< U >);

        SA(std::is_trivially_move_constructible_v< S >);
        SA(std::is_trivially_move_constructible_v< U >);
        SA(is_trivially_cmove_constructible_v< S >);
        SA(is_trivially_cmove_constructible_v< U >);

        SA(std::is_copy_assignable_v< S >);
        SA(!std::is_copy_assignable_v< U >);
        SA(is_vcopy_assignable_v< S >);
        SA(!is_vcopy_assignable_v< U >);

        SA(std::is_move_assignable_v< S >);
        SA(std::is_move_assignable_v< U >);
        SA(is_cmove_assignable_v< S >);
        SA(!is_cmove_assignable_v< U >);

        SA(!std::is_trivially_copy_assignable_v< S >);
        SA(!std::is_trivially_copy_assignable_v< U >);
        SA(!is_trivially_vcopy_assignable_v< S >);
        SA(!is_trivially_vcopy_assignable_v< U >);

        SA(std::is_trivially_move_assignable_v< S >);
        SA(std::is_trivially_move_assignable_v< U >);
        SA(!is_trivially_cmove_assignable_v< S >);
        SA(!is_trivially_cmove_assignable_v< U >);

        constexpr
        static
        bool
        run() noexcept
        {
            return true;
        }

    };

    struct vcopy_assignment
    {

        struct S
        {

            int i;
            S(int j) : i(j) { ; }

            S() = default;

            S(S const &) = default;
            S(S &) = default;
            //S(S const &&) { ; }
            S(S &&) = default;

            S & operator = (S const &) = default;
            S & operator = (S &) { ; }
            //S & operator = (S const &&) { return *this; }
            S & operator = (S &&) = default;

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
        SA(std::is_copy_constructible_v< U >);
        SA(is_vcopy_constructible_v< S >);
        SA(is_vcopy_constructible_v< U >);

        SA(std::is_move_constructible_v< S >);
        SA(std::is_move_constructible_v< U >);
        SA(is_cmove_constructible_v< S >);
        SA(is_cmove_constructible_v< U >);

        SA(std::is_trivially_copy_constructible_v< S >);
        SA(std::is_trivially_copy_constructible_v< U >);
        SA(!is_trivially_vcopy_constructible_v< S >);
        SA(is_trivially_vcopy_constructible_v< U >);

        SA(std::is_trivially_move_constructible_v< S >);
        SA(std::is_trivially_move_constructible_v< U >);
        SA(is_trivially_cmove_constructible_v< S >);
        SA(is_trivially_cmove_constructible_v< U >);

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
        SA(is_trivially_cmove_assignable_v< S >);
        SA(is_trivially_cmove_assignable_v< U >);

        constexpr
        static
        bool
        run() noexcept
        {
            return true;
        }

    };

    struct move_assignment
    {

        struct S
        {

            int i;
            S(int j) : i(j) { ; }

            S() = default;

            S(S const &) = default;
            S(S &) = default;
            //S(S const &&) { ; }
            S(S &&) = default;

            S & operator = (S const &) = default;
            S & operator = (S &) = default;
            //S & operator = (S const &&) { return *this; }
            S & operator = (S &&) { ; }

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
        SA(std::is_copy_constructible_v< U >);
        SA(is_vcopy_constructible_v< S >);
        SA(is_vcopy_constructible_v< U >);

        SA(std::is_move_constructible_v< S >);
        SA(std::is_move_constructible_v< U >);
        SA(is_cmove_constructible_v< S >);
        SA(is_cmove_constructible_v< U >);

        SA(std::is_trivially_copy_constructible_v< S >);
        SA(std::is_trivially_copy_constructible_v< U >);
        SA(!is_trivially_vcopy_constructible_v< S >);
        SA(is_trivially_vcopy_constructible_v< U >);

        SA(std::is_trivially_move_constructible_v< S >);
        SA(std::is_trivially_move_constructible_v< U >);
        SA(is_trivially_cmove_constructible_v< S >);
        SA(is_trivially_cmove_constructible_v< U >);

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

        SA(!std::is_trivially_move_assignable_v< S >);
        SA(std::is_trivially_move_assignable_v< U >);
        SA(is_trivially_cmove_assignable_v< S >);
        SA(is_trivially_cmove_assignable_v< U >);

        constexpr
        static
        bool
        run() noexcept
        {
            return true;
        }

    };

    struct cmove_assignment
    {

        struct S
        {

            int i;
            S(int j) : i(j) { ; }

            S() = default;

            S(S const &) = default;
            S(S &) = default;
            //S(S const &&) { ; }
            S(S &&) = default;

            S & operator = (S const &) = default;
            S & operator = (S &) = default;
            S & operator = (S const &&) { return *this; }
            /*!*/S & operator = (S &&) { ; }

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
        SA(std::is_copy_constructible_v< U >);
        SA(is_vcopy_constructible_v< S >);
        SA(is_vcopy_constructible_v< U >);

        SA(std::is_move_constructible_v< S >);
        SA(std::is_move_constructible_v< U >);
        SA(is_cmove_constructible_v< S >);
        SA(is_cmove_constructible_v< U >);

        SA(std::is_trivially_copy_constructible_v< S >);
        SA(std::is_trivially_copy_constructible_v< U >);
        SA(!is_trivially_vcopy_constructible_v< S >);
        SA(is_trivially_vcopy_constructible_v< U >);

        SA(std::is_trivially_move_constructible_v< S >);
        SA(std::is_trivially_move_constructible_v< U >);
        SA(is_trivially_cmove_constructible_v< S >);
        SA(is_trivially_cmove_constructible_v< U >);

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

        SA(!std::is_trivially_move_assignable_v< S >);
        SA(std::is_trivially_move_assignable_v< U >);
        SA(!is_trivially_cmove_assignable_v< S >);
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
        SA(trivial                ::run());
        SA(trivially_copyable     ::run());
        SA(default_constructor    ::run());
        SA(destructor             ::run());
        SA(copy_constructor       ::run());
        SA(vcopy_constructor      ::run());
        SA(move_constructor       ::run());
        SA(cmove_constructor      ::run());
        SA(copy_assignment        ::run());
        SA(vcopy_assignment       ::run());
        SA(move_assignment        ::run());
        SA(cmove_assignment       ::run());
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

} // namespace test_variant

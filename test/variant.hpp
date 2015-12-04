#pragma once

#include "prologue.hpp"

#include <type_traits>
#include <utility>
#include <typeinfo>
#include <string>
#include <vector>
#include <deque>
#include <list>

namespace test_variant
{

template< typename type >
struct aggregate
        : ::versatile::identity< ::versatile::aggregate_wrapper< type > >
{

};

template< typename type >
struct recursive_wrapper
        : ::versatile::identity< ::versatile::recursive_wrapper< type > >
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

SA(!std::is_trivially_default_constructible_v< literal_type<> >);
SA(std::is_default_constructible_v< literal_type<> >);
SA(std::is_literal_type_v< literal_type<> >);
SA(std::is_trivially_copyable_v< literal_type<> >);

template< std::size_t I = 0 >
struct common_type
{

    std::size_t i = I;

    CONSTEXPRF
    operator std::size_t () const noexcept
    {
        return i;
    }

    common_type() { ; }
    common_type(common_type const & c) : i(c.i) { ; }
    common_type(common_type & c) : i(c.i) { ; }
    common_type(common_type && c) : i(c.i) { ; }
    common_type & operator = (common_type const & c) { i = c.i; return *this; }
    common_type & operator = (common_type & c) { i = c.i; return *this; }
    common_type & operator = (common_type && c) { i = c.i; return *this; }
    ~common_type() { i = ~std::size_t{}; }

};

SA(!std::is_literal_type_v< common_type<> >);
SA(!std::is_trivially_default_constructible_v< common_type<> >);
SA(!std::is_trivially_destructible_v< common_type<> >);
SA(!std::is_trivially_copy_constructible_v< common_type<> >);
SA(!std::is_trivially_move_constructible_v< common_type<> >);
SA(!std::is_trivially_copy_assignable_v< common_type<> >);
SA(!std::is_trivially_move_assignable_v< common_type<> >);
SA(std::is_default_constructible_v< common_type<> >);
SA(std::is_destructible_v< common_type<> >);
SA(std::is_copy_constructible_v< common_type<> >);
SA(std::is_move_constructible_v< common_type<> >);
SA(std::is_copy_assignable_v< common_type<> >);
SA(std::is_move_assignable_v< common_type<> >);

template< typename type >
constexpr bool is_vcopy_constructible_v = std::is_constructible_v< type, type & >;

template< typename type >
constexpr bool is_cmove_constructible_v = std::is_constructible_v< type, type const && >;

template< typename type >
constexpr bool is_vcopy_assignable_v = std::is_assignable_v< type &, type & >;

template< typename type >
constexpr bool is_cmove_assignable_v = std::is_assignable_v< type &, type const && >;

template< typename type >
constexpr bool is_trivially_vcopy_constructible_v = std::is_trivially_constructible_v< type, type & >;

template< typename type >
constexpr bool is_trivially_cmove_constructible_v = std::is_trivially_constructible_v< type, type const && >;

template< typename type >
constexpr bool is_trivially_vcopy_assignable_v = std::is_trivially_assignable_v< type &, type & >;

template< typename type >
constexpr bool is_trivially_cmove_assignable_v = std::is_trivially_assignable_v< type &, type const && >;

template< template< typename ... > class wrapper = ::versatile::identity,
          template< typename ... > class variant = ::versatile::versatile >
class check_invariants
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
        SA(!std::is_trivially_default_constructible_v< U >);
        SA(std::is_trivially_copyable_v< U >);

        SA(std::is_standard_layout_v< S >);
        SA(std::is_standard_layout_v< U >);

        SA(std::is_literal_type_v< S >); // constexpr defaulted default c-tor
        SA(std::is_literal_type_v< U >);

        SA(std::is_pod_v< S >);
        SA(!std::is_pod_v< U >);

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
        SA(!std::is_trivially_default_constructible_v< S >); // ! false
        SA(!std::is_trivially_default_constructible_v< U >); // !

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

        SA(!std::is_trivially_copy_constructible_v< S >); // ? false
        SA(!std::is_trivially_copy_constructible_v< U >); // ?
        SA(!is_trivially_vcopy_constructible_v< S >); // ?
        SA(!is_trivially_vcopy_constructible_v< U >); // ?

        SA(!std::is_trivially_move_constructible_v< S >); // ? false
        SA(!std::is_trivially_move_constructible_v< U >); // ?
        SA(!is_trivially_cmove_constructible_v< S >); // ?
        SA(!is_trivially_cmove_constructible_v< U >); // ?

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
        SA(!std::is_trivially_default_constructible_v< U >);

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
        SA(!is_trivially_vcopy_assignable_v< S >); // ?
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
            S & operator = (S &&) { return *this; }

            ~S() = default;

        };

        using U = V< S >;

        SA(std::is_default_constructible_v< S >);
        SA(std::is_default_constructible_v< U >);
        SA(std::is_trivially_default_constructible_v< S >);
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
            /*!*/S & operator = (S const &&) { return *this; }
            S & operator = (S &&) = default;

            ~S() = default;

        };

        using U = V< S >;

        SA(std::is_default_constructible_v< S >);
        SA(std::is_default_constructible_v< U >);
        SA(std::is_trivially_default_constructible_v< S >);
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

    template< typename C >
    struct container
    {

        using U = V< C >;

        SA(std::is_default_constructible_v< C >);
        SA(std::is_default_constructible_v< U >);
        SA(!std::is_trivially_default_constructible_v< C >);
        SA(!std::is_trivially_default_constructible_v< U >);

        SA(std::is_destructible_v< C >);
        SA(std::is_destructible_v< U >);
        SA(!std::is_trivially_destructible_v< C >);
        SA(!std::is_trivially_destructible_v< U >);

        SA(std::is_copy_constructible_v< C >);
        SA(!std::is_copy_constructible_v< U >);
        SA(is_vcopy_constructible_v< C >);
        SA(!is_vcopy_constructible_v< U >);

        SA(std::is_move_constructible_v< C >);
        SA(std::is_move_constructible_v< U >); // ?
        SA(is_cmove_constructible_v< C >);
        SA(!is_cmove_constructible_v< U >);

        SA(!std::is_trivially_copy_constructible_v< C >);
        SA(!std::is_trivially_copy_constructible_v< U >);
        SA(!is_trivially_vcopy_constructible_v< C >);
        SA(!is_trivially_vcopy_constructible_v< U >);

        SA(!std::is_trivially_move_constructible_v< C >);
        SA(!std::is_trivially_move_constructible_v< U >);
        SA(!is_trivially_cmove_constructible_v< C >);
        SA(!is_trivially_cmove_constructible_v< U >);

        SA(std::is_copy_assignable_v< C >);
        SA(!std::is_copy_assignable_v< U >);
        SA(is_vcopy_assignable_v< C >);
        SA(!is_vcopy_assignable_v< U >);

        SA(std::is_move_assignable_v< C >);
        SA(!std::is_move_assignable_v< U >);
        SA(is_cmove_assignable_v< C >);
        SA(!is_cmove_assignable_v< U >);

        SA(!std::is_trivially_copy_assignable_v< C >);
        SA(!std::is_trivially_copy_assignable_v< U >);
        SA(!is_trivially_vcopy_assignable_v< C >);
        SA(!is_trivially_vcopy_assignable_v< U >);

        SA(!std::is_trivially_move_assignable_v< C >);
        SA(!std::is_trivially_move_assignable_v< U >);
        SA(!is_trivially_cmove_assignable_v< C >);
        SA(!is_trivially_cmove_assignable_v< U >);

        constexpr
        static
        bool
        run() noexcept
        {
            return true;
        }

    };

public :

    constexpr
    static
    bool
    run() noexcept // just for implicit instantiation
    {
        { // all std::containers are identical
            using S = common_type<>;
            SA(container< std::string >::run());
            SA(container< std::vector< S > >::run());
            SA(container< std::deque< S > >::run());
            SA(container< std::list< S > >::run());
        }
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

template< typename from, typename to >
struct is_explicitly_convertible // akrzemi1's answer http://stackoverflow.com/a/16894048/1430927
        : std::bool_constant< (std::is_constructible_v< to, from > && !std::is_convertible< from, to >::value) >
{

};

using ::versatile::is_active;
using ::versatile::forward_as;
using ::versatile::get;
using ::versatile::in_place;

template< template< typename ... > class wrapper = ::versatile::identity,
          template< typename ... > class variant = ::versatile::versatile >
class check_trivial
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
            CONSTEXPRF S(S const &&) { ; }
            CONSTEXPRF S(S &&) { ; }
            CONSTEXPRF S & operator = (S const &)  { return *this; }
            CONSTEXPRF S & operator = (S &)  { return *this; }
            CONSTEXPRF S & operator = (S const &&) { return *this; }
            CONSTEXPRF S & operator = (S &&) { return *this; }
            //~S() DESTRUCTOR
        };
        SA(std::is_trivially_default_constructible_v< S >);
        struct N {};
        SA(std::is_trivially_default_constructible_v< N >);
        {
            using U = V< S, N >;
            SA(!std::is_trivially_default_constructible_v< U >);
            {
                CONSTEXPR U v{};
                ASSERT (is_active< S >(v));
            }
            {
                U v;
                CHECK (is_active< S >(v));
            }
        }
        {
            using U = V< N, S >;
            SA(!std::is_trivially_default_constructible_v< U >);
            {
                CONSTEXPR U v{};
                ASSERT (is_active< N >(v));
            }
            {
                U v;
                CHECK (is_active< N >(v));
            }
        }
        return true;
    }

    CONSTEXPRF
    static
    bool
    trivially_copy_move_constructible() noexcept
    {
        struct S
        {
            CONSTEXPRF S() { ; }
            CONSTEXPRF S(S const &) = default;
            CONSTEXPRF S(S &) = default;
            //CONSTEXPRF S(S const &&) { ; }
            CONSTEXPRF S(S &&) = default;
            CONSTEXPRF S & operator = (S const &) { return *this; }
            CONSTEXPRF S & operator = (S &) { return *this; }
            //CONSTEXPRF S & operator = (S const &&) { return *this; }
            CONSTEXPRF S & operator = (S &&) { return *this; }
            ~S() = default;
        };
        SA(std::is_trivially_copy_constructible_v< S >);
        SA(!is_trivially_vcopy_constructible_v< S >   ); // ?
        SA(std::is_trivially_move_constructible_v< S >);
        SA(is_trivially_cmove_constructible_v< S >    );
        SA(std::is_copy_constructible_v< S >          );
        SA(is_vcopy_constructible_v< S >              );
        SA(std::is_move_constructible_v< S >          );
        SA(is_cmove_constructible_v< S >              );
        struct N {};
        SA(std::is_trivial_v< N >);
        {
            using U = variant< S, N >;
            SA(std::is_trivially_copy_constructible_v< U >);
            SA(is_trivially_vcopy_constructible_v< U >);
            SA(std::is_trivially_move_constructible_v< U >);
            SA(is_trivially_cmove_constructible_v< U >);
            SA(std::is_copy_constructible_v< U >);
            SA(is_vcopy_constructible_v< U >);
            SA(std::is_move_constructible_v< U >);
            SA(is_cmove_constructible_v< U >);
            {
                CONSTEXPR U v{N{}};
                ASSERT (is_active< N >(v));
                CONSTEXPR U w{v};
                ASSERT (is_active< N >(w));
            }
            {
                CONSTEXPR U v{S{}};
                ASSERT (is_active< S >(v));
                CONSTEXPR U w{v};
                ASSERT (is_active< S >(w));
            }
            {
                U v{N{}};
                CHECK (is_active< N >(v));
                U const w{v};
                CHECK (is_active< N >(w));
            }
            {
                U v{S{}};
                CHECK (is_active< S >(v));
                U const w{v};
                CHECK (is_active< S >(w));
            }
            {
                CONSTEXPR U v{N{}};
                ASSERT (is_active< N >(v));
                CONSTEXPR U w{std::move(v)};
                ASSERT (is_active< N >(w));
            }
            {
                CONSTEXPR U v{S{}};
                ASSERT (is_active< S >(v));
                CONSTEXPR U w{std::move(v)};
                ASSERT (is_active< S >(w));
            }
            {
                U v{N{}};
                CHECK (is_active< N >(v));
                U const w{std::move(v)};
                CHECK (is_active< N >(w));
            }
            {
                U v{S{}};
                CHECK (is_active< S >(v));
                U const w{std::move(v)};
                CHECK (is_active< S >(w));
            }
        }
        {
            using U = variant< N, S >;
            SA(std::is_trivially_copy_constructible_v< U >);
            SA(is_trivially_vcopy_constructible_v< U >);
            SA(std::is_trivially_move_constructible_v< U >);
            SA(is_trivially_cmove_constructible_v< U >);
            SA(std::is_copy_constructible_v< U >);
            SA(is_vcopy_constructible_v< U >);
            SA(std::is_move_constructible_v< U >);
            SA(is_cmove_constructible_v< U >);
            {
                CONSTEXPR U v{N{}};
                ASSERT (is_active< N >(v));
                CONSTEXPR U w{v};
                ASSERT (is_active< N >(w));
            }
            {
                CONSTEXPR U v{S{}};
                ASSERT (is_active< S >(v));
                CONSTEXPR U w{v};
                ASSERT (is_active< S >(w));
            }
            {
                U v{N{}};
                CHECK (is_active< N >(v));
                U const w{v};
                CHECK (is_active< N >(w));
            }
            {
                U v{S{}};
                CHECK (is_active< S >(v));
                U const w{v};
                CHECK (is_active< S >(w));
            }
            {
                CONSTEXPR U v{N{}};
                ASSERT (is_active< N >(v));
                CONSTEXPR U w{std::move(v)};
                ASSERT (is_active< N >(w));
            }
            {
                CONSTEXPR U v{S{}};
                ASSERT (is_active< S >(v));
                CONSTEXPR U w{std::move(v)};
                ASSERT (is_active< S >(w));
            }
            {
                U v{N{}};
                CHECK (is_active< N >(v));
                U const w{std::move(v)};
                CHECK (is_active< N >(w));
            }
            {
                U v{S{}};
                CHECK (is_active< S >(v));
                U const w{std::move(v)};
                CHECK (is_active< S >(w));
            }
        }
        return true;
    }

    CONSTEXPRF
    static
    bool
    trivially_copy_move_assignable() noexcept
    {
        struct S
        {
            CONSTEXPRF S() { ; }
            CONSTEXPRF S(S const &) = default;
            CONSTEXPRF S(S &) = default;
            //CONSTEXPRF S(S const &&) { ; }
            CONSTEXPRF S(S &&) = default;
            CONSTEXPRF S & operator = (S const &) = default;
            CONSTEXPRF S & operator = (S &) = default;
            //CONSTEXPRF S & operator = (S const &&) { return *this; }
            CONSTEXPRF S & operator = (S &&) = default;
            //~S() DESTRUCTOR // clang bug: if destructor is user-declared and defaulted, then defaulted assignment operators become non-trivial and marked as non-constexpr
        };
        SA(std::is_trivially_copy_assignable_v< S >);
        SA(!is_trivially_vcopy_assignable_v< S >   ); // ?
        SA(std::is_trivially_move_assignable_v< S >);
        SA(is_trivially_cmove_assignable_v< S >    );
        SA(std::is_copy_assignable_v< S >          );
        SA(is_vcopy_assignable_v< S >              );
        SA(std::is_move_assignable_v< S >          );
        SA(is_cmove_assignable_v< S >              );
        struct N {};
        SA(std::is_trivial_v< N >);
        {
            using U = variant< S, N >;
            SA(std::is_trivially_copy_assignable_v< U >);
            SA(is_trivially_vcopy_assignable_v< U >);
            SA(std::is_trivially_move_assignable_v< U >);
            SA(is_trivially_cmove_assignable_v< U >);
            SA(std::is_copy_assignable_v< U >);
            SA(is_vcopy_assignable_v< U >);
            SA(std::is_move_assignable_v< U >);
            SA(is_cmove_assignable_v< U >);
            {
                CONSTEXPR U v{N{}};
                ASSERT (is_active< N >(v));
                U w;
                CHECK (is_active< S >(w));
                w = v;
                CHECK (is_active< N >(w));
            }
            {
                CONSTEXPR U v{S{}};
                ASSERT (is_active< S >(v));
                U w;
                CHECK (is_active< S >(w));
                w = v;
                CHECK (is_active< S >(w));
            }
            {
                U v{N{}};
                CHECK (is_active< N >(v));
                U w;
                CHECK (is_active< S >(w));
                w = v;
                CHECK (is_active< N >(w));
            }
            {
                U v{S{}};
                CHECK (is_active< S >(v));
                U w;
                CHECK (is_active< S >(w));
                w = v;
                CHECK (is_active< S >(w));
            }
            {
                CONSTEXPR U v{N{}};
                ASSERT (is_active< N >(v));
                U w;
                CHECK (is_active< S >(w));
                w = std::move(v);
                CHECK (is_active< N >(w));
            }
            {
                CONSTEXPR U v{S{}};
                ASSERT (is_active< S >(v));
                U w;
                CHECK (is_active< S >(w));
                w = std::move(v);
                CHECK (is_active< S >(w));
            }
            {
                U v{N{}};
                CHECK (is_active< N >(v));
                U w;
                CHECK (is_active< S >(w));
                w = std::move(v);
                CHECK (is_active< N >(w));
            }
            {
                U v{S{}};
                CHECK (is_active< S >(v));
                U w;
                CHECK (is_active< S >(w));
                w = std::move(v);
                CHECK (is_active< S >(w));
            }
        }
        {
            using U = variant< N, S >;
            SA(std::is_trivially_copy_assignable_v< U >);
            SA(is_trivially_vcopy_assignable_v< U >);
            SA(std::is_trivially_move_assignable_v< U >);
            SA(is_trivially_cmove_assignable_v< U >);
            SA(std::is_copy_assignable_v< U >);
            SA(is_vcopy_assignable_v< U >);
            SA(std::is_move_assignable_v< U >);
            SA(is_cmove_assignable_v< U >);
            {
                CONSTEXPR U v{N{}};
                ASSERT (is_active< N >(v));
                U w;
                CHECK (is_active< N >(w));
                w = v;
                CHECK (is_active< N >(w));
            }
            {
                CONSTEXPR U v{S{}};
                ASSERT (is_active< S >(v));
                U w;
                CHECK (is_active< N >(w));
                w = v;
                CHECK (is_active< S >(w));
            }
            {
                U v{N{}};
                CHECK (is_active< N >(v));
                U w;
                CHECK (is_active< N >(w));
                w = v;
                CHECK (is_active< N >(w));
            }
            {
                U v{S{}};
                CHECK (is_active< S >(v));
                U w;
                CHECK (is_active< N >(w));
                w = v;
                CHECK (is_active< S >(w));
            }
            {
                CONSTEXPR U v{N{}};
                ASSERT (is_active< N >(v));
                U w;
                CHECK (is_active< N >(w));
                w = std::move(v);
                CHECK (is_active< N >(w));
            }
            {
                CONSTEXPR U v{S{}};
                ASSERT (is_active< S >(v));
                U w;
                CHECK (is_active< N >(w));
                w = std::move(v);
                CHECK (is_active< S >(w));
            }
            {
                U v{N{}};
                CHECK (is_active< N >(v));
                U w;
                CHECK (is_active< N >(w));
                w = std::move(v);
                CHECK (is_active< N >(w));
            }
            {
                U v{S{}};
                CHECK (is_active< S >(v));
                U w;
                CHECK (is_active< N >(w));
                w = std::move(v);
                CHECK (is_active< S >(w));
            }
        }
        return true;
    }

    enum class state
    {
        never_used = 0,
        default_constructed,
        copy_constructed,
        vcopy_constructed,
        move_constructed,
        cmove_constructed,
        copy_assigned,
        vcopy_assigned,
        move_assigned,
        cmove_assigned,
        moved_from,
    };

    SA(state::never_used == state{});

    CONSTEXPRF
    static
    bool
    convertible() noexcept
    {
        struct S
        {
            state state_ = state::never_used;
            CONSTEXPRF S() : state_{state::default_constructed} { ; }
            CONSTEXPRF S(S const &) : state_{state::copy_constructed} { ; }
            CONSTEXPRF S(S &) : state_{state::vcopy_constructed} { ; }
            //CONSTEXPRF S(S const &&) { state_ = state::cmove_constructed; }
            CONSTEXPRF S(S && s) : state_{state::move_constructed} { s.state_ = state::moved_from; }
            CONSTEXPRF S & operator = (S const &) { state_ = state::copy_assigned; return *this; }
            CONSTEXPRF S & operator = (S &) { state_ = state::vcopy_assigned; return *this; }
            //CONSTEXPRF S & operator = (S const &&) { state_ = state::cmove_assigned; return *this; }
            CONSTEXPRF S & operator = (S && s) { state_ = state::move_assigned; s.state_ = state::moved_from; return *this; }
            ~S() DESTRUCTOR
        };
        SA(!std::is_trivially_copy_constructible_v< S >);
        SA(!is_trivially_vcopy_constructible_v< S >    );
        SA(!std::is_trivially_move_constructible_v< S >);
        SA(!is_trivially_cmove_constructible_v< S >    );
        SA(std::is_copy_constructible_v< S >           );
        SA(is_vcopy_constructible_v< S >               );
        SA(std::is_move_constructible_v< S >           );
        SA(is_cmove_constructible_v< S >               );
        SA(!std::is_trivially_copy_assignable_v< S >   );
        SA(!is_trivially_vcopy_assignable_v< S >       );
        SA(!std::is_trivially_move_assignable_v< S >   );
        SA(!is_trivially_cmove_assignable_v< S >       );
        SA(std::is_copy_assignable_v< S >              );
        SA(is_vcopy_assignable_v< S >                  );
        SA(std::is_move_assignable_v< S >              );
        SA(is_cmove_assignable_v< S >                  );
        {
            using U = variant< S >;
            SA(!std::is_trivially_copy_constructible_v< U >);
            SA(!is_trivially_vcopy_constructible_v< U >);
            SA(!std::is_trivially_move_constructible_v< U >);
            SA(!is_trivially_cmove_constructible_v< U >);
            SA(!std::is_copy_constructible_v< U >);
            SA(!is_vcopy_constructible_v< U >);
            SA(std::is_move_constructible_v< U >); // ?
            SA(!is_cmove_constructible_v< U >);
            SA(!std::is_trivially_copy_assignable_v< U >);
            SA(!is_trivially_vcopy_assignable_v< U >);
            SA(!std::is_trivially_move_assignable_v< U >);
            SA(!is_trivially_cmove_assignable_v< U >);
            SA(!std::is_copy_assignable_v< U >);
            SA(!is_vcopy_assignable_v< U >);
            SA(!std::is_move_assignable_v< U >);
            SA(!is_cmove_assignable_v< U >);
            SA(is_explicitly_convertible< U, S >::value);
            SA(is_explicitly_convertible< U const, S >::value);
            SA(is_explicitly_convertible< U &, S >::value);
            SA(is_explicitly_convertible< U const &, S >::value);
            {
                struct T {};
                SA(!is_explicitly_convertible< U, T >::value); // SFINAE-disabled conversions
                SA(!is_explicitly_convertible< U const, T >::value); // SFINAE-disabled conversions
                SA(!is_explicitly_convertible< U &, T >::value); // SFINAE-disabled conversions
                SA(!is_explicitly_convertible< U const &, T >::value); // SFINAE-disabled conversions
            }
            {
                CONSTEXPR U v{};
                ASSERT (is_active< S >(v));
                ASSERT (static_cast< S const & >(v).state_ == state::default_constructed);
                CONSTEXPR S s{v};
                ASSERT (static_cast< S const & >(v).state_ == state::default_constructed);
                ASSERT (s.state_ == state::copy_constructed);
            }
            {
                U v;
                CHECK (is_active< S >(v));
                CHECK (static_cast< S & >(v).state_ == state::default_constructed);
                S const s{v};
                CHECK (static_cast< S & >(v).state_ == state::default_constructed);
                CHECK (s.state_ == state::vcopy_constructed);
            }
            {
                CONSTEXPR U v{};
                ASSERT (is_active< S >(v));
                ASSERT (static_cast< S const & >(v).state_ == state::default_constructed);
                CONSTEXPR S l{std::move(v)};
                ASSERT (static_cast< S const & >(v).state_ == state::default_constructed);
                ASSERT (l.state_ == state::copy_constructed); // only lvalue-reference conversion operator currently is available
                CONSTEXPR S r = std::move(static_cast< S const & >(v));
                ASSERT (static_cast< S const & >(v).state_ == state::default_constructed);
                ASSERT (r.state_ == state::copy_constructed); // `const &` operator win
            }
            {
                U v;
                CHECK (is_active< S >(v));
                CHECK (static_cast< S & >(v).state_ == state::default_constructed);
                S const l{std::move(v)};
                CHECK (static_cast< S & >(v).state_ == state::default_constructed);
                CHECK (l.state_ == state::vcopy_constructed); // only lvalue-reference to const conversion operator currently is available
                S const r = std::move(static_cast< S & >(v));
                CHECK (static_cast< S & >(v).state_ == state::moved_from);
                CHECK (r.state_ == state::move_constructed);
            }
        }
        return true;
    }

    CONSTEXPRF
    static
    bool
    constructible() noexcept
    {
        struct S
        {
            state state_ = state::never_used;
            CONSTEXPRF S() : state_{state::default_constructed} { ; }
            CONSTEXPRF S(S const &) : state_{state::copy_constructed} { ; }
            CONSTEXPRF S(S &) : state_{state::vcopy_constructed} { ; }
            //CONSTEXPRF S(S const &&) { state_ = state::cmove_constructed; }
            CONSTEXPRF S(S && s) : state_{state::move_constructed} { s.state_ = state::moved_from; }
            CONSTEXPRF S & operator = (S const &) { state_ = state::copy_assigned; return *this; }
            CONSTEXPRF S & operator = (S &) { state_ = state::vcopy_assigned; return *this; }
            //CONSTEXPRF S & operator = (S const &&) { state_ = state::cmove_assigned; return *this; }
            CONSTEXPRF S & operator = (S && s) { state_ = state::move_assigned; s.state_ = state::moved_from; return *this; }
            ~S() DESTRUCTOR
        };
        SA(!std::is_trivially_copy_constructible_v< S >);
        SA(!is_trivially_vcopy_constructible_v< S >    ); // ?
        SA(!std::is_trivially_move_constructible_v< S >);
        SA(!is_trivially_cmove_constructible_v< S >    );
        SA(std::is_copy_constructible_v< S >           );
        SA(is_vcopy_constructible_v< S >               );
        SA(std::is_move_constructible_v< S >           );
        SA(is_cmove_constructible_v< S >               );
        SA(!std::is_trivially_copy_assignable_v< S >   );
        SA(!is_trivially_vcopy_assignable_v< S >       ); // ?
        SA(!std::is_trivially_move_assignable_v< S >   );
        SA(!is_trivially_cmove_assignable_v< S >       );
        SA(std::is_copy_assignable_v< S >              );
        SA(is_vcopy_assignable_v< S >                  );
        SA(std::is_move_assignable_v< S >              );
        SA(is_cmove_assignable_v< S >                  );
        struct N {};
        SA(std::is_trivial_v< N >);
        {
            using U = variant< S, N >;
            SA(!std::is_trivially_copy_constructible_v< U >);
            SA(!is_trivially_vcopy_constructible_v< U >); // ?
            SA(!std::is_trivially_move_constructible_v< U >);
            SA(!is_trivially_cmove_constructible_v< U >);
            SA(!std::is_copy_constructible_v< U >);
            SA(!is_vcopy_constructible_v< U >);
            SA(std::is_move_constructible_v< U >); // ?
            SA(!is_cmove_constructible_v< U >);
            SA(!std::is_trivially_copy_assignable_v< U >);
            SA(!is_trivially_vcopy_assignable_v< U >); // ?
            SA(!std::is_trivially_move_assignable_v< U >);
            SA(!is_trivially_cmove_assignable_v< U >);
            SA(!std::is_copy_assignable_v< U >);
            SA(!is_vcopy_assignable_v< U >);
            SA(!std::is_move_assignable_v< U >);
            SA(!is_cmove_assignable_v< U >);
            {
                CONSTEXPR S s{};
                ASSERT (s.state_ == state::default_constructed);
                CONSTEXPR U v{s};
                ASSERT (is_active< S >(v));
                ASSERT (static_cast< S const & >(v).state_ == state::copy_constructed);
                ASSERT (s.state_ == state::default_constructed);
            }
            {
                S s;
                CHECK (s.state_ == state::default_constructed);
                U v{s};
                CHECK (is_active< S >(v));
                CHECK (static_cast< S & >(v).state_ == state::vcopy_constructed);
                CHECK (s.state_ == state::default_constructed);
            }
            {
                CONSTEXPR S s{};
                ASSERT (s.state_ == state::default_constructed);
                CONSTEXPR U v{std::move(s)};
                ASSERT (is_active< S >(v));
                ASSERT (static_cast< S const & >(v).state_ == state::copy_constructed); // `const &` operator win
                ASSERT (s.state_ == state::default_constructed);
            }
            {
                S s;
                CHECK (s.state_ == state::default_constructed);
                U v{std::move(s)};
                CHECK (is_active< S >(v));
                CHECK (static_cast< S & >(v).state_ == state::move_constructed);
                CHECK (s.state_ == state::moved_from);
            }
        }
        {
            using U = variant< N, S >;
            SA(!std::is_trivially_copy_constructible_v< U >);
            SA(!is_trivially_vcopy_constructible_v< U >); // ?
            SA(!std::is_trivially_move_constructible_v< U >);
            SA(!is_trivially_cmove_constructible_v< U >);
            SA(!std::is_copy_constructible_v< U >);
            SA(!is_vcopy_constructible_v< U >);
            SA(std::is_move_constructible_v< U >); // ?
            SA(!is_cmove_constructible_v< U >);
            SA(!std::is_trivially_copy_assignable_v< U >);
            SA(!is_trivially_vcopy_assignable_v< U >); // ?
            SA(!std::is_trivially_move_assignable_v< U >);
            SA(!is_trivially_cmove_assignable_v< U >);
            SA(!std::is_copy_assignable_v< U >);
            SA(!is_vcopy_assignable_v< U >);
            SA(!std::is_move_assignable_v< U >);
            SA(!is_cmove_assignable_v< U >);
            {
                CONSTEXPR S s{};
                ASSERT (s.state_ == state::default_constructed);
                CONSTEXPR U v{s};
                ASSERT (is_active< S >(v));
                ASSERT (static_cast< S const & >(v).state_ == state::copy_constructed);
                ASSERT (s.state_ == state::default_constructed);
            }
            {
                S s;
                CHECK (s.state_ == state::default_constructed);
                U v{s};
                CHECK (is_active< S >(v));
                CHECK (static_cast< S & >(v).state_ == state::vcopy_constructed);
                CHECK (s.state_ == state::default_constructed);
            }
            {
                CONSTEXPR S s{};
                ASSERT (s.state_ == state::default_constructed);
                CONSTEXPR U v{std::move(s)};
                ASSERT (is_active< S >(v));
                ASSERT (static_cast< S const & >(v).state_ == state::copy_constructed); // `const &` operator win
                ASSERT (s.state_ == state::default_constructed);
            }
            {
                S s;
                CHECK (s.state_ == state::default_constructed);
                U v{std::move(s)};
                CHECK (is_active< S >(v));
                CHECK (static_cast< S & >(v).state_ == state::move_constructed);
                CHECK (s.state_ == state::moved_from);
            }
        }
        return true;
    }

    CONSTEXPRF
    static
    bool
    assignable() noexcept
    {
        struct S
        {
            state state_ = state::never_used;
            CONSTEXPRF S() : state_{state::default_constructed} { ; }
            CONSTEXPRF S(S const &) : state_{state::copy_constructed} { ; }
            CONSTEXPRF S(S &) : state_{state::vcopy_constructed} { ; }
            //CONSTEXPRF S(S const &&) { state_ = state::cmove_constructed; }
            CONSTEXPRF S(S && s) : state_{state::move_constructed} { s.state_ = state::moved_from; }
            CONSTEXPRF S & operator = (S const &) = default;
            CONSTEXPRF S & operator = (S &) = default;
            //CONSTEXPRF S & operator = (S const &&) { return *this; }
            CONSTEXPRF S & operator = (S &&) = default;
            //~S() DESTRUCTOR // clang bug: if destructor is user-declared and defaulted, then defaulted assignment operators become non-trivial and marked as non-constexpr
        };
        SA(!std::is_trivially_copy_constructible_v< S >);
        SA(!is_trivially_vcopy_constructible_v< S >    );
        SA(!std::is_trivially_move_constructible_v< S >);
        SA(!is_trivially_cmove_constructible_v< S >    );
        SA(std::is_copy_constructible_v< S >           );
        SA(is_vcopy_constructible_v< S >               );
        SA(std::is_move_constructible_v< S >           );
        SA(is_cmove_constructible_v< S >               );
        SA(std::is_trivially_copy_assignable_v< S >    );
        SA(!is_trivially_vcopy_assignable_v< S >       ); // ?
        SA(std::is_trivially_move_assignable_v< S >    );
        SA(is_trivially_cmove_assignable_v< S >        );
        SA(std::is_copy_assignable_v< S >              );
        SA(is_vcopy_assignable_v< S >                  );
        SA(std::is_move_assignable_v< S >              );
        SA(is_cmove_assignable_v< S >                  );
        struct N {};
        SA(std::is_trivial_v< N >);
        {
            using U = variant< S, N >;
            SA(!std::is_trivially_copy_constructible_v< U >);
            SA(!is_trivially_vcopy_constructible_v< U >); // ?
            SA(!std::is_trivially_move_constructible_v< U >);
            SA(!is_trivially_cmove_constructible_v< U >);
            SA(!std::is_copy_constructible_v< U >);
            SA(!is_vcopy_constructible_v< U >);
            SA(std::is_move_constructible_v< U >); // ?
            SA(!is_cmove_constructible_v< U >);
            SA(std::is_trivially_copy_assignable_v< U >);
            SA(is_trivially_vcopy_assignable_v< U >);
            SA(std::is_trivially_move_assignable_v< U >);
            SA(is_trivially_cmove_assignable_v< U >);
            SA(std::is_copy_assignable_v< U >);
            SA(is_vcopy_assignable_v< U >);
            SA(std::is_move_assignable_v< U >);
            SA(is_cmove_assignable_v< U >);
            {
                CONSTEXPR S s{};
                ASSERT (s.state_ == state::default_constructed);
                U v;
                CHECK (is_active< S >(v));
                v = s;
                CHECK (is_active< S >(v));
                CHECK (static_cast< S & >(v).state_ == state::copy_constructed);
                ASSERT (s.state_ == state::default_constructed);
            }
            {
                S s;
                CHECK (s.state_ == state::default_constructed);
                U v;
                CHECK (is_active< S >(v));
                v = s;
                CHECK (is_active< S >(v));
                CHECK (static_cast< S & >(v).state_ == state::vcopy_constructed);
                CHECK (s.state_ == state::default_constructed);
            }
            {
                CONSTEXPR S s{};
                ASSERT (s.state_ == state::default_constructed);
                U v;
                CHECK (is_active< S >(v));
                v = std::move(s);
                CHECK (is_active< S >(v));
                CHECK (static_cast< S & >(v).state_ == state::copy_constructed); // `const &` operator win
                ASSERT (s.state_ == state::default_constructed);
            }
            {
                S s;
                CHECK (s.state_ == state::default_constructed);
                U v;
                CHECK (is_active< S >(v));
                v = std::move(s);
                CHECK (is_active< S >(v));
                CHECK (static_cast< S & >(v).state_ == state::move_constructed);
                CHECK (s.state_ == state::moved_from);
            }
        }
        {
            using U = variant< N, S >;
            SA(!std::is_trivially_copy_constructible_v< U >);
            SA(!is_trivially_vcopy_constructible_v< U >);
            SA(!std::is_trivially_move_constructible_v< U >);
            SA(!is_trivially_cmove_constructible_v< U >);
            SA(!std::is_copy_constructible_v< U >);
            SA(!is_vcopy_constructible_v< U >);
            SA(std::is_move_constructible_v< U >); // ?
            SA(!is_cmove_constructible_v< U >);
            SA(std::is_trivially_copy_assignable_v< U >);
            SA(is_trivially_vcopy_assignable_v< U >);
            SA(std::is_trivially_move_assignable_v< U >);
            SA(is_trivially_cmove_assignable_v< U >);
            SA(std::is_copy_assignable_v< U >);
            SA(is_vcopy_assignable_v< U >);
            SA(std::is_move_assignable_v< U >);
            SA(is_cmove_assignable_v< U >);
            {
                CONSTEXPR S s{};
                ASSERT (s.state_ == state::default_constructed);
                U v;
                CHECK (is_active< N >(v));
                v = s;
                CHECK (is_active< S >(v));
                CHECK (static_cast< S & >(v).state_ == state::copy_constructed);
                ASSERT (s.state_ == state::default_constructed);
            }
            {
                S s;
                CHECK (s.state_ == state::default_constructed);
                U v;
                CHECK (is_active< N >(v));
                v = s;
                CHECK (is_active< S >(v));
                CHECK (static_cast< S & >(v).state_ == state::vcopy_constructed);
                CHECK (s.state_ == state::default_constructed);
            }
            {
                CONSTEXPR S s{};
                ASSERT (s.state_ == state::default_constructed);
                U v;
                CHECK (is_active< N >(v));
                v = std::move(s);
                CHECK (is_active< S >(v));
                CHECK (static_cast< S & >(v).state_ == state::copy_constructed); // `const &` operator win
                ASSERT (s.state_ == state::default_constructed);
            }
            {
                S s;
                CHECK (s.state_ == state::default_constructed);
                U v;
                CHECK (is_active< N >(v));
                v = std::move(s);
                CHECK (is_active< S >(v));
                CHECK (static_cast< S & >(v).state_ == state::move_constructed);
                CHECK (s.state_ == state::moved_from);
            }
        }
        return true;
    }

    CONSTEXPRF
    static
    bool
    in_place_constructible() noexcept
    {
        struct S
        {
            CONSTEXPR S(int) { ; }
        };
        struct N {};
        {
            using U = V< S, N >;
            struct X {};
            SA(std::is_constructible_v< U, in_place< 2 >, int >);
            SA(!std::is_constructible_v< U, in_place< 2 >, X >);
            //SA(!std::is_constructible_v< U, in_place< 2 >, void >);
            /*SA(std::is_constructible_v< U, in_place< 2 >, S >);
            SA(!std::is_constructible_v< U, in_place< 2 > >);
            SA(std::is_constructible_v< U, in_place< 1 > >);
            SA(std::is_constructible_v< U, in_place< 1 >, N >);*/
        }
        return true;
    }

public :

    CONSTEXPRF
    static
    bool
    run() noexcept
    {
        ASSERT (trivially_default_constructible());
        ASSERT (trivially_copy_move_constructible());
        ASSERT (trivially_copy_move_assignable());
        ASSERT (convertible()); // conversion
        ASSERT (constructible()); // conversion
        ASSERT (assignable()); // conversion
        ASSERT (in_place_constructible()); // conversion
        return true;
    }

};

template< template< typename ... > class wrapper = ::versatile::identity,
          template< typename ... > class variant = ::versatile::versatile >
class check_common
{

    template< typename ...types >
    using V = variant< typename wrapper< types >::type... >;

    enum class state
    {
        never_used = 0,
        default_constructed,
        copy_constructed,
        vcopy_constructed,
        move_constructed,
        cmove_constructed,
        copy_assigned,
        vcopy_assigned,
        move_assigned,
        cmove_assigned,
        moved_from,
        destructed,
    };

    SA(state::never_used == state{});

    static
    bool
    destructible() noexcept
    {
        struct A
        {
            state s_;
            A() : s_{state::default_constructed} { ; }
            A(A const &) { s_ = state::copy_constructed; }
            A(A &) { s_ = state::vcopy_constructed; }
            A(A && a) { s_ = state::move_constructed;  a.s_ = state::moved_from; }
            A & operator = (A const &) { s_ = state::copy_assigned; return *this; }
            A & operator = (A &) { s_ = state::vcopy_assigned; return *this; }
            A & operator = (A && a) { s_ = state::move_assigned;  a.s_ = state::moved_from; return *this; }
            std::type_info const * * d_;
            void set(std::type_info const * & _d) { assert(!_d); d_ = &_d; }
            ~A() { if (!!d_) *d_ = &typeid(A); }
        };
        struct B
        {
            state s_;
            B() : s_{state::default_constructed} { ; }
            B(B const &) { s_ = state::copy_constructed; }
            B(B &) { s_ = state::vcopy_constructed; }
            B(B && a) { s_ = state::move_constructed;  a.s_ = state::moved_from; }
            B & operator = (B const &) { s_ = state::copy_assigned; return *this; }
            B & operator = (B &) { s_ = state::vcopy_assigned; return *this; }
            B & operator = (B && a) { s_ = state::move_assigned;  a.s_ = state::moved_from; return *this; }
            std::type_info const * * d_;
            void set(std::type_info const * & _d) { assert(!_d); d_ = &_d; }
            ~B() { if (!!d_) *d_ = &typeid(B); }
        };
        {
            using U = V< A, B >;
            {
                std::type_info const * d = nullptr;
                {
                    U u;
                    CHECK (is_active< A >(u));
                    CHECK (static_cast< A & >(u).s_ == state::default_constructed);
                    static_cast< A & >(u).set(d);
                }
                CHECK (!!d);
                CHECK (*d == typeid(A));
            }
            {
                std::type_info const * d = nullptr;
                {
                    U u{in_place< 1 >{}};
                    CHECK (is_active< B >(u));
                    CHECK (static_cast< B & >(u).s_ == state::default_constructed);
                    static_cast< B & >(u).set(d);
                }
                CHECK (!!d);
                CHECK (*d == typeid(B));
            }
            {
                std::type_info const * d = nullptr;
                {
                    U u{in_place< 2 >{}};
                    CHECK (is_active< A >(u));
                    CHECK (static_cast< A & >(u).s_ == state::default_constructed);
                    static_cast< A & >(u).set(d);
                }
                CHECK (!!d);
                CHECK (*d == typeid(A));
            }
        }
        return true;
    }

public :

    static
    bool
    run() noexcept
    {
        assert (destructible());
        return true;
    }

};

} // namespace test_variant

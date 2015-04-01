#include "type_traits.hpp"
#include "recursive_wrapper.hpp"
#include "versatile.hpp"
#include "variant.hpp"
#include "visitation.hpp"

#include <string>
#include <array>
#include <utility>
#include <tuple>
#ifdef _DEBUG
#include <iostream>
#include <iomanip>
#endif

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <cstdlib>

#ifndef COLS
#define COLS 5
#endif

#ifndef ROWS
#define ROWS COLS
#endif

namespace
{

struct introspector
{

    template< typename ...types >
    std::string
    operator () (types...) const
    {
        return __PRETTY_FUNCTION__;
    }

};

template< std::size_t I >
struct T
{

};

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

template< typename L, typename ...R >
constexpr
bool
lvalueizer(L const & _lhs, R const &... _rhs) noexcept // Additional layer too avoid known clang++ bug #19917 https://llvm.org/bugs/show_bug.cgi?id=19917 make all parametres lvalues.
{
    using variant::apply_visitor;
    return (_lhs == apply_visitor(visitor{}, _rhs...));
}

template< std::size_t ...M, std::size_t ...N >
constexpr
bool
invoke(std::index_sequence< M... >, std::index_sequence< N... >) noexcept
{
    return lvalueizer(std::array< std::size_t, sizeof...(N) >{(N % sizeof...(M))...}, variant::variant< T< M >... >{T< (N % sizeof...(M)) >{}}...);
}

#pragma clang diagnostic pop

template< std::size_t M, std::size_t N = M >
constexpr
bool
test() noexcept
{
    return invoke(std::make_index_sequence< M >{}, std::make_index_sequence< N >{});
}

}

int
main()
{
    {
        using namespace variant;
        {
            using V0 = versatile<>;
            assert(!V0{}.is_active());

            using V1 = versatile< V0 >;
            assert(V1{}.which() == 1);
            assert((index< V1, V0 > == 1));

            using V2 = versatile< V1, V0 >;
            assert(V2{}.which() == 2);
            assert((index< V2, V1 > == 2));

            using V3 = versatile< V2, V1, V0 >;
            assert(V3{}.which() == 3);
            assert((index< V3, V2 > == 3));

            using V4 = versatile< V3, V2, V1, V0 >;
            assert(V4{}.which() == 4);
            assert((index< V4, V3 > == 4));

            using V5 = versatile< V4, V3, V2, V1, V0 >;
            assert(V5{}.which() == 5);
            assert((index< V5, V4 > == 5));
            static_assert((std::is_same< V4, at< V5, 5 > >{}), "!");
        }
        {
            using V = versatile< int, float, double, long double >;
            assert(V{}.which() == 4);
            assert(V{0}.which() == 4);
            assert(V{1.0f}.which() == 3);
            assert(V{2.0}.which() == 2);
            assert(V{3.0L}.which() == 1);
        }
        {
            using V = versatile< int, float, double, long double >;
            assert(V().which() == 4);
            assert(V(0).which() == 4);
            assert(V(1.0f).which() == 3);
            assert(V(2.0).which() == 2);
            assert(V(3.0L).which() == 1);
        }
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-member-function"
            struct A { explicit A(int, int) {} };
#pragma clang diagnostic pop
            struct B { explicit B(int, double) {} };
            using V = versatile< B, A, int >;
            V v0{1};
            assert((v0.which() == index< V, int >));
            V v1{1, 2};
            assert((v1.which() == index< V, B >)); // !
            V v2{1, 2.0};
            assert((v2.which() == index< V, B >));
        }
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunneeded-member-function"
            struct A { ~A() noexcept(true) {} };
            struct B { ~B() noexcept(false) {} };
#pragma clang diagnostic pop
            using VA = versatile< A >;
            using VB = versatile< B >;
            using VAB = versatile< A, B >;
            static_assert(std::is_nothrow_destructible< A >{}, "!");
            static_assert(!std::is_nothrow_destructible< B >{}, "!");
            static_assert(std::is_nothrow_destructible< VA >{}, "!");
            static_assert(!std::is_nothrow_destructible< VB >{}, "!");
            static_assert(!std::is_nothrow_destructible< VAB >{}, "!");
        }
        {
            struct A { A() {} A & operator = (A const &) = default; A & operator = (A &) = delete; };
            using V = versatile< A >;
            V v;
            A const a{};
            v = a;
        }
        {
            struct A {};
            struct B {};
            struct L
            {

                int operator () (A) { return 100; }
                int operator () (B) { return 200; }

            };
            L v;
            using V = variant< A, B >;
            V a{A{}};
            assert(a.apply_visitor(v) == 100);
            V b{B{}};
            assert(b.apply_visitor(v) == 200);
        }
        {
            using V = versatile< int >;
            V a = +1;
            V b = -1;
            assert(static_cast< int & >(a) == +1);
            assert(static_cast< int & >(b) == -1);
            a.swap(b);
            assert(static_cast< int & >(a) == -1);
            assert(static_cast< int & >(b) == +1);
            swap(a, b);
            assert(static_cast< int & >(a) == +1);
            assert(static_cast< int & >(b) == -1);
        }
        {
            struct A {};
            struct B {};
            struct C {};
            struct D {};
            using AD = variant< A, D >;
            using BA = variant< B, A >;
            using CB = variant< C, B >;
            using DC = variant< D, C >;
            AD ad;
            BA ba;
            CB cb;
            DC dc;
            A a;
            B b;
            C c;
            D d;
            introspector introspector_;
            assert(apply_visitor(introspector_, ad, ba, cb, dc) == introspector_(a, b, c, d));
            AD da{D{}};
            BA ab{A{}};
            CB bc{B{}};
            DC cd{C{}};
            assert(apply_visitor(introspector_, da, ab, bc, cd) == introspector_(D{}, A{}, B{}, C{}));
        }
        {
            struct A;
            struct V { variant< int, recursive_wrapper< A > > v; };
            struct A { V v; };
            A a{};
            assert(a.v.v.which() == 2);
            std::ignore = static_cast< int const & >(a.v.v);
            std::ignore = static_cast< int & >(a.v.v);
            //std::ignore = static_cast< int const && >(v.v.v);
            //std::ignore = static_cast< int && >(v.v.v);
            A b{{{a}}};
            assert(b.v.v.which() == b.v.v.template index< A >());
            b.v.v = 1;
            assert(static_cast< int & >(b.v.v) == 1);
            //std::ignore = static_cast< A const && >(w.v.v);
            //std::ignore = static_cast< A && >(w.v.v);
        }
    }
    {
        using namespace variant;
        {
            struct A {};
            struct B {};
            struct visitor
            {
                int operator () (A, int i = 0) const { return 10 + i; }
                int operator () (B, int i = 0) const { return 20 + i; }
            };
            visitor visitor_;
            using DB = variant< A, B >;
            DB db_{B{}};
            assert((20 == apply_visitor(visitor_, db_)));
            assert((db_.apply_visitor(visitor_, -1) == 19));
            db_ = A{};
            assert(db_.apply_visitor(visitor_) == 10);
            assert((db_.apply_visitor(visitor_, 2) == 12));
            auto v = apply_visitor(visitor_);
            db_ = B{};
            assert(v(db_) == 20);
            int i = 5;
            assert(v(db_, i) == 25);
            db_ = A{};
            assert(v(db_) == 10);
            assert(v(db_, i) == 15);
        }
        {
            struct A { A() = delete; };
            struct B {};
            using DB = variant< A, B >;
            DB db;
            assert(db.which() == 1);
        }
    }
    {
        assert((test< ROWS, COLS >())); // 9 seconds (Release build) for COLS=5 ROWS=5 on Intel(R) Xeon(R) CPU E5-1650 0 @ 3.20GHz
    }
    return EXIT_SUCCESS;
}

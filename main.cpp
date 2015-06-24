#include "versatile/versatile.hpp"

#include <string>
#include <array>
#include <utility>
#include <tuple>
#include <functional>
#include <sstream>
#if defined(DEBUG) || defined(_DEBUG)
#include <iostream>
#include <iomanip>
#endif

#include <cstdlib>

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

#ifndef COLS
#define COLS 5
#endif

#ifndef ROWS
#define ROWS COLS
#endif

namespace test
{

using namespace versatile;

namespace test_traits
{

struct A {};
struct B {};
static_assert(std::is_same< copy_cv_reference_t<          A         , B >,          B          >{});
static_assert(std::is_same< copy_cv_reference_t<          A const   , B >,          B const    >{});
static_assert(std::is_same< copy_cv_reference_t< volatile A         , B >, volatile B          >{});
static_assert(std::is_same< copy_cv_reference_t< volatile A const   , B >, volatile B const    >{});
static_assert(std::is_same< copy_cv_reference_t<          A        &, B >,          B        & >{});
static_assert(std::is_same< copy_cv_reference_t<          A const  &, B >,          B const  & >{});
static_assert(std::is_same< copy_cv_reference_t< volatile A        &, B >, volatile B        & >{});
static_assert(std::is_same< copy_cv_reference_t< volatile A const  &, B >, volatile B const  & >{});
static_assert(std::is_same< copy_cv_reference_t<          A       &&, B >,          B       && >{});
static_assert(std::is_same< copy_cv_reference_t<          A const &&, B >,          B const && >{});
static_assert(std::is_same< copy_cv_reference_t< volatile A       &&, B >, volatile B       && >{});
static_assert(std::is_same< copy_cv_reference_t< volatile A const &&, B >, volatile B const && >{});

}

struct introspector
{

    template< typename ...types >
    std::string
    operator () (types...) const
    {
        return __PRETTY_FUNCTION__;
    }

};

struct visitor0
{

    using R = std::array< bool, 3 >;

    template< typename type >
    constexpr
    R
    operator () (type &&) const
    {
        return {{true, std::is_const< std::remove_reference_t< type > >::value, std::is_lvalue_reference< type >::value}};
    }

    template< typename type >
    constexpr
    R
    operator () (type &&)
    {
        return {{false, std::is_const< std::remove_reference_t< type > >::value, std::is_lvalue_reference< type >::value}};
    }

};

struct visitor1
{

    using R = std::array< bool, 4 >;

    template< typename type >
    constexpr
    R
    operator () (type &&) const &
    {
        return {{true, true, std::is_const< std::remove_reference_t< type > >::value, std::is_lvalue_reference< type >::value}};
    }

    template< typename type >
    constexpr
    R
    operator () (type &&) &
    {
        return {{false, true, std::is_const< std::remove_reference_t< type > >::value, std::is_lvalue_reference< type >::value}};
    }

    template< typename type >
    constexpr
    R
    operator () (type &&) &&
    {
        return {{false, false, std::is_const< std::remove_reference_t< type > >::value, std::is_lvalue_reference< type >::value}};
    }

};

struct visitor2
{
    template< typename ...types >
    constexpr
    auto
    operator () (types &&... _values) const
    {
        return std::tuple_cat(std::make_tuple(true), std::make_tuple(std::tie(typeid(std::forward< types >(_values))), std::is_const< std::remove_reference_t< types > >::value, std::is_lvalue_reference< types >::value)...);
    }

    template< typename ...types >
    constexpr
    auto
    operator () (types &&... _values)
    {
        return std::tuple_cat(std::make_tuple(false), std::make_tuple(std::tie(typeid(std::forward< types >(_values))), std::is_const< std::remove_reference_t< types > >::value, std::is_lvalue_reference< types >::value)...);
    }

};

struct visitor3
{

    using R = std::tuple< bool, bool, bool, bool >;

    template< typename type >
    constexpr
    R
    operator () (type &&) const &
    {
        return std::make_tuple(true, true, std::is_const< std::remove_reference_t< type > >::value, std::is_lvalue_reference< type >::value);
    }

    template< typename type >
    constexpr
    R
    operator () (type &&) &
    {
        return std::make_tuple(false, true, std::is_const< std::remove_reference_t< type > >::value, std::is_lvalue_reference< type >::value);
    }

    template< typename type >
    constexpr
    R
    operator () (type &&) const &&
    {
        return std::make_tuple(true, false, std::is_const< std::remove_reference_t< type > >::value, std::is_lvalue_reference< type >::value);
    }

    template< typename type >
    constexpr
    R
    operator () (type &&) &&
    {
        return std::make_tuple(false, false, std::is_const< std::remove_reference_t< type > >::value, std::is_lvalue_reference< type >::value);
    }

};

// value_or
template< typename lhs, typename rhs >
std::enable_if_t< (is_variant< std::remove_reference_t< lhs > >{} && !is_variant< std::remove_reference_t< rhs > >{}), std::remove_reference_t< rhs > >
operator || (lhs && _lhs, rhs && _rhs) noexcept
{
    using result_type = std::remove_reference_t< rhs >;
    if (_lhs.template active< result_type >()) {
        return static_cast< result_type >(std::forward< lhs >(_lhs));
    } else {
        return std::forward< rhs >(_rhs);
    }
}

template< typename lhs, typename rhs >
std::enable_if_t< (!is_variant< std::remove_reference_t< lhs > >{} && is_variant< std::remove_reference_t< rhs > >{}), lhs >
operator || (lhs && _lhs, rhs && _rhs) noexcept
{
    return (std::forward< rhs >(_rhs) || std::forward< lhs >(_lhs));
}

template< typename lhs, typename rhs >
std::enable_if_t< (is_variant< std::remove_reference_t< lhs > >{} && is_variant< std::remove_reference_t< rhs > >{}) >
operator || (lhs && _lhs, rhs && _rhs) = delete;

template< typename type >
using same_t = type;

template< typename variadic, typename type >
struct variadic_index;

template< template< typename ...types > class variadic, typename type, typename ...types >
struct variadic_index< variadic< types... >, type >
        : index_by_type< type, types... >
{

};

template< typename F, std::size_t ...indices >
struct enumerator;

template< typename F >
struct enumerator< F >
{

    constexpr
    enumerator(F && _f)
        : f(std::forward< F >(_f))
    { ; }

    template< std::size_t ...I >
    constexpr
    bool
    operator () () const
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

    constexpr
    enumerator(F && _f)
        : base(std::forward< F >(_f))
    { ; }

    template< std::size_t ...I >
    constexpr
    bool
    operator () () const
    {
        return enumerator::template operator () < I... >(std::make_index_sequence< first >{}); // ltr
    }

    template< std::size_t ...I, std::size_t ...J >
    constexpr
    bool
    operator () (std::index_sequence< J... >) const
    {
        return (base::template operator () < I..., J >() && ...); // rtl, `< J, I... >` - ltr
    }

};

template< std::size_t ...indices, typename F >
constexpr
enumerator< F, indices... >
make_enumerator(F && _f)
{
    static_assert(0 < sizeof...(indices));
    static_assert(((0 < indices) && ...));
    return std::forward< F >(_f);
}

template< std::size_t I >
struct T
{

    constexpr
    operator std::size_t () const noexcept
    {
        return I;
    }

};

template< std::size_t M >
struct pair
{

    std::array< type_qualifiers, (1 + M) > qual_ids;
    std::array< std::size_t, (1 + M) > type_ids;

    constexpr
    bool
    operator == (pair const & _rhs) const
    {
        return (qual_ids == _rhs.qual_ids) && (type_ids == _rhs.type_ids);
    }

    constexpr
    std::size_t
    size() const
    {
        return (1 + M);
    }

};

template< std::size_t M >
struct multivisitor
{

    using result_type = pair< M >;

    constexpr
    std::size_t
    which() const
    {
        return 0;
    }

    static constexpr std::size_t count = M;

    template< typename ...types >
    constexpr
    result_type
    operator () (types &&... _values) & noexcept
    {
        //static_assert(M == sizeof...(types));
        //static_assert(!(is_variant< types >{} || ...));
        return {{{get_type_qualifier< multivisitor & >, get_type_qualifier< types && >...}}, {{M, _values...}}};
    }

    template< typename ...types >
    constexpr
    result_type
    operator () (types &&... _values) const & noexcept
    {
        return {{{get_type_qualifier< multivisitor const & >, get_type_qualifier< types && >...}}, {{M, _values...}}};
    }

    template< typename ...types >
    constexpr
    result_type
    operator () (types &&... _values) && noexcept
    {
        return {{{get_type_qualifier< multivisitor && >, get_type_qualifier< types && >...}}, {{M, _values...}}};
    }

    template< typename ...types >
    constexpr
    result_type
    operator () (types &&... _values) const && noexcept
    {
        return {{{get_type_qualifier< multivisitor const && >, get_type_qualifier< types && >...}}, {{M, _values...}}};
    }

    template< typename ...types >
    constexpr
    result_type
    operator () (types &&... _values) volatile & noexcept
    {
        return {{{get_type_qualifier< volatile multivisitor & >, get_type_qualifier< types && >...}}, {{M, _values...}}};
    }

    template< typename ...types >
    constexpr
    result_type
    operator () (types &&... _values) volatile const & noexcept
    {
        return {{{get_type_qualifier< volatile multivisitor const & >, get_type_qualifier< types && >...}}, {{M, _values...}}};
    }

    template< typename ...types >
    constexpr
    result_type
    operator () (types &&... _values) volatile && noexcept
    {
        return {{{get_type_qualifier< volatile multivisitor && >, get_type_qualifier< types && >...}}, {{M, _values...}}};
    }

    template< typename ...types >
    constexpr
    result_type
    operator () (types &&... _values) volatile const && noexcept
    {
        return {{{get_type_qualifier< volatile multivisitor const && >, get_type_qualifier< types && >...}}, {{M, _values...}}};
    }

};

static constexpr std::size_t qualifier_id_begin = static_cast< std::size_t >(get_type_qualifier< void * & >);
static constexpr std::size_t qualifier_id_end = static_cast< std::size_t >(get_type_qualifier< void * volatile >);

template< typename ...types >
struct fusor
{

    std::tuple< types *... > const & stuff_;

    template< std::size_t ...Q, std::size_t ...K >
    bool
    operator () (std::index_sequence< Q... >, std::index_sequence< K... >) const
    {
        auto const lhs = visit(static_cast< add_qualifier_t< static_cast< type_qualifiers >(qualifier_id_begin + Q), types > >(*std::get< K >(stuff_))...);
        static_assert(sizeof...(types) == lhs.size());
        pair< (sizeof...(types) - 1) > const rhs = {{{static_cast< type_qualifiers >(qualifier_id_begin + Q)...}}, {{(std::get< K >(stuff_)->count - 1 - std::get< K >(stuff_)->which())...}}};
        if (lhs == rhs) {
            return false;
        }
        return true;
    }

    template< std::size_t ...Q >
    bool
    operator () (std::index_sequence< Q... >) const
    {
        return operator () (std::index_sequence< Q... >{}, std::index_sequence_for< types... >{});
    }

};

template< typename ...types >
fusor< types... >
make_fusor(std::tuple< types *... > const & _stuff)
{
    static_assert(((get_type_qualifier< types > == type_qualifiers::value) && ...));
    return {_stuff};
}

template< std::size_t M, std::size_t N = M > // M - multivisitor arity, N - number of alternative (bounded) types
class test_perferct_forwarding
{

    template< std::size_t ...J >
    static
    auto
    generate_variants(std::index_sequence< J... >)
    {
        using variant_type = variant< T< J >... >;
        return std::array< variant_type, N >{{T< J >{}...}};
    }

    using variant_type = typename decltype(generate_variants(std::make_index_sequence< N >{}))::value_type;

    static_assert(N == variant_type::count);

    template< std::size_t ...I >
    bool
    operator () (std::index_sequence< I... >) const
    {
        static_assert(sizeof...(I) == M);
        std::size_t indices[M];
        for (std::size_t & m : indices) {
            m = 0;
        }
        multivisitor< M > mv;
        auto variants = generate_variants(std::make_index_sequence< N >{});
        auto permutation_ = std::make_tuple(&mv, &variants[indices[I]]...);
        auto const fusor_ = make_fusor(permutation_);
        constexpr std::size_t ref_count = (qualifier_id_end - qualifier_id_begin); // test only reference types
        auto const enumerator_ = make_enumerator< ref_count, (I, ref_count)... >(fusor_);
        for (;;) {
            permutation_ = std::make_tuple(&mv, &variants[indices[I]]...);
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

    bool
    operator () () const
    {
        return operator () (std::make_index_sequence< M >{});
    }

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

template< std::size_t ...M, std::size_t ...N >
constexpr
bool
invoke(std::index_sequence< M... >, std::index_sequence< N... >) noexcept
{
    return (std::array< std::size_t, sizeof...(N) >{(N % sizeof...(M))...} == visit(visitor{}, variant< T< M >... >{T< (N % sizeof...(M)) >{}}...));
}

#pragma clang diagnostic pop

template< std::size_t M, std::size_t N = M >
constexpr
bool
hard() noexcept
{
    return invoke(std::make_index_sequence< M >{}, std::make_index_sequence< N >{});
}

}

int
main()
{
    using namespace test;
    {
        using namespace versatile;
        {
            using V = versatile< double, int >;
            V w{1};
            V v = std::move(w);
            assert(static_cast< int >(v) == 1);
            w = 2;
            assert(static_cast< int >(w) == 2);
            v = std::move(w);
            assert(static_cast< int >(v) == 2);
            static_assert(std::is_trivially_copy_assignable< V >{});
            static_assert(std::is_trivially_move_assignable< V >{});
            static_assert(!std::is_trivially_copy_constructible< V >{});
            static_assert(!std::is_trivially_move_constructible< V >{});
        }
        {
            using V = versatile< std::true_type, std::false_type >;
            V t;
            assert(static_cast< std::true_type >(t) == std::true_type{});
            V f{std::false_type{}};
            assert(static_cast< std::false_type >(f) == std::false_type{});
        }
        {
            using V = versatile< int >;
            V v{2};
            assert(static_cast< int >(v) == 2);
            v = versatile<>{}; // no op
            assert(static_cast< int >(v) == 2);
            V w{versatile<>{}}; // equivalent to default construction
            assert(static_cast< int >(w) == int{});
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
            using V = variant< int >;
            static_assert(V::count == 1, "V::count != 1");
            V v;
            static_assert(0 == variadic_index< V, int >{});
            assert(v.which() == 0);
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
            v.swap(w);
            assert(static_cast< int >(w) == 223);
            assert(static_cast< int >(v) == 222);
            V u = w;
            assert(static_cast< int >(u) == 223);
            assert(w.which() == 0);
            static_assert(variadic_index< V, int >{} == 0);
            assert(u.which() == 0);
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
            d.swap(j);
            assert(d.active< int >());
            assert(j.active< float >());
        }
        {
            struct A { A() = delete; };
            struct B {};
            using V =  variant< A, B >;
            V v;
            assert(v.active< B >());
        }
        {
            struct A {};
            struct B;
            using V = variant< A, B >;
            struct B {};
            V v;
            assert(v.active< A >());
            v = B{};
            assert(v.active< B >());
        }
        { // incomplete
            struct A {};
            struct B;
            using V = variant< A, recursive_wrapper< B > >;
            V v;
            assert(v.active< A >());
            struct B {};
            v = B{};
            assert(v.active< B >());
        }
        { // recursive
            struct R;
            struct A {};
            using V = variant< A, recursive_wrapper< R > >;
            V v;
            assert(v.active< A >());
            struct R { V v; };
            v = R{};
            assert(v.active< R >());
        }
        { // exact
            using V = variant< int, double >;
            assert(V{} == V{});
            assert(V{} == int{});
            assert(int{} == V{});
            assert(V{1} == 1);
            assert(1 == V{1});
            assert(V{2.0} == 2.0);
            assert(2.0 == V{2.0});
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
            V x{std::experimental::in_place, 1};
            V y{std::experimental::in_place, 2};
            y = std::move(x);
            assert(x == A{0});
            assert(y == A{1});
        }
        {
            struct A { A(int &, int) {} };
            struct B { B(int const &, int) {} };
            using V = variant< A, B >;
            V v{std::experimental::in_place, 1, 2};
            assert(v.active< B >());
            int i{1};
            V w{std::experimental::in_place, i, 2};
            assert(w.active< A >());
        }
        {
            variant< char const * > v;
            assert(static_cast< char const * >(v) == nullptr);
        }
        {
            struct A {};
            struct B { B(B &&) = default; B(B const &) = delete; B & operator = (B &&) = default; void operator = (B const &) = delete; };
            variant< A, B > v;
            assert(v.active< A >());
            v = B{};
            assert(v.active< B >());
        }
        {
            struct A {};
            using V = variant< A >;
            visitor0 p_;
            visitor0 const cp_{};
            V v;
            V const cv;
            using B3 = std::array< bool, 3 >;
            assert((v.visit(p_)    == B3{{false, false, true }}));
            assert((v.visit(cp_)   == B3{{true,  false, true }}));
            assert((cv.visit(p_)   == B3{{false, true,  true }}));
            assert((cv.visit(cp_)  == B3{{true,  true,  true }}));
            assert((V().visit(p_)  == B3{{false, false, false}}));
            assert((V().visit(cp_) == B3{{true,  false, false}}));
        }
        {
            struct A {};
            using V = variant< A >;
            visitor1 p_;
            visitor1 const cp_{};
            V v;
            V const cv{};
            using B4 = std::array< bool, 4 >;
            assert((v.visit(p_)           == B4{{false, true,  false, true }}));
            assert((v.visit(cp_)          == B4{{true,  true,  false, true }}));
            assert((v.visit(visitor1{})   == B4{{false, false, false, true }}));
            assert((cv.visit(p_)          == B4{{false, true,  true,  true }}));
            assert((cv.visit(cp_)         == B4{{true,  true,  true,  true }}));
            assert((cv.visit(visitor1{})  == B4{{false, false, true,  true }}));
            assert((V().visit(p_)         == B4{{false, true,  false, false}}));
            assert((V().visit(cp_)        == B4{{true,  true,  false, false}}));
            assert((V().visit(visitor1{}) == B4{{false, false, false, false}}));
        }
        { // multivisitation with forwarding
            struct A {};
            struct B {};
            using V = variant< A, B >;
            visitor2 v_;
            visitor2 const c_{};
            assert(visit(v_, V{B{}}) == std::make_tuple(false, std::tie(typeid(B)), false, false));
            assert(visit(c_, V{B{}}) == std::make_tuple(true, std::tie(typeid(B)), false, false));
            assert(visit(visitor2{}, V{B{}}) == std::make_tuple(false, std::tie(typeid(B)), false, false));
            assert(visit(v_, V{}, V{B{}}) == std::make_tuple(false, std::tie(typeid(A)), false, false, std::tie(typeid(B)), false, false));
            assert(visit(c_, V{}, V{B{}}) == std::make_tuple(true, std::tie(typeid(A)), false, false, std::tie(typeid(B)), false, false));
            assert(visit(visitor2{}, V{}, V{B{}}) == std::make_tuple(false, std::tie(typeid(A)), false, false, std::tie(typeid(B)), false, false));
            // forwarding
            enum class op { eq, lt, gt, nge };
            assert(visit(v_, V{}, op::eq, V{B{}}) == std::make_tuple(false, std::tie(typeid(A)), false, false, std::tie(typeid(op)), false, false, std::tie(typeid(B)), false, false));
            assert(visit(c_, V{}, op::eq, V{B{}}) == std::make_tuple(true, std::tie(typeid(A)), false, false, std::tie(typeid(op)), false, false, std::tie(typeid(B)), false, false));
            assert(visit(visitor2{}, V{}, op::eq, V{B{}}) == std::make_tuple(false, std::tie(typeid(A)), false, false, std::tie(typeid(op)), false, false, std::tie(typeid(B)), false, false));
        }
        { // delayed visitation
            visitor2 v_;
            visitor2 const c_{};

            auto d0 = make_visitor(v_);
            auto const d1 = make_visitor(v_);
            auto d2 = make_visitor(c_);
            auto const d3 = make_visitor(c_);
            auto d4 = make_visitor(visitor2{});
            auto const d5 = make_visitor(visitor2{});

            struct A {};
            struct B {};
            using V = variant< A, B >;
            V v;
            V const c(B{});

            assert(d0(v) ==   std::make_tuple(false, std::tie(typeid(A)), false, true ));
            assert(d0(c) ==   std::make_tuple(false, std::tie(typeid(B)), true,  true ));
            assert(d0(V{}) == std::make_tuple(false, std::tie(typeid(A)), false, false));

            assert(d1(v) ==   std::make_tuple(false, std::tie(typeid(A)), false, true ));
            assert(d1(c) ==   std::make_tuple(false, std::tie(typeid(B)), true,  true ));
            assert(d1(V{}) == std::make_tuple(false, std::tie(typeid(A)), false, false));

            assert(d2(v) ==   std::make_tuple(true,  std::tie(typeid(A)), false, true ));
            assert(d2(c) ==   std::make_tuple(true,  std::tie(typeid(B)), true,  true ));
            assert(d2(V{}) == std::make_tuple(true,  std::tie(typeid(A)), false, false));

            assert(d3(v) ==   std::make_tuple(true,  std::tie(typeid(A)), false, true ));
            assert(d3(c) ==   std::make_tuple(true,  std::tie(typeid(B)), true,  true ));
            assert(d3(V{}) == std::make_tuple(true,  std::tie(typeid(A)), false, false));

            assert(d4(v) ==   std::make_tuple(false, std::tie(typeid(A)), false, true ));
            assert(d4(c) ==   std::make_tuple(false, std::tie(typeid(B)), true,  true ));
            assert(d4(V{}) == std::make_tuple(false, std::tie(typeid(A)), false, false));

            // to use mutable visitor it is mandatory to have non-const delayed visitor instance
            assert(d5(v) ==   std::make_tuple(true,  std::tie(typeid(A)), false, true ));
            assert(d5(c) ==   std::make_tuple(true,  std::tie(typeid(B)), true,  true ));
            assert(d5(V{}) == std::make_tuple(true,  std::tie(typeid(A)), false, false));
        }
        { // delayed visitation
            visitor3 v_;
            visitor3 const cv_{};

            auto d = make_visitor(v_);
            auto const cd = make_visitor(v_);
            auto dcv = make_visitor(cv_);
            auto const cdcv = make_visitor(cv_);
            auto dmv = make_visitor(visitor3{});
            auto const cdmv = make_visitor(visitor3{});

            struct A {};
            using V = variant< A >;
            V v;
            V const cv{};

            assert(d(v)                           == std::make_tuple(false, true,  false, true ));
            assert(cd(v)                          == std::make_tuple(false, true,  false, true )); // ! true, true, false, true
            assert(make_visitor(v_)(v)           == std::make_tuple(false, false, false, true ));

            assert(dcv(v)                         == std::make_tuple(true,  true,  false, true ));
            assert(cdcv(v)                        == std::make_tuple(true,  true,  false, true ));
            assert(make_visitor(cv_)(v)          == std::make_tuple(true,  false, false, true )); // if visitor have not `const &&` version of `operator ()`, then `const &` chosen

            assert(dmv(v)                         == std::make_tuple(false, true,  false, true ));
            assert(cdmv(v)                        == std::make_tuple(true,  true,  false, true ));
            assert(make_visitor(visitor3{})(v)   == std::make_tuple(false, false, false, true ));

            assert(d(cv)                          == std::make_tuple(false, true,  true,  true ));
            assert(cd(cv)                         == std::make_tuple(false, true,  true,  true )); // ! true, true, true, true
            assert(make_visitor(v_)(cv)          == std::make_tuple(false, false, true,  true ));

            assert(dcv(cv)                        == std::make_tuple(true,  true,  true,  true ));
            assert(cdcv(cv)                       == std::make_tuple(true,  true,  true,  true ));
            assert(make_visitor(cv_)(cv)         == std::make_tuple(true,  false, true,  true ));

            assert(dmv(cv)                        == std::make_tuple(false, true,  true,  true ));
            assert(cdmv(cv)                       == std::make_tuple(true,  true,  true,  true ));
            assert(make_visitor(visitor3{})(cv)  == std::make_tuple(false, false, true,  true ));

            assert(d(V{})                         == std::make_tuple(false, true,  false, false));
            assert(cd(V{})                        == std::make_tuple(false, true,  false, false)); // ! true, true, false, false
            assert(make_visitor(v_)(V{})         == std::make_tuple(false, false, false, false));

            assert(dcv(V{})                       == std::make_tuple(true,  true,  false, false));
            assert(cdcv(V{})                      == std::make_tuple(true,  true,  false, false));
            assert(make_visitor(cv_)(V{})        == std::make_tuple(true,  false, false, false));

            assert(dmv(V{})                       == std::make_tuple(false, true,  false, false));
            assert(cdmv(V{})                      == std::make_tuple(true,  true,  false, false));
            assert(make_visitor(visitor3{})(V{}) == std::make_tuple(false, false, false, false));
        }/*
        {
            struct A {};
            struct B {};
            struct C {};
            using V = variant< A, B, C >;
            static_assert((std::is_same< variant< std::reference_wrapper< A >, std::reference_wrapper< B >, std::reference_wrapper< C > >, V::wrap< std::reference_wrapper > >{}), "!");
            static_assert((std::is_same< std::tuple< A, B, C >, V::wrap< same_t, std::tuple > >{}), "!");
            static_assert((std::is_same< std::tuple< std::reference_wrapper< A >, std::reference_wrapper< B >, std::reference_wrapper< C > >, V::wrap< std::reference_wrapper, std::tuple > >{}), "!");
        }*/
        {
            using V = variant< int, double >;
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
        {
            auto const l0 = [] (auto const &) { return 0; };
            auto const l1 = [] (auto &) { return 1; };
            auto const l2 = [] (auto const &&) { return 2; };
            auto const l3 = [] (auto &&) { return 3; };
            {
                struct A {};
                using V = variant< A >;
                V v;
                V const c{};
                auto const l = compose_visitors(l0, l1, l2, l3);
                assert(0 == visit(l, c));
                assert(1 == visit(l, v));
                assert(2 == visit(l, std::move(c)));
                assert(3 == visit(l, std::move(v)));
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
                A av{0};
                A const ac{1};
                {
                    auto const l = compose_visitors(l0, l1, l2, l3);
                    assert(0 == l(ac));
                    assert(1 == l(av));
                    assert(2 == l(std::move(ac)));
                    assert(3 == l(std::move(av)));
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
                    auto const l = compose_visitors(l0, l2, l3);
                    assert(0 == l(ac));
                    assert(3 == l(av));
                    assert(2 == l(std::move(ac)));
                    assert(3 == l(std::move(av)));
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
                A a;
                A const c{};
                {
                    auto const la = compose_visitors(l, [] (A const &) { return -1; });
                    assert(-1 == la(c));
                    assert(1 == la(a));
                    assert(2 == la(std::move(c)));
                    assert(3 == la(A{}));
                }
                {
                    struct V { auto operator () (A const &) && { return -11; } };
                    auto lam = compose_visitors(l, V{});
                    assert(0 == lam(c));
                    assert(-11 == std::move(lam)(c));
                }
            }
        }
    }
    {
        assert((test_perferct_forwarding< 2, 2 >{}()));
    }
    {
        assert((hard< ROWS, COLS >()));
    }
    return EXIT_SUCCESS;
}

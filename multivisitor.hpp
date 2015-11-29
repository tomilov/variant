#pragma once

#include "prologue.hpp"

namespace test_variant
{

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

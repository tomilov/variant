#pragma once

#include "prologue.hpp"

#include <utility>
#include <tuple>
#include <array>

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
struct enumerator
{

    static constexpr std::size_t size_ = sizeof...(indices);
    static constexpr std::size_t count_ = (indices * ...);

    template< typename I >
    struct decomposer;

    template< std::size_t ...I >
    struct decomposer< std::index_sequence< I... > >
    {

        F & f;

        static constexpr std::size_t indices_[size_] = {indices...};

        static
        constexpr
        std::size_t
        order(std::size_t const i)
        {
            std::size_t o = 1;
            for (std::size_t n = i + 1; n < size_; ++n) {
                o *= indices_[n];
            }
            return o;
        }

        static constexpr std::size_t orders_[size_] = {order(I)...};

        static
        constexpr
        std::size_t
        digit(std::size_t c, std::size_t const i)
        {
            for (std::size_t n = 0; n < i; ++n) {
                c = c % orders_[n];
            }
            return c / orders_[i];
        }

        template< std::size_t c >
        constexpr
        bool
        call() const
        {
            return f.template operator () < digit(c, I)... >(); // error here
        }

    };

    decomposer< std::make_index_sequence< size_ > > const decomposer_;

    constexpr
    bool
    operator () () const
    {
        return call(std::make_index_sequence< count_ >{});
    }

    template< std::size_t ...counter >
    constexpr
    bool
    call(std::index_sequence< counter... >) const
    {
        return (decomposer_.template call< counter >() && ...);
    }

};

template< std::size_t ...indices, typename F >
CONSTEXPRF
enumerator< F, indices... >
make_enumerator(F & f)
{
    SA(0 < sizeof...(indices));
    SA(((0 < indices) && ...));
    return {{f}};
}

using ::versatile::type_qualifier;

template< std::size_t M >
struct pair
{

    type_qualifier qual_ids[1 + M];
    std::size_t type_ids[1 + M];

    CONSTEXPRF
    bool
    operator == (pair const & _rhs) const noexcept
    {
        for (std::size_t i = 0; i <= M; ++i) {
            if (qual_ids[i] != _rhs.qual_ids[i]) {
                return false;
            }
            if (type_ids[i] != _rhs.type_ids[i]) {
                return false;
            }
        }
        return true;
    }

    CONSTEXPRF
    std::size_t
    size() const noexcept
    {
        return (1 + M);
    }

};

using ::versatile::type_qualifier_of;

template< std::size_t M, type_qualifier type_qual = type_qualifier::value >
struct multivisitor
{

    using result_type = pair< M >;

    result_type & result_;

    using return_type = ::versatile::add_type_qualifier_t< type_qual, result_type >;

    static constexpr type_qualifier type_qual_ = type_qual;

    CONSTEXPRF
    std::size_t
    which() const noexcept
    {
        return 1 + M;
    }

    template< typename ...types >
    CONSTEXPRF
    return_type
    operator () (types &&... _values) & noexcept
    {
        //ASSERT (M == sizeof...(types));
        //ASSERT (!(is_visitable< types >{} || ...));
        result_ = {{type_qualifier_of< multivisitor & >, type_qualifier_of< types && >...}, {which(), _values...}};
        return static_cast< return_type >(result_);
    }

    template< typename ...types >
    CONSTEXPRF
    return_type
    operator () (types &&... _values) const & noexcept
    {
        result_ = {{type_qualifier_of< multivisitor const & >, type_qualifier_of< types && >...}, {which(), _values...}};
        return static_cast< return_type >(result_);
    }

    template< typename ...types >
    CONSTEXPRF
    return_type
    operator () (types &&... _values) && noexcept
    {
        result_ = {{type_qualifier_of< multivisitor && >, type_qualifier_of< types && >...}, {which(), _values...}};
        return static_cast< return_type >(result_);
    }

    template< typename ...types >
    CONSTEXPRF
    return_type
    operator () (types &&... _values) const && noexcept
    {
        result_ = {{type_qualifier_of< multivisitor const && >, type_qualifier_of< types && >...}, {which(), _values...}};
        return static_cast< return_type >(result_);
    }

};

template< std::size_t M, type_qualifier type_qual >
struct variadic_size< multivisitor< M, type_qual > >
        : ::versatile::index< M >
{

};

static constexpr std::size_t type_qual_begin = static_cast< std::size_t >(type_qualifier_of< void * & >);
static constexpr std::size_t type_qual_end = static_cast< std::size_t >(type_qualifier_of< void * volatile & >);

template< typename M, typename ...types >
struct fusor;

template< typename ...types, std::size_t ...K >
struct fusor< std::index_sequence< K... >, types... >
{

    SA(sizeof...(types) == sizeof...(K));

    std::tuple< types *... > const & stuff_;

    template< std::size_t ...Q >
    CONSTEXPRF
    bool
    operator () () const noexcept
    {
        static_assert(sizeof...(K) == sizeof...(Q));
        constexpr type_qualifier type_quals[sizeof...(Q)] = {static_cast< type_qualifier >(type_qual_begin + Q)...};
        decltype(auto) lhs = ::versatile::multivisit(forward_as< type_quals[K] >(*std::get< K >(stuff_))...);
        if (sizeof...(types) != lhs.size()) {
            return false;
        }
        if (type_qualifier_of< decltype(lhs) > != std::get< 0 >(stuff_)->type_qual_) {
            return false;
        }
        pair< (sizeof...(types) - 1) > const rhs = {{type_quals[K]...}, {std::get< K >(stuff_)->which()...}};
        return (lhs == rhs);
    }

};

template< typename ...types >
CONSTEXPRF
fusor< std::index_sequence_for< types... >, types... >
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

    static constexpr std::size_t ref_count = (type_qual_end - type_qual_begin); // test only reference types

    template< typename >
    struct variants;

    template< std::size_t ...J >
    struct variants< std::index_sequence< J... > >
    {

        using variant_type = variant< T< N - J >... >;
        SA(N == sizeof...(J));

        variant_type variants_[N] = {T< N - J >{}...};

        constexpr
        variant_type &
        operator [] (std::size_t const n)
        {
            return variants_[n];
        }

    };

    template< type_qualifier type_qual, std::size_t ...I >
    CONSTEXPRF
    static
    bool
    run(std::index_sequence< I... >) noexcept
    {
        SA(sizeof...(I) == M);
        std::size_t indices[M] = {};
        using multivisitor_type = multivisitor< M, type_qual >;
        typename multivisitor_type::result_type result_{};
        multivisitor_type mv{result_};
        variants< std::make_index_sequence< N > > variants_;
        auto permutation_ = std::make_tuple(&mv, &variants_[indices[I]]...);
        auto const fusor_ = make_fusor(permutation_);
        auto const enumerator_ = make_enumerator< ref_count, (I, ref_count)... >(fusor_);
        for (;;) {
            { // constexpr version of `permutation_ = std::make_tuple(&mv, &variants_[indices[I]]...);`
                std::get< 0 >(permutation_) = &mv;
                ((std::get< 1 + I >(permutation_) = &variants_[indices[I]]), ...);
            }
            if (!enumerator_()) {
                return false;
            }
            std::size_t m = 0;
            for (;;) {
                std::size_t & n = indices[m];
                if (++n != N) {
                    break;
                }
                n = 0;
                if (++m == M) {
                    break;
                }
            }
            if (m == M) {
                break;
            }
        }
        for (std::size_t n = 0; n < N; ++n) {
            if (variants_[n].which() != N - n) {
                return false;
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
        auto const i = std::make_index_sequence< M >{};
        CHECK (run< type_qualifier::value       >(i));
        CHECK (run< type_qualifier::const_value >(i));
        CHECK (run< type_qualifier::lref        >(i));
        CHECK (run< type_qualifier::rref        >(i));
        CHECK (run< type_qualifier::const_lref  >(i));
        CHECK (run< type_qualifier::const_rref  >(i));
        return true;
    }

};

} // namespace test_variant

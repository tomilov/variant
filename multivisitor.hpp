#pragma once

#include "prologue.hpp"

#include <utility>

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

template< typename multivisitor, typename variants >
struct fusor
{

    static constexpr std::size_t size_ = std::extent< variants >::value;

    template< typename = std::make_index_sequence< size_ > >
    struct fuse;

    template< std::size_t ...i >
    struct fuse< std::index_sequence< i... > >
    {

        multivisitor multivisitor_;
        variants variants_;
        std::size_t counter_;

        template< std::size_t m, std::size_t ...v >
        CONSTEXPRF
        bool
        operator () () noexcept
        {
            static_assert(size_ == sizeof...(v));
            constexpr type_qualifier type_qual_m = static_cast< type_qualifier >(type_qual_begin + m);
            constexpr type_qualifier type_quals_v[sizeof...(v)] = {static_cast< type_qualifier >(type_qual_begin + v)...};
            pair< size_ > const rhs = {{type_qual_m, type_quals_v[i]...}, {size_ + 1, variants_[i].which()...}};
            decltype(auto) lhs = ::versatile::multivisit(forward_as< type_qual_m >(multivisitor_),
                                                         forward_as< type_quals_v[i] >(variants_[i])...);
            CHECK (1 + size_ == lhs.size());
            CHECK (type_qualifier_of< decltype(lhs) > == multivisitor_.type_qual_);
            if (!(lhs == rhs)) {
                return false;
            }
            ++counter_;
            return true;
        }

    };

    fuse<> fuse_;

    constexpr
    auto &
    operator [] (std::size_t const n)
    {
        return fuse_.variants_[n];
    }

};

// variant - variant
// T - type generator
// M - multivisitor arity, N - number of alternative (bounded) types
template< template< std::size_t I > class T,
          template< typename ...types > class variant,
          template< typename ...types > class wrapper = ::versatile::identity,
          std::size_t M = 2, std::size_t N = M >
class test_perferct_forwarding
{

    static constexpr std::size_t ref_count = (type_qual_end - type_qual_begin); // test only reference types

    template< typename = std::make_index_sequence< N > >
    struct variants;

    template< std::size_t ...j >
    struct variants< std::index_sequence< j... > >
    {

        using variant_type = variant< typename wrapper< T< N - j > >::type... >;

        variant_type variants_[N] = {T< N - j >{}...};

        constexpr
        variant_type const &
        operator [] (std::size_t const n) const
        {
            return variants_[n];
        }

    };

    template< type_qualifier type_qual, std::size_t ...i >
    CONSTEXPRF
    static
    bool
    run(std::index_sequence< i... >) noexcept
    {
        SA(sizeof...(i) == M);
        std::size_t indices[M] = {};
        using multivisitor_type = multivisitor< M, type_qual >;
        typename multivisitor_type::result_type result_{};
        variants<> const variants_{};
        using variant_type = typename variants<>::variant_type;
        //constexpr std::size_t count_ = M * N *
        fusor< multivisitor_type, variant_type [M] > fusor_{{{result_}, {}, 0}};
        auto const enumerator_ = make_enumerator< ref_count, (i, ref_count)... >(fusor_.fuse_);
        for (;;) {
            ((fusor_[i] = variants_[indices[i]]), ...);
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
        //std::cout << fusor_.fuse_.counter_ << std::endl;
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

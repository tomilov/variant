#pragma once

#include "prologue.hpp"

#include <boost/variant.hpp>

namespace test_boost_variant
{

template< typename ...types >
struct variant_c
{

    using variant = boost::variant< types... >;

    template< typename type >
    using index_at_t = ::versatile::index_at_t< ::versatile::unwrap_type_t< type >, ::versatile::unwrap_type_t< types >..., void >;

    variant_c() = default;

    variant_c(variant_c &) = default;
    variant_c(variant_c const &) = default;
    variant_c(variant_c &&) = default;

    variant_c &
    operator = (variant_c const &) = default;
    variant_c &
    operator = (variant_c &) = default;
    variant_c &
    operator = (variant_c &&) = default;

    template< typename type,
              typename = index_at_t< type > >
    constexpr
    variant_c(type && _value)
        : member_(std::forward< type >(_value))
    { ; }

    template< typename argument,
              typename = index_at_t< argument > >
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
    constexpr
    bool
    active() const noexcept
    {
        return (index_at_t< type >::value == which());
    }

    template< typename type,
              typename = index_at_t< type > >
    explicit
    constexpr
    operator type const & () const
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return boost::get< type >(member_);
    }

    template< typename type,
              typename = index_at_t< type > >
    explicit
    constexpr
    operator type & ()
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return boost::get< type >(member_);
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

    template< typename type >
    using index_at_t = ::versatile::index_at_t< ::versatile::unwrap_type_t< type >, ::versatile::unwrap_type_t< types >..., void >;

    //using base::base; // seems there is wrong design of boost::variant constructor
    //using base::operator =;

    variant_i() = default;

    variant_i(variant_i &) = default;
    variant_i(variant_i const &) = default;
    variant_i(variant_i &&) = default;

    variant_i &
    operator = (variant_i const &) = default;
    variant_i &
    operator = (variant_i &) = default;
    variant_i &
    operator = (variant_i &&) = default;

    template< typename type,
              typename = index_at_t< type > >
    constexpr
    variant_i(type && _value)
        : base(std::forward< type >(_value))
    { ; }

    template< typename argument,
              typename = index_at_t< argument > >
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
    constexpr
    bool
    active() const noexcept
    {
        return (index_at_t< type >::value == which());
    }

    template< typename type,
              typename = index_at_t< type > >
    explicit
    constexpr
    operator type const & () const
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return boost::get< type >(static_cast< variant_i::base const & >(*this));
    }

    template< typename type,
              typename = index_at_t< type > >
    explicit
    constexpr
    operator type & ()
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return boost::get< type >(static_cast< variant_i::base & >(*this));
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

template< typename type >
struct boost_recursive_wrapper
        : ::versatile::identity< ::boost::recursive_wrapper< type > >
{

};

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

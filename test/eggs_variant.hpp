#pragma once

#include <eggs/variant.hpp>

#include <versatile/wrappers.hpp>
#include <versatile/visit.hpp>
#include <versatile/utility.hpp>

#include "prologue.hpp"

namespace test_eggs_variant
{

using ::eggs::variants::get;

template< typename ...types >
struct eggs_variant_c // composition
{

    using variant = ::eggs::variant< types... >;

    template< typename type >
    using index_at_t = ::versatile::index_at_t< ::versatile::unwrap_type_t< type >, ::versatile::unwrap_type_t< types >..., void >;

    constexpr
    eggs_variant_c() = default;

    constexpr
    eggs_variant_c(eggs_variant_c const &) = default;
    constexpr
    eggs_variant_c(eggs_variant_c &) = default;
    constexpr
    eggs_variant_c(eggs_variant_c &&) = default;

    constexpr
    eggs_variant_c &
    operator = (eggs_variant_c const &) = default;
    constexpr
    eggs_variant_c &
    operator = (eggs_variant_c &) = default;
    constexpr
    eggs_variant_c &
    operator = (eggs_variant_c &&) = default;

    template< typename type, typename = index_at_t< type > >
    constexpr
    eggs_variant_c(type && _value)
        : member_(std::forward< type >(_value))
    { ; }

    template< typename type, typename = index_at_t< type > >
    constexpr
    eggs_variant_c &
    operator = (type && _value)
    {
        member_ = std::forward< type >(_value);
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

    template< typename type, typename = index_at_t< type > >
    explicit
    constexpr
    operator type const & () const
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return get< type >(member_);
    }

    template< typename type, typename = index_at_t< type > >
    explicit
    constexpr
    operator type & ()
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return get< type >(member_);
    }

private :

    variant member_;

};

} // namespace test

namespace versatile
{

template< typename first, typename ...rest >
struct is_visitable< ::test_eggs_variant::eggs_variant_c< first, rest... > >
        : std::true_type
{

};

} // namespace versatile

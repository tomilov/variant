#pragma once

#include <eggs/variant.hpp>

#include <versatile/type_traits.hpp>

#include "prologue.hpp"

namespace test_eggs_variant
{

using ::eggs::variants::get;
using ::versatile::index_at;
using ::versatile::get_index;
using ::versatile::unwrap_type_t;

template< typename ...types >
struct eggs_variant_c // composition
{

    using variant = ::eggs::variant< types... >;

    template< typename type >
    using index_at = index_at< unwrap_type_t< type >, unwrap_type_t< types >... >;

    template< typename ...arguments >
    using index_of_constructible = get_index< std::is_constructible_v< types, arguments... >... >;

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

    template< typename type, typename = index_at< type > >
    constexpr
    eggs_variant_c(type && _value)
        : member_(std::forward< type >(_value))
    { ; }

    template< typename type, typename = index_at< type > >
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
        if (!member_) {
            return 0;
        }
        return sizeof...(types) - member_.which();
    }

    template< typename type >
    constexpr
    bool
    active() const noexcept
    {
        return (index_at< type >::value == which());
    }

    template< typename type, typename = index_at< type > >
    explicit
    constexpr
    operator type const & () const
    {
        if (!active< type >()) {
            throw std::bad_cast{};
        }
        return get< type >(member_);
    }

    template< typename type, typename = index_at< type > >
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

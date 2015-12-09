#pragma once

#include "prologue.hpp"

#include <typeinfo>

namespace test
{

template< template< typename ... > class wrapper,
          template< typename ... > class variant >
class check_destructible
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
        using ::versatile::in_place;
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
                    U u{};
                    CHECK (is_active< A >(u));
                    CHECK (static_cast< A & >(u).s_ == state::default_constructed);
                    static_cast< A & >(u).set(d);
                    CHECK (!d);
                }
                CHECK (!!d);
                CHECK (*d == typeid(A));
            }
            {
                std::type_info const * d = nullptr;
                {
                    U u{in_place< B >};
                    CHECK (is_active< B >(u));
                    CHECK (static_cast< B & >(u).s_ == state::default_constructed);
                    static_cast< B & >(u).set(d);
                    CHECK (!d);
                }
                CHECK (!!d);
                CHECK (*d == typeid(B));
            }
            {
                std::type_info const * d = nullptr;
                {
                    U u{in_place< A >};
                    CHECK (is_active< A >(u));
                    CHECK (static_cast< A & >(u).s_ == state::default_constructed);
                    static_cast< A & >(u).set(d);
                    CHECK (!d);
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
        CHECK (destructible());
        return true;
    }

};

} // namespace test

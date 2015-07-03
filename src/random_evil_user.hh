/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef RANDOM_EVIL_USER
#define RANDOM_EVIL_USER

#include <iostream>
#include <deque>
#include "abstract_user.hh"
#include "market.hh"


class RandomEvilUser : public AbstractUser
{
    bool done_ = false;

    public:
    RandomEvilUser( const size_t &uid );

    void take_actions( Market& mkt ) override;

    bool done( const Market& mkt ) override;

    void print_stats( const Market& mkt ) const override;

    double utility( const Market& ) const override { return 0.; };
    double best_expected_utility() const override { return 0.; };
    double benefit( const Market& ) const override { return 0.; };
};

#endif /* RANDOM_EVIL_USER */

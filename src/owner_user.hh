/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef OWNER_USER
#define OWNER_USER

#include <iostream>
#include <deque>
#include "abstract_user.hh"
#include "market.hh"


class OwnerUser : public AbstractUser
{
    const double default_slot_offer_;
    const size_t total_num_slots_;
    const bool only_add_once_;
    bool added_before_;

    public:
    OwnerUser( const std::string &name, const double default_slot_offer, const size_t total_num_slots, const bool add_once );

    void take_actions( Market& mkt ) override;

    bool done( const Market& mkt ) const override;

    void print_stats( const Market& mkt ) const override;
};

#endif /* OWNER_USER */

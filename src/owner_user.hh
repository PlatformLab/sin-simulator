/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef OWNER_USER
#define OWNER_USER

#include <iostream>
#include "abstract_user.hh"
#include "market.hh"


class OwnerUser : public AbstractUser
{
    const double default_slot_offer_;
    const size_t total_num_slots_;
    bool done_ = false;

    public:
    OwnerUser( const size_t &uid, const double default_slot_offer, const size_t total_num_slots )
        : AbstractUser( uid ),
        default_slot_offer_( default_slot_offer ),
        total_num_slots_( total_num_slots )
    {
    }

    void take_actions( Market& mkt ) override
    {
        if ( not done_ )
        {
            // add a bunch of slots
            done_ = true;
        }
    }

    bool done( const Market& mkt ) override
    {
        return done_;
    }
};

#endif /* OWNER_USER */

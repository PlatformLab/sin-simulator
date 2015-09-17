/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef OWNER_USER
#define OWNER_USER

#include <iostream>
#include "abstract_user.hh"
#include "market.hh"
#include "link.hh"


class OwnerUser : public AbstractUser
{
    const std::vector<Interval> intervals_to_add_;
    bool done_ = false;

    public:
    OwnerUser( Link link )
        : AbstractUser( link.uid, link.start ),
        intervals_to_add_( link.get_intervals() )
    {
    }

    void take_actions( Market& mkt ) override
    {
        if ( not done_ ) {
            for ( auto &interval : intervals_to_add_ ) {
                Offer toAdd = { uid_, interval, 1.0, { false, interval } };
                bool result = mkt.add_offer( toAdd );
                assert( result );
            }
            done_ = true;
        }
    }

    std::vector<Interval> intervals() { return { }; }
};

#endif /* OWNER_USER */

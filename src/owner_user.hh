/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef OWNER_USER
#define OWNER_USER

#include <iostream>
#include "abstract_user.hh"
#include "market.hh"
#include "link.hh"


class OwnerUser : public AbstractUser
{
    const std::vector<Opportunity> opportunities_to_add_;
    bool done_ = false;

    public:
    OwnerUser( Link link )
        : AbstractUser( link.uid, link.start ),
        opportunities_to_add_( link.get_opportunities() )
    {
    }

    void take_actions( Market& mkt ) override
    {
        if ( not done_ ) {
            //const std::vector<std::shared_ptr<Opportunity>> added_opportunities = mkt.add_opportunities( opportunities_to_add_ );
            for ( auto &o : opportunities_to_add_ ) {
                Offer toAdd = { uid_, o.interval, 1, 1.0, { false, { 0,0 } } };
                bool result = mkt.add_offer( toAdd );
                assert( result );
                //opportunities_ = opportunities_to_add_; TODO maybe add back later
            }
            done_ = true;
        }
    }
};

#endif /* OWNER_USER */

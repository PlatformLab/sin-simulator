/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef OWNER_USER
#define OWNER_USER

#include <iostream>
#include "abstract_user.hh"
#include "market.hh"
#include "link.hh"


class OwnerUser : public AbstractUser
{
    const Link link_;
    bool done_ = false;

    public:
    OwnerUser( Link link )
        : AbstractUser( link.uid ),
        link_( link )
    {
    }

    void take_actions( Market& mkt ) override
    {
        if ( not done_ )
        {
            for ( Interval &i : link_.get_intervals() )
            {
                mkt.add_interval( i );
            }
            done_ = true;
        }
    }

    bool done( const Market& mkt ) override
    {
        return done_;
    }
};

#endif /* OWNER_USER */

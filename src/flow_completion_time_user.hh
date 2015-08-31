/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef FLOW_COMPLETION_TIME_USER
#define FLOW_COMPLETION_TIME_USER

#include <iostream>
#include "abstract_user.hh"
#include "market.hh"


class FlowCompletionTimeUser : public AbstractUser
{
    const size_t start_time_;
    const size_t num_packets_;
    bool done_ = false;

    public:
    FlowCompletionTimeUser( const size_t uid, const size_t start_time, const size_t num_packets )
        : AbstractUser( uid ),
        start_time_( start_time ),
        num_packets_( num_packets )
    {
    }

    void take_actions( Market& mkt ) override
    {
        if ( not done_ )
        {
            double cost = mkt.cost_for_intervals( uid_, start_time_, start_time_+20, num_packets_ );
            std::cout << "cost was " << cost << std::endl;
            // do stuff
            done_ = true;
        }
    }

    bool done( const Market& mkt ) override
    {
        return done_;
    }
};

#endif /* FLOW_COMPLETION_TIME_USER */

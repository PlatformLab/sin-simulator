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
            size_t best_interval_length = 1;
            double best_interval_cost = 0;

            for ( size_t interval_length = 1; interval_length <= 1024; interval_length <<= 1 ) {
                double cost = mkt.cost_for_intervals( uid_, start_time_, start_time_+interval_length, num_packets_ );
                if ( cost > 0 and ( best_interval_cost == 0 or -best_interval_length-best_interval_cost < -interval_length-cost ) ) {
                    best_interval_length = interval_length;
                    best_interval_cost = cost;
                }
            }
            if ( best_interval_cost > 0 ) {
                double spent = mkt.buy_intervals( uid_, start_time_, start_time_+best_interval_length, num_packets_, best_interval_cost );
                std::cout << "cost was " << best_interval_cost << " and spent " << spent << " for interval length " << best_interval_length << std::endl;
                if ( spent > 0 ) {
                    done_ = true;
                }
            }
        }
    }

    bool done( const Market& mkt ) override
    {
        return done_;
    }
};

#endif /* FLOW_COMPLETION_TIME_USER */

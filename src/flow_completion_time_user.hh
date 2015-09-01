/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef FLOW_COMPLETION_TIME_USER
#define FLOW_COMPLETION_TIME_USER

#include <iostream>
#include "abstract_user.hh"
#include "market.hh"
#include "flow.hh"


class FlowCompletionTimeUser : public AbstractUser
{
    const Flow flow_;
    bool done_ = false;

    public:
    FlowCompletionTimeUser( Flow flow )
        : AbstractUser( flow.uid ),
        flow_( flow )
    {
    }

    void take_actions( Market& mkt ) override
    {
        if ( not done_ )
        {
            size_t best_interval_length = 1;
            double best_interval_cost = 0;

            for ( size_t interval_length = 1; interval_length <= 1024; interval_length <<= 1 ) {
                double cost = mkt.cost_for_intervals( uid_, flow_.start, flow_.start+interval_length, flow_.num_packets );
                if ( cost > 0 and ( best_interval_cost == 0 or -best_interval_length-best_interval_cost < -interval_length-cost ) ) {
                    best_interval_length = interval_length;
                    best_interval_cost = cost;
                }
            }
            if ( best_interval_cost > 0 ) {
                double spent = mkt.buy_intervals( uid_, flow_.start, flow_.start+best_interval_length, flow_.num_packets, best_interval_cost );
                std::cout << "cost was " << best_interval_cost << " and spent " << spent << " for interval length " << best_interval_length << std::endl;
                if ( spent > 0 ) {
                    done_ = true;
                }
            }
        }
        done_ = true; // only run once regardless for now
    }

    bool done( const Market& mkt ) override
    {
        return done_;
    }
};

#endif /* FLOW_COMPLETION_TIME_USER */

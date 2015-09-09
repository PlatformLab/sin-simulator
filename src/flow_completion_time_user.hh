/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef FLOW_COMPLETION_TIME_USER
#define FLOW_COMPLETION_TIME_USER

#include <iostream>
#include "abstract_user.hh"
#include "market.hh"
#include "flow.hh"


class FlowCompletionTimeUser : public AbstractUser
{
    const size_t num_packets_;
    size_t done_ = 0;

    public:
    FlowCompletionTimeUser( Flow flow )
        : AbstractUser( flow.uid, flow.start ),
        num_packets_( flow.num_packets )
    {
    }

    void take_actions( Market& mkt ) override
    {
        const size_t num_owned = mkt.intervals_owned_by_user( uid_ );
        const size_t num_to_buy = num_owned < num_packets_ ? num_packets_ - num_owned : 0;

        if ( num_to_buy > 0 ) {
           // std::cout << uid_to_string( uid_ ) << "owns " << num_owned << "!!! trying to buy " << num_to_buy << std::endl;
            size_t best_interval_length = 1;
            double best_interval_cost = std::numeric_limits<double>::max();
            size_t start_time = std::max( mkt.time(), start_ );

            //for ( size_t interval_length = 1; interval_length <= 1024; interval_length <<= 1 ) {
            for ( size_t interval_length = num_to_buy - 1; interval_length <= 16; interval_length+=2 ) {
                double cost = mkt.cost_for_intervals( uid_, start_time, start_time+interval_length, num_to_buy );
                if ( mkt.verbose() ) {
                    std::cout << uid_to_string( uid_ ) << ": $" << cost << " for interval "<< start_time << ", " << start_time + interval_length << std::endl;
                }
                if ( cost < std::numeric_limits<double>::max() and -(double) best_interval_length-best_interval_cost < -(double) interval_length-cost ) {
                    best_interval_length = interval_length;
                    best_interval_cost = cost;
                }

                if ( -(double) best_interval_length-best_interval_cost > -(double) interval_length ) {
                    // utility can't improve unless slots were negative cost
                    break;
                }
            }
            if ( best_interval_cost < std::numeric_limits<double>::max() ) {
                std::vector<std::pair<size_t, double>> offers { }; 
                //for ( size_t extra_length = 1; extra_length <= 64; extra_length <<= 1 ) {
                for ( size_t extra_length = 1; extra_length <= 16; extra_length++ ) {
                    //std::cout << "start_time " << tart_time << "best_interval_length " <<  best_interval_length << "extra_length " << extra_length << std::endl;
                    offers.push_back( { start_time + best_interval_length + extra_length, (double) extra_length + .01 } );
                }

                double spent = mkt.buy_intervals( uid_, start_time, start_time+best_interval_length, num_to_buy, best_interval_cost, offers );
                //std::cout << uid_to_string( uid_ ) << " cost was " << best_interval_cost << " and spent " << spent << " for interval length " << best_interval_length << std::endl;
                if ( spent > 0 ) {
                    //std::cout << uid_to_string( uid_ ) << " spent " << spent << " for interval "<< start_time << ", " << start_time+best_interval_length << std::endl;
                }
            }
            //done_++;
        }
    }
};

#endif /* FLOW_COMPLETION_TIME_USER */

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

    void take_actions( Market& ) override
    {
        size_t num_to_buy = num_packets_;

        std::vector<Offer *> = best_offers;
        double best_offers_utility = std::numberic_limits<double>::max();

        size_t start_time = std::max( mkt.time(), start_ );

        for ( size_t interval_length = num_to_buy - 1; interval_length <= 16; interval_length++ ) {
            const std::vector<Offer> = interval_offers = offers_in_interval( { start_time, start_time+interval_length } );
            const std::vector<Offer *> = cheapest_offers { };
            for ( auto &o : interval_offers ) {
                assert( o.num_packets == 1 ); // XXX silly
                auto cost_compare = [ uid_ ] ( const Offer *a, const Offer *b )
                { double acost = a->seller_uid == uid ? 0 : a->cost; double bcost = b->seller_uid == uid ? 0 : b->cost; return acost < bcost; };

                double offer_cost = ( i.seller_uid == uid_ ) ? 0 : o.cost;
                if ( cheapest_offers.size() == num_intervals and offer_cost < cheapest_offers[0]->cost ) {
                    pop_heap( cheapest_offers.begin(), cheapest_offers.end(), cost_compare );
                    cheapest_offers.back() = &o;
                    push_heap( cheapest_offers.begin(), cheapest_offers.end(), cost_compare );
                } else if ( cheapest_offers.size() < num_intervals ) {
                    cheapest_offers.push_back( &i );
                    push_heap( cheapest_offers.begin(), cheapest_offers.end(), cost_compare );
                }
            }
            if ( cheapest_offers.size() == num_packets_ ) {
                const double benefit = -( start_time+interval_length-start_ );
                double cost = 0; 
                for ( auto &o_ptr : cheapest_offers ) {
                    cost += o_ptr->cost;
                }
                const double utility = benefit - cost;
                if ( utility > best_offers_utility ) {
                    best_offers = move( cheapest_offers );
                    best_offers_utility = utility;
                }
            }

            /*
               if ( -(double) best_interval_length-best_interval_cost > -(double) interval_length ) {
            // utility can't improve unless slots were negative cost
            break;
            }
             */
        }
        if ( best_offers_utility < std::numeric_limits<double>::max() ) {
            // we can buy some offers
            for ( size_t extra_length = 1; extra_length <= 16; extra_length++ ) {
                /*
                //std::cout << "start_time " << tart_time << "best_interval_length " <<  best_interval_length << "extra_length " << extra_length << std::endl;
                offers.push_back( { start_time + best_interval_length + extra_length, (double) extra_length + .01 } );
                 */
            }
        }
    }

#endif /* FLOW_COMPLETION_TIME_USER */

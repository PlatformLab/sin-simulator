/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef FLOW_COMPLETION_TIME_USER
#define FLOW_COMPLETION_TIME_USER

#include <iostream>
#include <algorithm>
#include "abstract_user.hh"
#include "pretty_print.hh"
#include "market.hh"
#include "flow.hh"
#include "offer.hh"


class FlowCompletionTimeUser : public AbstractUser
{
    const size_t num_packets_;
    bool done_ = false;

    public:
    FlowCompletionTimeUser( Flow flow )
        : AbstractUser( flow.uid, flow.start ),
        num_packets_( flow.num_packets )
    {
    }

    void take_actions( Market& mkt ) override
    {
        if ( done_ )
            return;

        size_t num_to_buy = num_packets_;
        std::cout << uid_to_string( uid_ ) << " trying to buy " << num_to_buy << " opportunties" << std::endl;

        std::vector<Offer> best_offers;
        double best_offers_utility = std::numeric_limits<double>::lowest();

        size_t start_time = std::max( mkt.time(), start_ );

        for ( size_t interval_length = num_to_buy - 1; interval_length <= 5; interval_length++ ) {
            Interval interval = { start_time, start_time+interval_length };
            std::cout << "exploring offers on interval " << interval_to_string( interval ) << std::endl;
            const std::vector<Offer> interval_offers = mkt.offers_in_interval( interval );
            std::vector<Offer> cheapest_offers { };
            for ( const auto &o : interval_offers ) {
                assert( o.num_packets == 1 ); // XXX silly
                assert( o.seller_uid != uid_ ); // XXX silly

                auto cost_compare = [] ( const Offer a, const Offer b ) { return a.cost < b.cost; };

                if ( cheapest_offers.size() == num_to_buy and o.cost < cheapest_offers[0].cost ) {
                    std::pop_heap( cheapest_offers.begin(), cheapest_offers.end(), cost_compare );
                    cheapest_offers.back() = o;
                    std::push_heap( cheapest_offers.begin(), cheapest_offers.end(), cost_compare );
                } else if ( cheapest_offers.size() < num_to_buy ) {
                    cheapest_offers.push_back( o );
                    std::push_heap( cheapest_offers.begin(), cheapest_offers.end(), cost_compare );
                }
            }
            if ( cheapest_offers.size() == num_packets_ ) {
                const double benefit = -( start_time+interval_length-start_ );
                double cost = 0; 
                for ( auto &o : cheapest_offers ) {
                    cost += o.cost;
                }
                const double utility = benefit - cost;
                if ( utility > best_offers_utility ) {
                    best_offers = std::move( cheapest_offers );
                    best_offers_utility = utility;
                }
            } else {
                std::cout << "can't buy enough packets on this interval " << std::endl;
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
            bool all_successful = true;
            for ( const Offer &o : best_offers ) {
                std::cout << "User " << uid_to_string( uid_ ) << " buying offer on interval " <<  interval_to_string( o.interval ) << " for $" << o.cost << std::endl;
                bool success = mkt.buy_offer( uid_, o );
                if ( success ) {
                    Opportunity toAdd = { size_t( -1 ), uid_, { o.interval.start, o.interval.end } };
                    opportunities_.push_back( toAdd );
                } else {
                    all_successful = false;
                }
            }
            done_ = all_successful;
        } else {
                std::cout << "no offers we could take" << std::endl;
        }
    }
};

#endif /* FLOW_COMPLETION_TIME_USER */

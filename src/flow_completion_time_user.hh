/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef FLOW_COMPLETION_TIME_USER
#define FLOW_COMPLETION_TIME_USER

#include <iostream>
#include <algorithm>
#include <unordered_set>
#include "abstract_user.hh"
#include "pretty_print.hh"
#include "market.hh"
#include "flow.hh"
#include "interval.hh"
#include "offer.hh"


class FlowCompletionTimeUser : public AbstractUser
{
    const size_t num_packets_;
    size_t num_packets_allocated_ = 0;
    bool done_ = false;
    std::unordered_set<Offer> offers_posted_ { };
    std::vector<Interval> intervals_ { };

    public:
    FlowCompletionTimeUser( Flow flow )
        : AbstractUser( flow.uid, flow.start ),
        num_packets_( flow.num_packets )
    {
    }

    std::vector<Interval> intervals() { return intervals_; }

    /* check taken offers against record of offers we posted */
    void check_taken_offers( Market& mkt )
    {
        for ( const Offer &o : mkt.taken_offers() ) {
            auto search = offers_posted_.find( o );
            if ( search != offers_posted_.end() ) {
                // someone took an offer we posted

                if ( o.exchange_for_interval.first ) {

                } else {
                    // not a trading offer, we have less packets allocated now
                    num_packets_allocated_ -= o.interval.num_packets;
                }

                offers_posted_.erase( search );
            }
        }
    }

    void take_actions( Market& mkt ) override
    {
        check_taken_offers( mkt );
        if ( num_packets_allocated_ >= num_packets_ )
            return;

        size_t num_to_buy = num_packets_-num_packets_allocated_;
        std::cout << uid_to_string( uid_ ) << " trying to buy " << num_to_buy << " packet deliveries" << std::endl;

        std::vector<Offer> best_offers;
        double best_offers_utility = std::numeric_limits<double>::lowest();

        size_t start_time = std::max( mkt.time(), start_ );

        for ( size_t interval_length = num_to_buy - 1; interval_length <= 64; interval_length++ ) {
            std::cout << "exploring offers on between " << start_time << " and " << start_time+interval_length << std::endl;
            const std::vector<Offer> interval_offers = mkt.offers_in_interval( start_time, start_time+interval_length );
            std::vector<Offer> cheapest_offers { };
            for ( const auto &o : interval_offers ) {
                assert( o.interval.num_packets == 1 ); // XXX silly
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
                //std::cout << "start_time " << start_time << " interval_length " << interval_length << " start_ " << start_<< std::endl;
                assert( start_time + interval_length >= start_ ); // no underflow
                const double benefit = -(double) ( start_time + interval_length - start_ );
                double cost = 0; 
                for ( auto &o : cheapest_offers ) {
                    cost += o.cost;
                }
                const double utility = benefit - cost;
                if ( utility > best_offers_utility ) {
                //    std::cout << "best seen so far " << utility << ", previous was "<< best_offers_utility<< std::endl;
                    best_offers = std::move( cheapest_offers );
                    best_offers_utility = utility;
                }
            } else {
                //std::cout << "can't buy enough packets on this interval " << std::endl;
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
            for ( const Offer &o : best_offers ) {
                std::cout << "User " << uid_to_string( uid_ ) << " buying offer on interval " <<  interval_to_string( o.interval ) << " for $" << o.cost << std::endl;
                bool success = mkt.buy_offer( uid_, o );
                if ( success ) {
                    Interval toAdd = { uid_, o.interval.start, o.interval.end, 1 };
                    intervals_.push_back( toAdd );
                    num_packets_allocated_++;
                } else {
                    assert( false && "thought we could buy offer but we couldn't");
                }
            }
        } else {
                std::cout << "no offers we could take" << std::endl;
        }
    }
};

#endif /* FLOW_COMPLETION_TIME_USER */

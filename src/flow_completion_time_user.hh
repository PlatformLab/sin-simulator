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
    const size_t num_packets_; // size of flow
    size_t num_opportunities_owned_ = 0;
    std::unordered_set<Offer> offers_posted_ { };
    std::vector<Opportunity> opportunities_ { };

    public:
    FlowCompletionTimeUser( const Flow flow )
        : AbstractUser( flow.uid, flow.start ),
        num_packets_( flow.num_packets )
    {
    }

    std::vector<Opportunity> opportunities() { return opportunities_ ; }

    /* check taken offers against record of offers we posted */
    void check_taken_offers( Market& mkt )
    {
        for ( const Offer &o : mkt.taken_offers() ) {
            auto search = offers_posted_.find( o );
            if ( search != offers_posted_.end() ) {
                // someone took an offer we posted

                /*
                if ( o.exchange_for_interval.first ) {

                } else {
                    */
                    // not a trading offer, we have less packets allocated now
                    num_opportunities_owned_ -= o.opportunity.num_packets;
                //}

                offers_posted_.erase( search );
            }
        }
    }

    const std::vector<Offer> cheapest_offers_in_interval( const Market& mkt, const size_t start, const size_t end, const size_t num_to_buy, bool recursive )
    {
        std::vector<Offer> toRet { }; // vector that stores priority queue of cheapest offers

        /*
        auto cost_compare = [] ( const Offer &a, const Offer &b ) { return a.cost < b.cost; };

        for ( const Offer &o : mkt.offers_in_interval( start, end ) ) {
            assert( o.seller_uid != uid_ ); // XXX silly

            if ( exchange_for_interval.first ) {
                if ( recursive ) {
                    const size_t recursize_start = exchange_for_interval.second.start;
                    const size_t recursize_end = exchange_for_interval.second.end;
                    const size_t recursize_num_packets = exchange_for_interval.second.num_packets;
                    const std::vector<Offer> toAdd = cheapest_offers_in_interval( mkt, recursive_start, recursive_end, recursive_num_packets, false );
                }
            } else {
                assert( o.interval.num_packets == 1 ); // XXX silly

                if ( toRet.size() == num_to_buy and o.cost < toRet[0].cost ) {
                    std::pop_heap( toRet.begin(), toRet.end(), cost_compare );
                    toRet.back() = o;
                    std::push_heap( toRet.begin(), toRet.end(), cost_compare );
                } else if ( toRet.size() < num_to_buy ) {
                    toRet.push_back( o );
                    std::push_heap( toRet.begin(), toRet.end(), cost_compare );
                }
            }
        }
        */

        return toRet;
    }

    void take_actions( Market& mkt, const size_t time ) override
    {
    //    check_taken_offers( mkt );
    //    if ( num_packets_allocated_ >= num_packets_ )
    //        return;

    //    size_t num_to_buy = num_packets_-num_packets_allocated_;
    //    std::cout << uid_to_string( uid_ ) << " trying to buy " << num_to_buy << " packet deliveries" << std::endl;

    //    std::vector<Offer> best_offers;
    //    double best_offers_utility = std::numeric_limits<double>::lowest();

    //    size_t start_time = std::max( time, start_ );

    //    for ( size_t interval_length = num_to_buy - 1; interval_length <= 64; interval_length++ ) {
    //        std::cout << "exploring offers on between " << start_time << " and " << start_time+interval_length << std::endl;

    //        const std::vector<Offer> cheapest_offers = cheapest_offers_in_interval( mkt, start_time, start_time + interval_length, num_to_buy, true );

    //        if ( cheapest_offers.size() == num_packets_ ) { // TODO change
    //            //std::cout << "start_time " << start_time << " interval_length " << interval_length << " start_ " << start_<< std::endl;
    //            assert( start_time + interval_length >= start_ ); // no underflow
    //            const double benefit = -(double) ( start_time + interval_length - start_ ); // TODO change if offers taken fit in shorter interval, previous taken offer had later end time
    //            double cost = 0; 
    //            for ( const Offer &o : cheapest_offers ) {
    //                cost += o.cost;
    //            }
    //            const double utility = benefit - cost;
    //            if ( utility > best_offers_utility ) {
    //            //    std::cout << "best seen so far " << utility << ", previous was "<< best_offers_utility<< std::endl;
    //                best_offers = std::move( cheapest_offers );
    //                best_offers_utility = utility;
    //            }
    //        } else {
    //            //std::cout << "can't buy enough packets on this interval " << std::endl;
    //        }

    //        /*
    //           if ( -(double) best_interval_length-best_interval_cost > -(double) interval_length ) {
    //        // utility can't improve unless slots were negative cost
    //        break;
    //        }
    //         */
    //    }
    //    if ( best_offers_utility < std::numeric_limits<double>::max() ) {
    //        // we can buy some offers
    //        for ( const Offer &o : best_offers ) {
    //            std::cout << "User " << uid_to_string( uid_ ) << " buying offer on interval " <<  interval_to_string( o.interval ) << " for $" << o.cost << std::endl;
    //            bool success = mkt.buy_offer( uid_, o );
    //            if ( success ) {
    //                Interval toAdd = { uid_, o.interval.start, o.interval.end, 1 };
    //                intervals_.push_back( toAdd );
    //                num_packets_allocated_++;
    //            } else {
    //                assert( false && "thought we could buy offer but we couldn't");
    //            }
    //        }
    //    } else {
    //            std::cout << "no offers we could take" << std::endl;
    //    }
    }
};

#endif /* FLOW_COMPLETION_TIME_USER */

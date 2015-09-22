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

    std::vector<Opportunity> opportunities()
    {
        return opportunities_;
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
    const std::vector<std::pair<Offer, Opportunity *>> best_offers( const size_t start_time, const size_t num_packets_to_buy )
    {
        std::vector<Offer> best_offers;
        double best_offers_utility = std::numeric_limits<double>::lowest();


        for ( size_t interval_length = 1; interval_length <= 64; interval_length++ ) {
            std::cout << "exploring offers on between " << start_time << " and " << start_time+interval_length << std::endl;

//            const std::vector<Offer> cheapest_offers = cheapest_offers_in_interval( mkt, start_time, start_time + interval_length, num_packets_to_buy, true );
//
//            if ( cheapest_offers.size() == num_packets_ ) { // TODO change
//                //std::cout << "start_time " << start_time << " interval_length " << interval_length << " start_ " << start_<< std::endl;
//                assert( start_time + interval_length >= start_ ); // no underflow
//                const double benefit = -(double) ( start_time + interval_length - start_ ); // TODO change if offers taken fit in shorter interval, previous taken offer had later end time
//                double cost = 0; 
//                for ( const Offer &o : cheapest_offers ) {
//                    cost += o.cost;
//                }
//                const double utility = benefit - cost;
//                if ( utility > best_offers_utility ) {
//                    //    std::cout << "best seen so far " << utility << ", previous was "<< best_offers_utility<< std::endl;
//                    best_offers = std::move( cheapest_offers );
//                    best_offers_utility = utility;
//                }
//            } else {
//                //std::cout << "can't buy enough packets on this interval " << std::endl;
//            }
//
//            /*
//               if ( -(double) best_interval_length-best_interval_cost > -(double) interval_length ) {
//            // utility can't improve unless slots were negative cost
//            break;
//            }
//             */
        }
        return { };
    }

    void take_actions( Market& mkt, const size_t time ) override
    {
        size_t num_packets_to_buy = num_packets_;
        std::cout << uid_to_string( uid_ ) << " trying to buy " << num_packets_to_buy << " packet deliveries" << std::endl;

        size_t start_time = std::max( time, start_ );
        const std::vector<std::pair<Offer, Opportunity *>> offers_to_take = best_offers( start_time, num_packets_ );

        for ( const std::pair<Offer, Opportunity *> &o : offers_to_take ) {
            std::cout << "User " << uid_to_string( uid_ ) << " buying offer on interval " << opportunity_to_string( o.first.opportunity ) << " for $" << o.first.cost << std::endl;
            bool success = mkt.buy_offer( uid_, o.first, o.second );
            if ( success ) {
                opportunities_.push_back( o.first.opportunity );
            } else {
                assert( false && "thought we could buy offer but we couldn't");
            }
        }
    }
};

#endif /* FLOW_COMPLETION_TIME_USER */

/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ROUND_ROBIN_SIMULATOR_HH
#define ROUND_ROBIN_SIMULATOR_HH

#include <iostream>
#include <vector>
#include <unordered_map>
#include <list>

#include "flow.hh"
#include "opportunity.hh"
#include "link.hh"

const std::unordered_map<Flow, std::vector<Opportunity>> simulate_round_robin( const std::vector<Link> &links, const std::vector<Flow> flows )
{
    assert ( links.size() == 1 );
    Link link = links.at(0);
    const std::vector<Opportunity> opportunities_to_schedule = link.get_opportunities();
    std::unordered_map<Flow, std::vector<Opportunity>> allocation {};
    std::unordered_map<Flow, size_t> packets_sent {};
    
    auto cur_opportunity = opportunities_to_schedule.begin();
    while ( cur_opportunity != opportunities_to_schedule.end() ) {
        bool someone_went = false;
        bool everyone_finished = true;

        for ( const Flow &flow : flows ) {
            bool flow_finished = flow.num_packets == packets_sent[ flow ];
            everyone_finished = everyone_finished and flow_finished;

            bool flow_started = cur_opportunity->interval.start >= flow.start;

            if ( flow_started and not flow_finished ) { // then schedule it
                allocation[ flow ].push_back( *cur_opportunity );
                packets_sent[ flow ]++;
                cur_opportunity++;
                if ( cur_opportunity == opportunities_to_schedule.end() ) {
                    return allocation;
                }
                someone_went = true;
            }
        }

        if ( everyone_finished ) {
            return allocation;
        }
        if ( not someone_went ) { // if nobody could go then advance anyway
            cur_opportunity++;
        }
    }
    return allocation;
}

#endif /* ROUND_ROBIN_SIMULATOR_HH */

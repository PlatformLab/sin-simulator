/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ROUND_ROBIN_SIMULATOR_HH
#define ROUND_ROBIN_SIMULATOR_HH

#include <iostream>
#include <vector>
#include <list>

#include "flow.hh"
#include "link.hh"

const std::vector<Opportunity> simulate_round_robin( const std::vector<Link> &links, std::vector<Flow> flows )
{
    assert ( links.size() == 1 );
    Link link = links.at(0);
    std::vector<Opportunity> allocation = link.get_opportunities();
    
    auto cur_opportunity = allocation.begin();
    while ( cur_opportunity != allocation.end() ) {
        bool someone_went = false;
        bool everyone_finished = true;

        for ( Flow &flow : flows ) {
            bool flow_finished = flow.num_packets == 0;
            everyone_finished = everyone_finished and flow_finished;

            bool flow_started = cur_opportunity->interval.start >= flow.start;

            if ( flow_started and not flow_finished ) { // then schedule it
                cur_opportunity->owner = flow.uid;
                flow.num_packets--;
                cur_opportunity++;
                if ( cur_opportunity == allocation.end() ) {
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

/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ROUND_ROBIN_SIMULATOR_HH
#define ROUND_ROBIN_SIMULATOR_HH

#include <iostream>
#include <vector>
#include <list>

#include "flow.hh"
#include "link.hh"

std::vector<Interval> simulate_round_robin( const std::vector<Link> &links, std::vector<Flow> flows ) {
    assert ( links.size() == 1 );
    Link link = links.at(0);
    std::vector<Interval> allocation = link.get_intervals();
    
    auto cur_interval = allocation.begin();
    while ( cur_interval != allocation.end() )
    {
        bool someone_went = false;
        bool everyone_finished = true;

        for (auto &flow : flows) {
            bool flow_finished = flow.num_packets == 0;
            everyone_finished = everyone_finished and flow_finished;

            bool flow_started = cur_interval->start >= flow.start;

            if (flow_started and not flow_finished) { // then schedule it
                cur_interval->owner = flow.uid;
                flow.num_packets--;
                cur_interval++;
                if ( cur_interval == allocation.end() )
                {
                    return allocation;
                }
                someone_went = true;
            }
        }

        if (everyone_finished) {
            return allocation;
        }
        if (not someone_went) { // if nobody could go then advance anyway
            cur_interval++;
        }
    }
    return allocation;
}

#endif /* ROUND_ROBIN_SIMULATOR_HH */

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
    
    auto flow_iterator = flows.begin();
    for ( Interval &interval : allocation )
    {
        // find a flow we can schedule
        auto flow_to_schedule = flows.end();
        for ( int i = 0; i < flows.size(); i++ )
        {
            flow_iterator++;
            if ( flow_iterator == flows.end() )
            {
                flow_iterator = flows.begin();
            }

            if ( flow_iterator->start <= interval.start ) {
                flow_to_schedule = flow_iterator;
                break;
            }
        }
        if ( flow_to_schedule != flows.end() )
        {
            interval.owner = flow_to_schedule->uid;

            flow_to_schedule->num_packets--;
            if ( flow_to_schedule->num_packets == 0 )
            {
                flows.erase( flow_to_schedule );
            }
        }
    }

    return allocation;
}

#endif /* ROUND_ROBIN_SIMULATOR_HH */

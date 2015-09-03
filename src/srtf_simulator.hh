/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef SRTF_SIMULATOR_HH
#define SRTF_SIMULATOR_HH

#include <iostream>
#include <vector>
#include <list>

#include "flow.hh"
#include "link.hh"

const std::vector<Interval> simulate_srtf( const std::vector<Link> &links, std::vector<Flow> flows )
{
    assert ( links.size() == 1 );
    Link link = links.at(0);
    std::vector<Interval> allocation = link.get_intervals();

    for ( Interval &interval : allocation )
    {
        // first find shortest remaining time flow we can schedule
        auto srtf_flow = flows.end();
        for ( auto potential_flow = flows.begin(); potential_flow != flows.end(); ++potential_flow ) {
            bool can_schedule_flow = potential_flow->start <= interval.start;
            bool flow_is_shortest_remaining = srtf_flow == flows.end() or potential_flow->num_packets < srtf_flow->num_packets;
            if ( can_schedule_flow and flow_is_shortest_remaining ) {
                srtf_flow = potential_flow;
            }
        }

        // we can schedule a flow now (otherwise they all have later start times and we advance time with allocation)
        if ( srtf_flow != flows.end() )
        {
            interval.owner = srtf_flow->uid;

            // decrement packets to be sent for flow we serviced, remove from list completed
            srtf_flow->num_packets--;
            if ( srtf_flow->num_packets == 0 )
            {
                flows.erase( srtf_flow );
            }
        }
    }

    return allocation;
}

#endif /* SRTF_SIMULATOR_HH */

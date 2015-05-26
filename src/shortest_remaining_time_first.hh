/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef SRTF_HH
#define SRTF_HH

#include <iostream>

#include "market_event.hh"
#include "pretty_print.hh"

struct flow {
    std::string name;
    size_t flow_start_time;
    size_t num_packets;
};

const std::list<PacketSent> simulate_shortest_remaining_time_first( std::list<flow> flows )
{
    std::list<PacketSent> final_schedule;
    uint64_t cur_time = 0;

    while ( not flows.empty() )
    {
        auto shortest_remaining_time_flow = flows.end();
        for (auto it = flows.begin(); it != flows.end();  ++it )
        {
            // we can schedule this flow at this time
            if (it->flow_start_time <= cur_time)
            {
                if (shortest_remaining_time_flow == flows.end()
                        || it->num_packets < shortest_remaining_time_flow->num_packets)
                {
                    shortest_remaining_time_flow = it;
                }
            }
        }

        // we can schedule a flow now (otherwise they all have too high of start times
        if (shortest_remaining_time_flow != flows.end())
        {
            final_schedule.push_back( {shortest_remaining_time_flow->name, cur_time} );

            // delete flow from list if flow completed
            shortest_remaining_time_flow->num_packets--;
            if (shortest_remaining_time_flow->num_packets == 0)
            {
                flows.erase(shortest_remaining_time_flow);
            }
        }
        cur_time++;
    }

    // now print results
//    printPacketsSent(final_schedule);
    return final_schedule;
}

#endif /* SRTF_HH */

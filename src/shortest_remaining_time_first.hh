/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef SRTF_HH
#define SRTF_HH

#include "market_event.hh"
#include "schedule.hh"

const std::deque<PacketSent> simulate_shortest_remaining_time_first( std::list<flow> flows )
{
    std::deque<PacketSent> final_schedule;
    uint64_t cur_time = 0;

    while ( not flows.empty() )
    {
        // first find shortest remaining time flow we can schedule
        auto shortest_remaining_time_flow = flows.end();
        for (auto it = flows.begin(); it != flows.end(); ++it) {
            bool can_schedule = it->flow_start_time <= cur_time;
            bool is_shortest = shortest_remaining_time_flow == flows.end() || it->num_packets < shortest_remaining_time_flow->num_packets;
            if ( can_schedule and is_shortest ) {
                shortest_remaining_time_flow = it;
            }
        }

        // we can schedule a flow now (otherwise they all have later start times and we advance time with no packet sent)
        if (shortest_remaining_time_flow != flows.end())
        {
            final_schedule.push_back( {shortest_remaining_time_flow->uid, cur_time} );

            // decrement packets to be sent for flow we serviced, remove from list completed
            shortest_remaining_time_flow->num_packets--;
            if (shortest_remaining_time_flow->num_packets == 0)
            {
                flows.erase(shortest_remaining_time_flow);
            }
        }
        cur_time++;
    }

    return final_schedule;
}

#endif /* SRTF_HH */

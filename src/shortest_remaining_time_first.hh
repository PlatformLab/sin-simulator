/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef SRTF_HH
#define SRTF_HH

#include <iostream>
#include <cassert>
#include <map>

#include "market_event.hh"

struct flow {
    std::string name;
    size_t flow_start_time;
    size_t num_packets;
};

void simulate_shortest_remaining_time_first( std::list<flow> flows )
{
    std::list<PacketSent> schedule;
    uint64_t cur_time = 0;

    while ( not flows.empty() )
    {
        auto shortest_remaining_time_flow = flows.end();
        for (auto it = flows.begin(); it != flows.end();  ++it )
        {
            if (it->flow_start_time <= cur_time)
            {
                if (shortest_remaining_time_flow == flows.end()
                        || it->num_packets < shortest_remaining_time_flow->num_packets)
                {
                    shortest_remaining_time_flow = it;
                }
            }
        }

        if (shortest_remaining_time_flow != flows.end())
        {
            schedule.push_back( {shortest_remaining_time_flow->name, cur_time} );
            shortest_remaining_time_flow->num_packets--;
            if (shortest_remaining_time_flow->num_packets == 0)
            {
                flows.erase(shortest_remaining_time_flow);
            }
        }
        cur_time++;
    }

    // now print results
    std::cout << "[ ";
    for (auto & pkt : schedule){
        std::cout << pkt.time << ". " << pkt.owner << "| ";
    }
    std::cout << "]" << std::endl;
}

#endif /* SRTF_HH */

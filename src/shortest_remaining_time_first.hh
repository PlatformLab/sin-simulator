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

// figures out if a schedule is compatible with shortest remaining time first depending on tie breaking
bool is_shortest_remaining_time_first( std::list<flow> flows, std::list<PacketSent> schedule )
{
    while ( not schedule.empty() )
    {
        std::list<std::list<flow>::iterator> shortest_remaining_time_flows = {};
        bool a_flow_can_send = false;
        for (auto it = flows.begin(); it != flows.end();  ++it )
        {
            // we can schedule this flow at this time
            if (it->flow_start_time <= schedule.front().time)
            {
                a_flow_can_send = true;

                if (shortest_remaining_time_flows.size() == 0
                        || it->num_packets < shortest_remaining_time_flows.front()->num_packets)
                {
                    // a new shortest remaining time
                    shortest_remaining_time_flows = {it};
                } else if (shortest_remaining_time_flows.size() != 0 &&
                        it->num_packets == shortest_remaining_time_flows.front()->num_packets) {
                    // a tie
                    shortest_remaining_time_flows.push_back(it);
                }
            }
        }
        // see if next packet sent was tied for shortest remainign time
        bool next_packet_good = false;
        for (auto flow : shortest_remaining_time_flows){
            if (flow->name == schedule.front().owner) {
                flow->num_packets--;
                if (flow->num_packets == 0)
                {
                    flows.erase(flow);
                }
                next_packet_good = true;
            }
        }
        if (next_packet_good || not a_flow_can_send) {
            schedule.pop_front();
        } else {
            std::cout << "got next packet " << schedule.front().owner << " instead of: ";
            for (auto flow : shortest_remaining_time_flows)
                std::cout << flow->name << " with remaining time " << flow->num_packets << " or ";
            std::cout << " at time " << schedule.front().time << std::endl;
            return false;
        }
    }
    return true;
}

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
